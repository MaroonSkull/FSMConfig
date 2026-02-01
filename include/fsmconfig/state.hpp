#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "fsmconfig/types.hpp"

namespace fsmconfig {

/**
 * @file state.hpp
 * @brief Finite state machine state class
 */

/**
 * @class State
 * @brief Class representing a state in finite state machine
 *
 * State class provides:
 * - Storage of state information
 * - State variable management
 * - Interface for accessing state data
 * - Use of Pimpl idiom for implementation hiding
 */
class State {
 public:
  /**
   * @brief Constructor
   * @param info State information
   */
  explicit State(const StateInfo& info);

  /**
   * @brief Destructor
   */
  ~State();

  // Copy prohibition
  State(const State&) = delete;
  State& operator=(const State&) = delete;

  // Move permission
  /**
   * @brief Move constructor
   * @param other State to move
   */
  State(State&& other) noexcept;

  /**
   * @brief Move assignment operator
   * @param other State to move
   * @return Reference to current object
   */
  State& operator=(State&& other) noexcept;

  /**
   * @brief Get state name
   * @return State name
   */
  const std::string& getName() const;

  /**
   * @brief Get all state variables
   * @return Reference to variables map
   */
  const std::map<std::string, VariableValue>& getVariables() const;

  /**
   * @brief Get on-enter callback
   * @return Callback name
   */
  const std::string& getOnEnterCallback() const;

  /**
   * @brief Get on-exit callback
   * @return Callback name
   */
  const std::string& getOnExitCallback() const;

  /**
   * @brief Get list of state actions
   * @return Reference to actions vector
   */
  const std::vector<std::string>& getActions() const;

  /**
   * @brief Check if variable exists
   * @param name Variable name
   * @return true if variable exists
   */
  bool hasVariable(const std::string& name) const;

  /**
   * @brief Get variable value
   * @param name Variable name
   * @return Variable value
   * @throw StateException If variable does not exist
   */
  VariableValue getVariable(const std::string& name) const;

  /**
   * @brief Set variable value
   * @param name Variable name
   * @param value Variable value
   */
  void setVariable(const std::string& name, const VariableValue& value);

  /**
   * @brief Get all state variables
   * @return Reference to variables map
   */
  const std::map<std::string, VariableValue>& getAllVariables() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace fsmconfig
