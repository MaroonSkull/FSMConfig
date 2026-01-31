#include "fsmconfig/state.hpp"

#include <utility>

namespace fsmconfig {

// ============================================================================
// State::Impl - Pimpl implementation
// ============================================================================

/**
 * @brief Внутренняя реализация класса State (Pimpl идиома)
 */
class State::Impl {
   public:
    /**
     * @brief Конструктор
     * @param info Информация о состоянии
     */
    explicit Impl(const StateInfo& info)
        : name(info.name),
          variables(info.variables),
          on_enter_callback(info.on_enter_callback),
          on_exit_callback(info.on_exit_callback),
          actions(info.actions) {}

    std::string name;                                ///< Имя состояния
    std::map<std::string, VariableValue> variables;  ///< Переменные состояния
    std::string on_enter_callback;                   ///< Коллбэк при входе
    std::string on_exit_callback;                    ///< Коллбэк при выходе
    std::vector<std::string> actions;                ///< Список действий
};

// ============================================================================
// State implementation
// ============================================================================

State::State(const StateInfo& info) : impl_(std::make_unique<Impl>(info)) {}

State::~State() = default;

State::State(State&& other) noexcept = default;

State& State::operator=(State&& other) noexcept = default;

const std::string& State::getName() const { return impl_->name; }

const std::map<std::string, VariableValue>& State::getVariables() const { return impl_->variables; }

const std::string& State::getOnEnterCallback() const { return impl_->on_enter_callback; }

const std::string& State::getOnExitCallback() const { return impl_->on_exit_callback; }

const std::vector<std::string>& State::getActions() const { return impl_->actions; }

bool State::hasVariable(const std::string& name) const {
    return impl_->variables.find(name) != impl_->variables.end();
}

VariableValue State::getVariable(const std::string& name) const {
    auto it = impl_->variables.find(name);
    if (it == impl_->variables.end()) {
        throw StateException("Variable '" + name + "' not found in state '" + impl_->name + "'");
    }
    return it->second;
}

void State::setVariable(const std::string& name, const VariableValue& value) {
    impl_->variables[name] = value;
}

const std::map<std::string, VariableValue>& State::getAllVariables() const {
    return impl_->variables;
}

}  // namespace fsmconfig
