#include "fsmconfig/variable_manager.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

namespace fsmconfig {

/**
 * @brief VariableManager implementation (Pimpl idiom)
 */
class VariableManager::Impl {
 public:
  /// Global variables
  std::map<std::string, VariableValue> global_variables;

  /// State local variables: key = state_name
  std::map<std::string, std::map<std::string, VariableValue>> state_variables;

  /// Mutex for thread safety
  mutable std::mutex mutex;

  /**
   * @brief Clear all variables
   */
  void clear() {
    global_variables.clear();
    state_variables.clear();
  }
};

// ============================================================================
// Constructors and destructor
// ============================================================================

VariableManager::VariableManager() : impl_(std::make_unique<Impl>()) {}

VariableManager::~VariableManager() = default;

VariableManager::VariableManager(VariableManager&& other) noexcept = default;

VariableManager& VariableManager::operator=(VariableManager&& other) noexcept = default;

// ============================================================================
// Variable setter methods
// ============================================================================

void VariableManager::setGlobalVariable(const std::string& name, const VariableValue& value) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  impl_->global_variables[name] = value;
}

void VariableManager::setStateVariable(const std::string& state_name, const std::string& name,
                                       const VariableValue& value) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  impl_->state_variables[state_name][name] = value;
}

// ============================================================================
// Variable getter methods
// ============================================================================

std::optional<VariableValue> VariableManager::getVariable(const std::string& state_name,
                                                          const std::string& name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  // First search for local variable
  auto state_it = impl_->state_variables.find(state_name);
  if (state_it != impl_->state_variables.end()) {
    auto var_it = state_it->second.find(name);
    if (var_it != state_it->second.end()) {
      return var_it->second;
    }
  }

  // If local not found, search for global
  auto global_it = impl_->global_variables.find(name);
  if (global_it != impl_->global_variables.end()) {
    return global_it->second;
  }

  return std::nullopt;
}

std::optional<VariableValue> VariableManager::getGlobalVariable(const std::string& name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  auto it = impl_->global_variables.find(name);
  if (it != impl_->global_variables.end()) {
    return it->second;
  }

  return std::nullopt;
}

std::optional<VariableValue> VariableManager::getStateVariable(const std::string& state_name,
                                                               const std::string& name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  auto state_it = impl_->state_variables.find(state_name);
  if (state_it != impl_->state_variables.end()) {
    auto var_it = state_it->second.find(name);
    if (var_it != state_it->second.end()) {
      return var_it->second;
    }
  }

  return std::nullopt;
}

const std::map<std::string, VariableValue>& VariableManager::getGlobalVariables() const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  return impl_->global_variables;
}

const std::map<std::string, VariableValue>& VariableManager::getStateVariables(const std::string& state_name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  // Return empty map for non-existent state
  static const std::map<std::string, VariableValue> empty_map;

  auto it = impl_->state_variables.find(state_name);
  if (it != impl_->state_variables.end()) {
    return it->second;
  }

  return empty_map;
}

// ============================================================================
// Variable existence check methods
// ============================================================================

bool VariableManager::hasVariable(const std::string& state_name, const std::string& name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  // First check local variable
  auto state_it = impl_->state_variables.find(state_name);
  if (state_it != impl_->state_variables.end()) {
    if (state_it->second.contains(name)) {
      return true;
    }
  }

  // If local not found, check global
  return impl_->global_variables.contains(name);
}

bool VariableManager::hasGlobalVariable(const std::string& name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  return impl_->global_variables.find(name) != impl_->global_variables.end();
}

bool VariableManager::hasStateVariable(const std::string& state_name, const std::string& name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  auto state_it = impl_->state_variables.find(state_name);
  if (state_it != impl_->state_variables.end()) {
    return state_it->second.contains(name);
  }

  return false;
}

// ============================================================================
// Variable removal methods
// ============================================================================

bool VariableManager::removeVariable(const std::string& state_name, const std::string& name) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  // First try to remove local variable
  auto state_it = impl_->state_variables.find(state_name);
  if (state_it != impl_->state_variables.end()) {
    if (state_it->second.erase(name) > 0) {
      return true;
    }
  }

  // If local not found, try to remove global
  return impl_->global_variables.erase(name) > 0;
}

bool VariableManager::removeGlobalVariable(const std::string& name) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  return impl_->global_variables.erase(name) > 0;
}

bool VariableManager::removeStateVariable(const std::string& state_name, const std::string& name) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  auto state_it = impl_->state_variables.find(state_name);
  if (state_it != impl_->state_variables.end()) {
    return state_it->second.erase(name) > 0;
  }

  return false;
}

// ============================================================================
// Variable management methods
// ============================================================================

void VariableManager::clear() {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  impl_->clear();
}

void VariableManager::clearStateVariables(const std::string& state_name) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  impl_->state_variables.erase(state_name);
}

void VariableManager::clearGlobalVariables() {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  impl_->global_variables.clear();
}

void VariableManager::copyStateVariables(const std::string& from_state, const std::string& to_state) {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  auto from_it = impl_->state_variables.find(from_state);
  if (from_it != impl_->state_variables.end()) {
    impl_->state_variables[to_state] = from_it->second;
  }
}

// ============================================================================
// Variable counting methods
// ============================================================================

size_t VariableManager::getGlobalVariableCount() const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);
  return impl_->global_variables.size();
}

size_t VariableManager::getStateVariableCount(const std::string& state_name) const {
  const std::lock_guard<std::mutex> lock(impl_->mutex);

  auto it = impl_->state_variables.find(state_name);
  if (it != impl_->state_variables.end()) {
    return it->second.size();
  }

  return 0;
}

}  // namespace fsmconfig
