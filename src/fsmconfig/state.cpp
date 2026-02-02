#include "fsmconfig/state.hpp"

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "fsmconfig/types.hpp"

namespace fsmconfig {

// ============================================================================
// State::Impl - Pimpl implementation
// ============================================================================

/**
 * @brief Internal implementation of State class (Pimpl idiom)
 */
class State::Impl {
 public:
  /**
   * @brief Constructor
   * @param info State information
   */
  explicit Impl(const StateInfo& info)
      : name(info.name),
        variables(info.variables),
        on_enter_callback(info.on_enter_callback),
        on_exit_callback(info.on_exit_callback),
        actions(info.actions) {}

  std::string name;                                ///< State name
  std::map<std::string, VariableValue> variables;  ///< State variables
  std::string on_enter_callback;                   ///< On-enter callback
  std::string on_exit_callback;                    ///< On-exit callback
  std::vector<std::string> actions;                ///< List of actions
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

bool State::hasVariable(const std::string& name) const { return impl_->variables.contains(name); }

VariableValue State::getVariable(const std::string& name) const {
  auto it = impl_->variables.find(name);
  if (it == impl_->variables.end()) {
    throw StateException("Variable '" + name + "' not found in state '" + impl_->name + "'");
  }
  return it->second;
}

void State::setVariable(const std::string& name, const VariableValue& value) { impl_->variables[name] = value; }

const std::map<std::string, VariableValue>& State::getAllVariables() const { return impl_->variables; }

}  // namespace fsmconfig
