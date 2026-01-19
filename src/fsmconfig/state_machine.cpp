#include "fsmconfig/state_machine.hpp"
#include "fsmconfig/config_parser.hpp"
#include "fsmconfig/callback_registry.hpp"
#include "fsmconfig/event_dispatcher.hpp"
#include "fsmconfig/state.hpp"
#include "fsmconfig/variable_manager.hpp"
#include <algorithm>
#include <stdexcept>

namespace fsmconfig {

/**
 * @brief Реализация StateMachine (Pimpl идиома)
 */
struct StateMachine::Impl {
    std::unique_ptr<ConfigParser> config_parser;
    std::unique_ptr<CallbackRegistry> callback_registry;
    std::unique_ptr<VariableManager> variable_manager;
    std::unique_ptr<EventDispatcher> event_dispatcher;

    std::map<std::string, std::unique_ptr<State>> states;
    std::string current_state;
    std::string initial_state;
    bool started = false;

    std::vector<StateObserver*> observers;
    ErrorHandler error_handler;

    void clear() {
        states.clear();
        current_state.clear();
        initial_state.clear();
        started = false;
    }
};

// Конструкторы и деструктор

StateMachine::StateMachine(const std::string& config_path)
    : impl_(std::make_unique<Impl>()) {
    impl_->config_parser = std::make_unique<ConfigParser>();
    impl_->callback_registry = std::make_unique<CallbackRegistry>();
    impl_->variable_manager = std::make_unique<VariableManager>();
    impl_->event_dispatcher = std::make_unique<EventDispatcher>();

    // Загружаем конфигурацию из файла
    impl_->config_parser->loadFromFile(config_path);

    // Копируем глобальные переменные в VariableManager
    const auto& global_vars = impl_->config_parser->getGlobalVariables();
    for (const auto& [name, value] : global_vars) {
        impl_->variable_manager->setGlobalVariable(name, value);
    }

    // Создаем состояния из конфигурации
    const auto& states_info = impl_->config_parser->getStates();
    for (const auto& [name, info] : states_info) {
        auto state = std::make_unique<State>(info);

        // Копируем переменные состояния в VariableManager
        for (const auto& [var_name, var_value] : info.variables) {
            impl_->variable_manager->setStateVariable(name, var_name, var_value);
        }

        impl_->states[name] = std::move(state);
    }

    // Находим начальное состояние (первое в списке)
    if (!states_info.empty()) {
        impl_->initial_state = states_info.begin()->first;
    }
}

StateMachine::StateMachine(const std::string& yaml_content, bool is_content)
    : impl_(std::make_unique<Impl>()) {
    if (!is_content) {
        throw ConfigException("Second constructor argument must be true when passing YAML content");
    }

    impl_->config_parser = std::make_unique<ConfigParser>();
    impl_->callback_registry = std::make_unique<CallbackRegistry>();
    impl_->variable_manager = std::make_unique<VariableManager>();
    impl_->event_dispatcher = std::make_unique<EventDispatcher>();

    // Загружаем конфигурацию из строки
    impl_->config_parser->loadFromString(yaml_content);

    // Копируем глобальные переменные в VariableManager
    const auto& global_vars = impl_->config_parser->getGlobalVariables();
    for (const auto& [name, value] : global_vars) {
        impl_->variable_manager->setGlobalVariable(name, value);
    }

    // Создаем состояния из конфигурации
    const auto& states_info = impl_->config_parser->getStates();
    for (const auto& [name, info] : states_info) {
        auto state = std::make_unique<State>(info);

        // Копируем переменные состояния в VariableManager
        for (const auto& [var_name, var_value] : info.variables) {
            impl_->variable_manager->setStateVariable(name, var_name, var_value);
        }

        impl_->states[name] = std::move(state);
    }

    // Находим начальное состояние (первое в списке)
    if (!states_info.empty()) {
        impl_->initial_state = states_info.begin()->first;
    }
}

StateMachine::~StateMachine() = default;

StateMachine::StateMachine(StateMachine&& other) noexcept = default;

StateMachine& StateMachine::operator=(StateMachine&& other) noexcept = default;

// Lifecycle методы

void StateMachine::start() {
    if (impl_->started) {
        throw StateException("StateMachine is already started");
    }

    if (impl_->initial_state.empty()) {
        throw StateException("No initial state found in configuration");
    }

    if (!impl_->config_parser->hasState(impl_->initial_state)) {
        throw StateException("Initial state '" + impl_->initial_state + "' not found");
    }

    // Переходим в начальное состояние
    impl_->current_state = impl_->initial_state;

    // Вызываем on_enter коллбэк начального состояния
    const auto& state_info = impl_->config_parser->getState(impl_->current_state);
    if (!state_info.on_enter_callback.empty()) {
        impl_->callback_registry->callStateCallback(impl_->current_state, "on_enter");
    }

    // Выполняем действия начального состояния
    executeStateActions(impl_->current_state);

    // Уведомляем наблюдателей о входе в состояние
    for (auto* observer : impl_->observers) {
        observer->onStateEnter(impl_->current_state);
    }

    impl_->started = true;
}

void StateMachine::stop() {
    if (!impl_->started) {
        return;
    }

    // Вызываем on_exit коллбэк текущего состояния
    if (!impl_->current_state.empty()) {
        const auto& state_info = impl_->config_parser->getState(impl_->current_state);
        if (!state_info.on_exit_callback.empty()) {
            impl_->callback_registry->callStateCallback(impl_->current_state, "on_exit");
        }

        // Уведомляем наблюдателей о выходе из состояния
        for (auto* observer : impl_->observers) {
            observer->onStateExit(impl_->current_state);
        }
    }

    impl_->started = false;
}

void StateMachine::reset() {
    stop();
    impl_->clear();
}

// State query методы

std::string StateMachine::getCurrentState() const {
    return impl_->current_state;
}

bool StateMachine::hasState(const std::string& state_name) const {
    return impl_->states.find(state_name) != impl_->states.end();
}

std::vector<std::string> StateMachine::getAllStates() const {
    std::vector<std::string> result;
    result.reserve(impl_->states.size());
    for (const auto& [name, state] : impl_->states) {
        result.push_back(name);
    }
    return result;
}

// Event handling методы

void StateMachine::triggerEvent(const std::string& event_name) {
    triggerEvent(event_name, {});
}

void StateMachine::triggerEvent(const std::string& event_name, const std::map<std::string, VariableValue>& data) {
    if (!impl_->started) {
        throw StateException("StateMachine is not started");
    }

    if (impl_->current_state.empty()) {
        throw StateException("No current state");
    }

    // Ищем переход для события из текущего состояния
    const TransitionInfo* transition = impl_->config_parser->findTransition(impl_->current_state, event_name);
    if (!transition) {
        throw StateException("No transition found for event '" + event_name + "' from state '" + impl_->current_state + "'");
    }

    // Проверяем guard-условие
    if (!transition->guard_callback.empty()) {
        if (!evaluateGuard(transition->from_state, transition->to_state, event_name)) {
            // Guard вернул false - не выполняем переход
            return;
        }
    }

    // Создаем событие перехода
    TransitionEvent event;
    event.event_name = event_name;
    event.from_state = impl_->current_state;
    event.to_state = transition->to_state;
    event.data = data;
    event.timestamp = std::chrono::system_clock::now();

    // Выполняем переход
    performTransition(event);
}

// Variable management методы

void StateMachine::setVariable(const std::string& name, const VariableValue& value) {
    impl_->variable_manager->setGlobalVariable(name, value);
}

VariableValue StateMachine::getVariable(const std::string& name) const {
    auto value = impl_->variable_manager->getVariable(impl_->current_state, name);
    if (!value) {
        throw StateException("Variable '" + name + "' not found");
    }
    return *value;
}

bool StateMachine::hasVariable(const std::string& name) const {
    return impl_->variable_manager->hasVariable(impl_->current_state, name);
}

// Observer методы

void StateMachine::registerStateObserver(StateObserver* observer) {
    if (observer == nullptr) {
        return;
    }

    // Проверяем, не зарегистрирован ли уже наблюдатель
    auto it = std::find(impl_->observers.begin(), impl_->observers.end(), observer);
    if (it == impl_->observers.end()) {
        impl_->observers.push_back(observer);
    }
}

void StateMachine::unregisterStateObserver(StateObserver* observer) {
    if (observer == nullptr) {
        return;
    }

    auto it = std::find(impl_->observers.begin(), impl_->observers.end(), observer);
    if (it != impl_->observers.end()) {
        impl_->observers.erase(it);
    }
}

// Error handling методы

void StateMachine::setErrorHandler(ErrorHandler handler) {
    impl_->error_handler = handler;
}

// Вспомогательные методы

void StateMachine::performTransition(const TransitionEvent& event) {
    std::string old_state = impl_->current_state;
    std::string new_state = event.to_state;

    // Проверяем, существует ли целевое состояние
    if (!hasState(new_state)) {
        throw StateException("Target state '" + new_state + "' does not exist");
    }

    // Вызываем on_exit коллбэк текущего состояния
    const auto& old_state_info = impl_->config_parser->getState(old_state);
    if (!old_state_info.on_exit_callback.empty()) {
        impl_->callback_registry->callStateCallback(old_state, "on_exit");
    }

    // Уведомляем наблюдателей о выходе из состояния
    for (auto* observer : impl_->observers) {
        observer->onStateExit(old_state);
    }

    // Выполняем действия перехода
    const TransitionInfo* transition = impl_->config_parser->findTransition(old_state, event.event_name);
    if (transition && !transition->actions.empty()) {
        executeTransitionActions(transition->actions);
    }

    // Вызываем коллбэк перехода
    if (transition && !transition->transition_callback.empty()) {
        impl_->callback_registry->callTransitionCallback(old_state, new_state, event);
    }

    // Переключаемся на новое состояние
    impl_->current_state = new_state;

    // Вызываем on_enter коллбэк нового состояния
    const auto& new_state_info = impl_->config_parser->getState(new_state);
    if (!new_state_info.on_enter_callback.empty()) {
        impl_->callback_registry->callStateCallback(new_state, "on_enter");
    }

    // Выполняем действия нового состояния
    executeStateActions(new_state);

    // Уведомляем наблюдателей о входе в новое состояние
    for (auto* observer : impl_->observers) {
        observer->onStateEnter(new_state);
    }

    // Уведомляем наблюдателей о переходе
    for (auto* observer : impl_->observers) {
        observer->onTransition(event);
    }
}

bool StateMachine::evaluateGuard(const std::string& from_state, const std::string& to_state, const std::string& event_name) {
    return impl_->callback_registry->callGuard(from_state, to_state, event_name);
}

void StateMachine::executeStateActions(const std::string& state_name) {
    const auto& state_info = impl_->config_parser->getState(state_name);
    for (const auto& action_name : state_info.actions) {
        impl_->callback_registry->callAction(action_name);
    }
}

void StateMachine::executeTransitionActions(const std::vector<std::string>& actions) {
    for (const auto& action_name : actions) {
        impl_->callback_registry->callAction(action_name);
    }
}

// Вспомогательные методы для регистрации коллбэков (для шаблонных методов)

void StateMachine::registerStateCallbackImpl(
    const std::string& state_name,
    const std::string& callback_type,
    std::function<void()> callback
) {
    impl_->callback_registry->registerStateCallback(state_name, callback_type, callback);
}

void StateMachine::registerTransitionCallbackImpl(
    const std::string& from_state,
    const std::string& to_state,
    std::function<void(const TransitionEvent&)> callback
) {
    impl_->callback_registry->registerTransitionCallback(from_state, to_state, callback);
}

void StateMachine::registerGuardImpl(
    const std::string& from_state,
    const std::string& to_state,
    const std::string& event_name,
    std::function<bool()> callback
) {
    impl_->callback_registry->registerGuard(from_state, to_state, event_name, callback);
}

void StateMachine::registerActionImpl(
    const std::string& action_name,
    std::function<void()> callback
) {
    impl_->callback_registry->registerAction(action_name, callback);
}

} // namespace fsmconfig
