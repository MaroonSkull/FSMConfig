#include "fsmconfig/config_parser.hpp"
#include "fsmconfig/types.hpp"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fsmconfig {

// ============================================================================
// ConfigParser::Impl - Реализация (Pimpl идиома)
// ============================================================================

/**
 * @brief Внутренняя реализация ConfigParser
 */
class ConfigParser::Impl {
public:
    /// Глобальные переменные конфигурации
    std::map<std::string, VariableValue> global_variables;

    /// Карта состояний
    std::map<std::string, StateInfo> states;

    /// Вектор переходов
    std::vector<TransitionInfo> transitions;

    /// Начальное состояние (пустое если не задано явно)
    std::string initial_state;

    /**
     * @brief Очистить все данные конфигурации
     */
    void clear() {
        global_variables.clear();
        states.clear();
        transitions.clear();
        initial_state.clear();
    }
};

// ============================================================================
// Конструкторы и деструктор
// ============================================================================

ConfigParser::ConfigParser()
    : impl_(std::make_unique<Impl>()) {
}

ConfigParser::~ConfigParser() = default;

ConfigParser::ConfigParser(ConfigParser&& other) noexcept
    : impl_(std::move(other.impl_)) {
}

ConfigParser& ConfigParser::operator=(ConfigParser&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

// ============================================================================
// Методы загрузки
// ============================================================================

void ConfigParser::loadFromFile(const std::string& file_path) {
    try {
        // Очистить предыдущую конфигурацию
        impl_->clear();

        // Загрузить YAML из файла
        YAML::Node root = YAML::LoadFile(file_path);

        // Парсить глобальные переменные
        if (root["variables"]) {
            parseGlobalVariables(root["variables"]);
        }

        // Парсить состояния
        if (root["states"]) {
            parseStates(root["states"]);
        }

        // Парсить переходы
        if (root["transitions"]) {
            parseTransitions(root["transitions"]);
        }

        // Парсить начальное состояние
        if (root["initial_state"] && root["initial_state"].IsScalar()) {
            impl_->initial_state = root["initial_state"].Scalar();
        }

        // Валидировать конфигурацию
        validateConfig();

    } catch (const YAML::Exception& e) {
        impl_->clear();
        throw ConfigException(std::string("YAML parsing error: ") + e.what());
    } catch (const std::exception& e) {
        impl_->clear();
        throw ConfigException(std::string("Error loading configuration: ") + e.what());
    }
}

void ConfigParser::loadFromString(const std::string& yaml_content) {
    try {
        // Очистить предыдущую конфигурацию
        impl_->clear();

        // Загрузить YAML из строки
        YAML::Node root = YAML::Load(yaml_content);

        // Парсить глобальные переменные
        if (root["variables"]) {
            parseGlobalVariables(root["variables"]);
        }

        // Парсить состояния
        if (root["states"]) {
            parseStates(root["states"]);
        }

        // Парсить переходы
        if (root["transitions"]) {
            parseTransitions(root["transitions"]);
        }

        // Парсить начальное состояние
        if (root["initial_state"] && root["initial_state"].IsScalar()) {
            impl_->initial_state = root["initial_state"].Scalar();
        }

        // Валидировать конфигурацию
        validateConfig();

    } catch (const YAML::Exception& e) {
        impl_->clear();
        throw ConfigException(std::string("YAML parsing error: ") + e.what());
    } catch (const std::exception& e) {
        impl_->clear();
        throw ConfigException(std::string("Error loading configuration: ") + e.what());
    }
}

// ============================================================================
// Методы доступа к данным
// ============================================================================

const std::map<std::string, VariableValue>& ConfigParser::getGlobalVariables() const {
    return impl_->global_variables;
}

const std::map<std::string, StateInfo>& ConfigParser::getStates() const {
    return impl_->states;
}

const std::vector<TransitionInfo>& ConfigParser::getTransitions() const {
    return impl_->transitions;
}

bool ConfigParser::hasState(const std::string& state_name) const {
    return impl_->states.find(state_name) != impl_->states.end();
}

const StateInfo& ConfigParser::getState(const std::string& state_name) const {
    auto it = impl_->states.find(state_name);
    if (it == impl_->states.end()) {
        throw ConfigException("State '" + state_name + "' not found");
    }
    return it->second;
}

std::vector<TransitionInfo> ConfigParser::getTransitionsFrom(const std::string& state_name) const {
    std::vector<TransitionInfo> result;
    for (const auto& transition : impl_->transitions) {
        if (transition.from_state == state_name) {
            result.push_back(transition);
        }
    }
    return result;
}

const TransitionInfo* ConfigParser::findTransition(
    const std::string& from_state,
    const std::string& event_name) const {

    for (const auto& transition : impl_->transitions) {
        if (transition.from_state == from_state && transition.event_name == event_name) {
            return &transition;
        }
    }
    return nullptr;
}

std::string ConfigParser::getInitialState() const {
    return impl_->initial_state;
}

void ConfigParser::clear() {
    impl_->clear();
}

// ============================================================================
// Вспомогательные методы парсинга
// ============================================================================

VariableValue ConfigParser::parseVariable(const YAML::Node& node) const {
    if (!node.IsDefined() || node.IsNull()) {
        throw ConfigException("Variable node is null or undefined");
    }

    // Проверка на скалярное значение
    if (node.IsScalar()) {
        std::string value = node.Scalar();

        // Попытка распознать тип по строковому представлению
        if (value == "true" || value == "false") {
            return VariableValue(node.as<bool>());
        }

        // Проверка на целое число (включая отрицательные)
        try {
            // Проверяем, что строка состоит из цифр и опционального знака минус в начале
            bool is_integer = true;
            for (size_t i = 0; i < value.size(); ++i) {
                if (i == 0 && value[i] == '-') {
                    continue;  // Минус в начале допустим
                }
                if (!std::isdigit(value[i])) {
                    is_integer = false;
                    break;
                }
            }
            if (is_integer && !value.empty()) {
                return VariableValue(node.as<int>());
            }
        } catch (...) {
            // Игнорируем ошибки при попытке парсинга как int
        }

        // Проверка на вещественное число
        try {
            return VariableValue(node.as<float>());
        } catch (...) {
            // Игнорируем ошибки при попытке парсинга как float
        }

        // По умолчанию - строка
        return VariableValue(value);
    }

    throw ConfigException("Unsupported variable type in YAML");
}

StateInfo ConfigParser::parseState(const std::string& name, const YAML::Node& node) const {
    StateInfo state_info(name);

    // Парсинг переменных состояния
    if (node["variables"] && node["variables"].IsMap()) {
        for (const auto& var_pair : node["variables"]) {
            std::string var_name = var_pair.first.Scalar();
            state_info.variables[var_name] = parseVariable(var_pair.second);
        }
    }

    // Парсинг коллбэка on_enter
    if (node["on_enter"] && node["on_enter"].IsScalar()) {
        state_info.on_enter_callback = node["on_enter"].Scalar();
    }

    // Парсинг коллбэка on_exit
    if (node["on_exit"] && node["on_exit"].IsScalar()) {
        state_info.on_exit_callback = node["on_exit"].Scalar();
    }

    // Парсинг действий
    if (node["actions"] && node["actions"].IsSequence()) {
        for (const auto& action_node : node["actions"]) {
            if (action_node.IsScalar()) {
                state_info.actions.push_back(action_node.Scalar());
            }
        }
    }

    return state_info;
}

TransitionInfo ConfigParser::parseTransition(const YAML::Node& node) const {
    TransitionInfo transition_info;

    // Обязательные поля
    if (!node["from"] || !node["from"].IsScalar()) {
        throw ConfigException("Transition missing required field 'from'");
    }
    transition_info.from_state = node["from"].Scalar();

    if (!node["to"] || !node["to"].IsScalar()) {
        throw ConfigException("Transition missing required field 'to'");
    }
    transition_info.to_state = node["to"].Scalar();

    if (!node["event"] || !node["event"].IsScalar()) {
        throw ConfigException("Transition missing required field 'event'");
    }
    transition_info.event_name = node["event"].Scalar();

    // Опциональные поля
    if (node["guard"] && node["guard"].IsScalar()) {
        transition_info.guard_callback = node["guard"].Scalar();
    }

    if (node["on_transition"] && node["on_transition"].IsScalar()) {
        transition_info.transition_callback = node["on_transition"].Scalar();
    }

    // Парсинг действий при переходе
    if (node["actions"] && node["actions"].IsSequence()) {
        for (const auto& action_node : node["actions"]) {
            if (action_node.IsScalar()) {
                transition_info.actions.push_back(action_node.Scalar());
            }
        }
    }

    return transition_info;
}

// ============================================================================
// Валидация конфигурации
// ============================================================================

void ConfigParser::validateConfig() const {
    // Проверить, что все состояния, указанные в переходах, существуют
    for (const auto& transition : impl_->transitions) {
        if (!hasState(transition.from_state)) {
            throw ConfigException(
                "Transition references non-existent source state: '" +
                transition.from_state + "'");
        }

        if (!hasState(transition.to_state)) {
            throw ConfigException(
                "Transition references non-existent target state: '" +
                transition.to_state + "'");
        }
    }

    // Проверить, что нет дублирующихся переходов (from_state, event)
    std::map<std::string, std::set<std::string>> state_events;
    for (const auto& transition : impl_->transitions) {
        auto& events = state_events[transition.from_state];
        if (events.find(transition.event_name) != events.end()) {
            throw ConfigException(
                "Duplicate transition from state '" + transition.from_state +
                "' with event '" + transition.event_name + "'");
        }
        events.insert(transition.event_name);
    }
}

// ============================================================================
// Приватные вспомогательные методы
// ============================================================================

void ConfigParser::parseGlobalVariables(const YAML::Node& node) {
    if (!node.IsMap()) {
        throw ConfigException("'variables' section must be a map");
    }

    for (const auto& var_pair : node) {
        std::string var_name = var_pair.first.Scalar();
        impl_->global_variables[var_name] = parseVariable(var_pair.second);
    }
}

void ConfigParser::parseStates(const YAML::Node& node) {
    if (!node.IsMap()) {
        throw ConfigException("'states' section must be a map");
    }

    bool first_state = true;
    for (const auto& state_pair : node) {
        std::string state_name = state_pair.first.Scalar();
        impl_->states[state_name] = parseState(state_name, state_pair.second);

        // Если начальное состояние не задано явно, используем первое состояние
        if (first_state && impl_->initial_state.empty()) {
            impl_->initial_state = state_name;
        }
        first_state = false;
    }
}

void ConfigParser::parseTransitions(const YAML::Node& node) {
    if (!node.IsSequence()) {
        throw ConfigException("'transitions' section must be a sequence");
    }

    for (const auto& transition_node : node) {
        impl_->transitions.push_back(parseTransition(transition_node));
    }
}

} // namespace fsmconfig
