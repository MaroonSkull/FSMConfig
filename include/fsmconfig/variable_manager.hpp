#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "types.hpp"

namespace fsmconfig {

/// Variable scope
enum class VariableScope {
  GLOBAL,      ///< Global variable (available in all states)
  STATE_LOCAL  ///< State local variable
};

/**
 * @brief Variable manager for finite state machine
 *
 * VariableManager provides management of two types of variables:
 * - Global variables (available in all states)
 * - State local variables (available only in specific state)
 *
 * Local variables have priority over global variables when searching.
 * All operations are thread-safe thanks to std::mutex usage.
 */
class VariableManager {
 public:
  /**
   * @brief Default constructor
   */
  VariableManager();

  /**
   * @brief Destructor
   */
  ~VariableManager();

  // Copy prohibition
  VariableManager(const VariableManager&) = delete;
  VariableManager& operator=(const VariableManager&) = delete;

  // Move permission
  VariableManager(VariableManager&& other) noexcept;
  VariableManager& operator=(VariableManager&& other) noexcept;

  /**
   * @brief Set global variable
   * @param name Variable name
   * @param value Variable value
   */
  void setGlobalVariable(const std::string& name, const VariableValue& value);

  /**
   * @brief Set state local variable
   * @param state_name State name
   * @param name Variable name
   * @param value Variable value
   */
  void setStateVariable(const std::string& state_name, const std::string& name, const VariableValue& value);

  /**
   * @brief Get variable (searches local first, then global)
   * @param state_name State name for searching local variable
   * @param name Variable name
   * @return Variable value or std::nullopt if not found
   *
   * Local variables have priority over global variables.
   */
  [[nodiscard]] std::optional<VariableValue> getVariable(const std::string& state_name, const std::string& name) const;

  /**
   * @brief Get global variable
   * @param name Variable name
   * @return Variable value or std::nullopt if not found
   */
  [[nodiscard]] std::optional<VariableValue> getGlobalVariable(const std::string& name) const;

  /**
   * @brief Get state local variable
   * @param state_name State name
   * @param name Variable name
   * @return Variable value or std::nullopt if not found
   */
  [[nodiscard]] std::optional<VariableValue> getStateVariable(const std::string& state_name,
                                                              const std::string& name) const;

  /**
   * @brief Check if variable exists (searches local first, then global)
   * @param state_name State name for checking local variable
   * @param name Variable name
   * @return true if variable exists
   *
   * Local variables have priority over global variables.
   */
  [[nodiscard]] bool hasVariable(const std::string& state_name, const std::string& name) const;

  /**
   * @brief Check if global variable exists
   * @param name Variable name
   * @return true if global variable exists
   */
  [[nodiscard]] bool hasGlobalVariable(const std::string& name) const;

  /**
   * @brief Check if state local variable exists
   * @param state_name State name
   * @param name Variable name
   * @return true if local variable exists
   */
  [[nodiscard]] bool hasStateVariable(const std::string& state_name, const std::string& name) const;

  /**
   * @brief Remove variable (searches local first, then global)
   * @param state_name State name for removing local variable
   * @param name Variable name
   * @return true if variable was removed, false if it didn't exist
   *
   * First tries to remove local variable, then global.
   */
  bool removeVariable(const std::string& state_name, const std::string& name);

  /**
   * @brief Remove global variable
   * @param name Variable name
   * @return true if variable was removed, false if it didn't exist
   */
  bool removeGlobalVariable(const std::string& name);

  /**
   * @brief Remove state local variable
   * @param state_name State name
   * @param name Variable name
   * @return true if variable was removed, false if it didn't exist
   */
  bool removeStateVariable(const std::string& state_name, const std::string& name);

  /**
   * @brief Get all global variables
   * @return Copy of global variables map
   *
   * Returns a copy to ensure thread safety. The returned map is a snapshot
   * of the variables at the time of the call and will not reflect subsequent changes.
   */
  [[nodiscard]] std::map<std::string, VariableValue> getGlobalVariables() const;

  /**
   * @brief Get all state local variables
   * @param state_name State name
   * @return Copy of state local variables map
   *
   * Returns a copy to ensure thread safety. The returned map is a snapshot
   * of the variables at the time of the call and will not reflect subsequent changes.
   */
  [[nodiscard]] std::map<std::string, VariableValue> getStateVariables(const std::string& state_name) const;

  /**
   * @brief Clear all variables (global and local)
   */
  void clear();

  /**
   * @brief Clear state local variables
   * @param state_name State name
   */
  void clearStateVariables(const std::string& state_name);

  /**
   * @brief Clear global variables
   */
  void clearGlobalVariables();

  /**
   * @brief Copy variables from one state to another
   * @param from_state Source state name
   * @param to_state Target state name
   *
   * Copies all local variables from one state to another.
   * If target state already has variables, they will be overwritten.
   */
  void copyStateVariables(const std::string& from_state, const std::string& to_state);

  /**
   * @brief Get number of global variables
   * @return Number of global variables
   */
  [[nodiscard]] size_t getGlobalVariableCount() const;

  /**
   * @brief Get number of state local variables
   * @param state_name State name
   * @return Number of state local variables
   */
  [[nodiscard]] size_t getStateVariableCount(const std::string& state_name) const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace fsmconfig
