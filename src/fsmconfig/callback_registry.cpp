#include "fsmconfig/callback_registry.hpp"

#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace fsmconfig {

/**
 * @brief CallbackRegistry implementation (Pimpl idiom)
 */
class CallbackRegistry::Impl {
 public:
  /// State callbacks: key = "state_name:callback_type"
  std::map<std::string, StateCallback> state_callbacks;

  /// Transition callbacks: key = "from_state:to_state"
  std::map<std::string, TransitionCallback> transition_callbacks;

  /// Guard callbacks: key = "from_state:to_state:event_name"
  std::map<std::string, GuardCallback> guards;

  /// Action callbacks: key = "action_name"
  std::map<std::string, ActionCallback> actions;

  /// Mutex for thread safety
  mutable std::mutex mutex;

  /**
   * @brief Clear all callbacks
   */
  void clear() {
    state_callbacks.clear();
    transition_callbacks.clear();
    guards.clear();
    actions.clear();
  }
};

// ============================================================================
// Constructors and destructor
// ============================================================================

CallbackRegistry::CallbackRegistry() : impl_(std::make_unique<Impl>()) {}

CallbackRegistry::~CallbackRegistry() = default;

CallbackRegistry::CallbackRegistry(CallbackRegistry&& other) noexcept : impl_(std::move(other.impl_)) {}

CallbackRegistry& CallbackRegistry::operator=(CallbackRegistry&& other) noexcept {
  if (this != &other) {
    impl_ = std::move(other.impl_);
  }
  return *this;
}

// ============================================================================
// Registration methods
// ============================================================================

void CallbackRegistry::registerStateCallback(const std::string& state_name, const std::string& callback_type,
                                             StateCallback callback) {
  if (!callback) {
    return;
  }

  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeStateCallbackKey(state_name, callback_type);
  impl_->state_callbacks[key] = std::move(callback);
}

void CallbackRegistry::registerTransitionCallback(const std::string& from_state, const std::string& to_state,
                                                  TransitionCallback callback) {
  if (!callback) {
    return;
  }

  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeTransitionCallbackKey(from_state, to_state);
  impl_->transition_callbacks[key] = std::move(callback);
}

void CallbackRegistry::registerGuard(const std::string& from_state, const std::string& to_state,
                                     const std::string& event_name, GuardCallback callback) {
  if (!callback) {
    return;
  }

  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeGuardKey(from_state, to_state, event_name);
  impl_->guards[key] = std::move(callback);
}

void CallbackRegistry::registerAction(const std::string& action_name, ActionCallback callback) {
  if (!callback) {
    return;
  }

  const std::scoped_lock lock(impl_->mutex);
  impl_->actions[action_name] = std::move(callback);
}

// ============================================================================
// Invocation methods
// ============================================================================

void CallbackRegistry::callStateCallback(const std::string& state_name, const std::string& callback_type) const {
  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeStateCallbackKey(state_name, callback_type);

  auto iter = impl_->state_callbacks.find(key);
  if (iter != impl_->state_callbacks.end() && iter->second) {
    iter->second();
  }
}

void CallbackRegistry::callTransitionCallback(const std::string& from_state, const std::string& to_state,
                                              const TransitionEvent& event) const {
  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeTransitionCallbackKey(from_state, to_state);

  auto iter = impl_->transition_callbacks.find(key);
  if (iter != impl_->transition_callbacks.end() && iter->second) {
    iter->second(event);
  }
}

bool CallbackRegistry::callGuard(const std::string& from_state, const std::string& to_state,
                                 const std::string& event_name) const {
  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeGuardKey(from_state, to_state, event_name);

  auto iter = impl_->guards.find(key);
  if (iter != impl_->guards.end() && iter->second) {
    return iter->second();
  }
  // If guard is not registered, deny transition
  return false;
}

void CallbackRegistry::callAction(const std::string& action_name) const {
  const std::scoped_lock lock(impl_->mutex);

  auto iter = impl_->actions.find(action_name);
  if (iter != impl_->actions.end() && iter->second) {
    iter->second();
  }
}

// ============================================================================
// Check methods
// ============================================================================

bool CallbackRegistry::hasStateCallback(const std::string& state_name, const std::string& callback_type) const {
  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeStateCallbackKey(state_name, callback_type);

  auto iter = impl_->state_callbacks.find(key);
  return iter != impl_->state_callbacks.end() && static_cast<bool>(iter->second);
}

bool CallbackRegistry::hasTransitionCallback(const std::string& from_state, const std::string& to_state) const {
  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeTransitionCallbackKey(from_state, to_state);

  auto iter = impl_->transition_callbacks.find(key);
  return iter != impl_->transition_callbacks.end() && static_cast<bool>(iter->second);
}

bool CallbackRegistry::hasGuard(const std::string& from_state, const std::string& to_state,
                                const std::string& event_name) const {
  const std::scoped_lock lock(impl_->mutex);
  const std::string key = makeGuardKey(from_state, to_state, event_name);

  auto iter = impl_->guards.find(key);
  return iter != impl_->guards.end() && static_cast<bool>(iter->second);
}

bool CallbackRegistry::hasAction(const std::string& action_name) const {
  const std::scoped_lock lock(impl_->mutex);

  auto iter = impl_->actions.find(action_name);
  return iter != impl_->actions.end() && static_cast<bool>(iter->second);
}

// ============================================================================
// Management methods
// ============================================================================

void CallbackRegistry::clear() {
  const std::scoped_lock lock(impl_->mutex);
  impl_->clear();
}

size_t CallbackRegistry::getStateCallbackCount() const {
  const std::scoped_lock lock(impl_->mutex);
  return impl_->state_callbacks.size();
}

size_t CallbackRegistry::getTransitionCallbackCount() const {
  const std::scoped_lock lock(impl_->mutex);
  return impl_->transition_callbacks.size();
}

size_t CallbackRegistry::getGuardCount() const {
  const std::scoped_lock lock(impl_->mutex);
  return impl_->guards.size();
}

size_t CallbackRegistry::getActionCount() const {
  const std::scoped_lock lock(impl_->mutex);
  return impl_->actions.size();
}

// ============================================================================
// Helper methods
// ============================================================================

std::string CallbackRegistry::makeStateCallbackKey(const std::string& state_name,
                                                   const std::string& callback_type) const {
  return state_name + ":" + callback_type;
}

std::string CallbackRegistry::makeTransitionCallbackKey(const std::string& from_state,
                                                        const std::string& to_state) const {
  return from_state + ":" + to_state;
}

std::string CallbackRegistry::makeGuardKey(const std::string& from_state, const std::string& to_state,
                                           const std::string& event_name) const {
  return from_state + ":" + to_state + ":" + event_name;
}

}  // namespace fsmconfig
