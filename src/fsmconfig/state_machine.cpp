#include "fsmconfig/state_machine.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "fsmconfig/callback_registry.hpp"
#include "fsmconfig/config_parser.hpp"
#include "fsmconfig/event_dispatcher.hpp"
#include "fsmconfig/state.hpp"
#include "fsmconfig/variable_manager.hpp"

namespace fsmconfig {

/**
 * @brief StateMachine implementation (Pimpl idiom)
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
    current_state.clear();
    initial_state.clear();
    started = false;
  }
};

// Constructors and destructor

StateMachine::StateMachine(const std::string& config_path) : impl_(std::make_unique<Impl>()) {
  impl_->config_parser = std::make_unique<ConfigParser>();
  impl_->callback_registry = std::make_unique<CallbackRegistry>();
  impl_->variable_manager = std::make_unique<VariableManager>();
  impl_->event_dispatcher = std::make_unique<EventDispatcher>();

  // Load configuration from file
  impl_->config_parser->loadFromFile(config_path);

  // Copy global variables to VariableManager
  const auto& global_vars = impl_->config_parser->getGlobalVariables();
  for (const auto& [name, value] : global_vars) {
    impl_->variable_manager->setGlobalVariable(name, value);
  }

  // Create states from configuration
  const auto& states_info = impl_->config_parser->getStates();
  for (const auto& [name, info] : states_info) {
    auto state = std::make_unique<State>(info);

    // Copy state variables to VariableManager
    for (const auto& [var_name, var_value] : info.variables) {
      impl_->variable_manager->setStateVariable(name, var_name, var_value);
    }

    impl_->states[name] = std::move(state);
  }

  // Find initial state from configuration
  const std::string initial_state = impl_->config_parser->getInitialState();
  impl_->initial_state = initial_state;  // Save even if empty
}

StateMachine::StateMachine(const std::string& yaml_content, bool is_content) : impl_(std::make_unique<Impl>()) {
  if (!is_content) {
    throw ConfigException("Second constructor argument must be true when passing YAML content");
  }

  impl_->config_parser = std::make_unique<ConfigParser>();
  impl_->callback_registry = std::make_unique<CallbackRegistry>();
  impl_->variable_manager = std::make_unique<VariableManager>();
  impl_->event_dispatcher = std::make_unique<EventDispatcher>();

  // Load configuration from string
  impl_->config_parser->loadFromString(yaml_content);

  // Copy global variables to VariableManager
  const auto& global_vars = impl_->config_parser->getGlobalVariables();
  for (const auto& [name, value] : global_vars) {
    impl_->variable_manager->setGlobalVariable(name, value);
  }

  // Create states from configuration
  const auto& states_info = impl_->config_parser->getStates();
  for (const auto& [name, info] : states_info) {
    auto state = std::make_unique<State>(info);

    // Copy state variables to VariableManager
    for (const auto& [var_name, var_value] : info.variables) {
      impl_->variable_manager->setStateVariable(name, var_name, var_value);
    }

    impl_->states[name] = std::move(state);
  }

  // Find initial state from configuration
  const std::string initial_state = impl_->config_parser->getInitialState();
  impl_->initial_state = initial_state;  // Save even if empty
}

StateMachine::~StateMachine() = default;

StateMachine::StateMachine(StateMachine&& other) noexcept = default;

StateMachine& StateMachine::operator=(StateMachine&& other) noexcept = default;

// Lifecycle methods

void StateMachine::start() {
  if (impl_->started) {
    const std::string error = "StateMachine is already started";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  if (impl_->initial_state.empty()) {
    const std::string error = "No initial state found in configuration";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  if (!impl_->config_parser->hasState(impl_->initial_state)) {
    const std::string error = "Initial state '" + impl_->initial_state + "' not found";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  // Transition to initial state
  impl_->current_state = impl_->initial_state;

  // Call on_enter callback of initial state
  // Check for callback in registry, not just in configuration
  if (impl_->callback_registry->hasStateCallback(impl_->current_state, "on_enter")) {
    impl_->callback_registry->callStateCallback(impl_->current_state, "on_enter");
  }

  // Execute initial state actions
  executeStateActions(impl_->current_state);

  // Notify observers about entering initial state
  for (auto* observer : impl_->observers) {
    observer->onStateEnter(impl_->current_state);
  }

  impl_->started = true;
}

void StateMachine::stop() {
  if (!impl_->started) {
    const std::string error = "StateMachine is not started";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  // Call on_exit callback of current state
  if (!impl_->current_state.empty()) {
    impl_->callback_registry->callStateCallback(impl_->current_state, "on_exit");

    // Notify observers about exiting state
    for (auto* observer : impl_->observers) {
      observer->onStateExit(impl_->current_state);
    }
  }

  impl_->started = false;
}

void StateMachine::reset() {
  if (impl_->started) {
    stop();
  }
  // Save initial state
  std::string saved_initial_state = impl_->initial_state;
  impl_->clear();
  // Restore initial state
  impl_->initial_state = saved_initial_state;
}

// State query methods

std::string StateMachine::getCurrentState() const { return impl_->current_state; }

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

// Event handling methods

void StateMachine::triggerEvent(const std::string& event_name) { triggerEvent(event_name, {}); }

void StateMachine::triggerEvent(const std::string& event_name, const std::map<std::string, VariableValue>& data) {
  if (!impl_->started) {
    const std::string error = "StateMachine is not started";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  if (impl_->current_state.empty()) {
    const std::string error = "No current state";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  // Look for transition for event from current state
  const TransitionInfo* transition = impl_->config_parser->findTransition(impl_->current_state, event_name);
  if (!transition) {
    // Ignore event if transition not found
    return;
  }

  // Check guard condition
  if (!transition->guard_callback.empty()) {
    if (!evaluateGuard(transition->from_state, transition->to_state, event_name)) {
      // Guard returned false - don't perform transition
      return;
    }
  }

  // Create transition event
  TransitionEvent event;
  event.event_name = event_name;
  event.from_state = impl_->current_state;
  event.to_state = transition->to_state;
  event.data = data;
  event.timestamp = std::chrono::system_clock::now();

  // Perform transition
  performTransition(event);
}

// Variable management methods

void StateMachine::setVariable(const std::string& name, const VariableValue& value) {
  // If there is a current state, set state local variable
  if (!impl_->current_state.empty()) {
    impl_->variable_manager->setStateVariable(impl_->current_state, name, value);
  } else {
    // Otherwise set global variable
    impl_->variable_manager->setGlobalVariable(name, value);
  }
}

VariableValue StateMachine::getVariable(const std::string& name) const {
  auto value = impl_->variable_manager->getVariable(impl_->current_state, name);
  if (!value) {
    const std::string error = "Variable '" + name + "' not found";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }
  return *value;
}

bool StateMachine::hasVariable(const std::string& name) const {
  return impl_->variable_manager->hasVariable(impl_->current_state, name);
}

// Observer methods

void StateMachine::registerStateObserver(StateObserver* observer) {
  if (observer == nullptr) {
    return;
  }

  // Check if observer is already registered
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

// Error handling methods

void StateMachine::setErrorHandler(ErrorHandler handler) { impl_->error_handler = handler; }

// Helper methods

void StateMachine::performTransition(const TransitionEvent& event) {
  const std::string old_state = impl_->current_state;
  const std::string new_state = event.to_state;

  // Check if target state exists
  if (!hasState(new_state)) {
    const std::string error = "Target state '" + new_state + "' does not exist";
    if (impl_->error_handler) {
      impl_->error_handler(error);
    }
    throw StateException(error);
  }

  // Call on_exit callback of current state
  const auto& old_state_info = impl_->config_parser->getState(old_state);
  if (!old_state_info.on_exit_callback.empty()) {
    impl_->callback_registry->callStateCallback(old_state, "on_exit");
  }

  // Notify observers about exiting state
  for (auto* observer : impl_->observers) {
    observer->onStateExit(old_state);
  }

  // Execute transition actions
  const TransitionInfo* transition = impl_->config_parser->findTransition(old_state, event.event_name);
  if (transition && !transition->actions.empty()) {
    executeTransitionActions(transition->actions);
  }

  // Call transition callback
  if (transition && !transition->transition_callback.empty()) {
    impl_->callback_registry->callTransitionCallback(old_state, new_state, event);
  }

  // Switch to new state
  impl_->current_state = new_state;

  // Call on_enter callback of new state
  // Check for callback in registry, not just in configuration
  if (impl_->callback_registry->hasStateCallback(new_state, "on_enter")) {
    impl_->callback_registry->callStateCallback(new_state, "on_enter");
  }

  // Execute new state actions
  executeStateActions(new_state);

  // Notify observers about entering new state
  for (auto* observer : impl_->observers) {
    observer->onStateEnter(new_state);
  }

  // Notify observers about transition
  for (auto* observer : impl_->observers) {
    observer->onTransition(event);
  }
}

bool StateMachine::evaluateGuard(const std::string& from_state, const std::string& to_state,
                                 const std::string& event_name) {
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

// Helper methods for callback registration (for template methods)

void StateMachine::registerStateCallbackImpl(const std::string& state_name, const std::string& callback_type,
                                             std::function<void()> callback) {
  impl_->callback_registry->registerStateCallback(state_name, callback_type, callback);
}

void StateMachine::registerTransitionCallbackImpl(const std::string& from_state, const std::string& to_state,
                                                  std::function<void(const TransitionEvent&)> callback) {
  impl_->callback_registry->registerTransitionCallback(from_state, to_state, callback);
}

void StateMachine::registerGuardImpl(const std::string& from_state, const std::string& to_state,
                                     const std::string& event_name, std::function<bool()> callback) {
  impl_->callback_registry->registerGuard(from_state, to_state, event_name, callback);
}

void StateMachine::registerActionImpl(const std::string& action_name, std::function<void()> callback) {
  impl_->callback_registry->registerAction(action_name, callback);
}

}  // namespace fsmconfig
