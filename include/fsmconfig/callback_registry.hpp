#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

#include "types.hpp"

namespace fsmconfig {

/**
 * @file callback_registry.hpp
 * @brief Callback registry for finite state machine
 */

/**
 * @brief State callback type (on_enter, on_exit)
 */
using StateCallback = std::function<void()>;

/**
 * @brief Transition callback type
 */
using TransitionCallback = std::function<void(const TransitionEvent&)>;

/**
 * @brief Guard callback type (returns bool)
 */
using GuardCallback = std::function<bool()>;

/**
 * @brief Action callback type
 */
using ActionCallback = std::function<void()>;

/**
 * @class CallbackRegistry
 * @brief Callback registry for finite state machine
 *
 * CallbackRegistry provides:
 * - Registration of state callbacks (on_enter, on_exit)
 * - Registration of transition callbacks
 * - Registration of guard callbacks for condition checking
 * - Registration of action callbacks
 * - Execution of registered callbacks
 * - Thread safety when working with callbacks
 */
class CallbackRegistry {
 public:
  /**
   * @brief Default constructor
   */
  CallbackRegistry();

  /**
   * @brief Destructor
   */
  ~CallbackRegistry();

  // Copy prohibition
  CallbackRegistry(const CallbackRegistry&) = delete;
  CallbackRegistry& operator=(const CallbackRegistry&) = delete;

  // Move permission
  /**
   * @brief Move constructor
   */
  CallbackRegistry(CallbackRegistry&& other) noexcept;

  /**
   * @brief Move assignment operator
   */
  CallbackRegistry& operator=(CallbackRegistry&& other) noexcept;

  /**
   * @brief Register state callback
   * @param state_name State name
   * @param callback_type Callback type (e.g., "on_enter", "on_exit")
   * @param callback Callback function
   */
  void registerStateCallback(const std::string& state_name, const std::string& callback_type, StateCallback callback);

  /**
   * @brief Register transition callback
   * @param from_state Source state
   * @param to_state Target state
   * @param callback Callback function
   */
  void registerTransitionCallback(const std::string& from_state, const std::string& to_state,
                                  TransitionCallback callback);

  /**
   * @brief Register guard callback
   * @param from_state Source state
   * @param to_state Target state
   * @param event_name Event name
   * @param callback Guard callback function
   */
  void registerGuard(const std::string& from_state, const std::string& to_state, const std::string& event_name,
                     GuardCallback callback);

  /**
   * @brief Register action callback
   * @param action_name Action name
   * @param callback Callback function
   */
  void registerAction(const std::string& action_name, ActionCallback callback);

  /**
   * @brief Call state callback
   * @param state_name State name
   * @param callback_type Callback type
   */
  void callStateCallback(const std::string& state_name, const std::string& callback_type) const;

  /**
   * @brief Call transition callback
   * @param from_state Source state
   * @param to_state Target state
   * @param event Transition event
   */
  void callTransitionCallback(const std::string& from_state, const std::string& to_state,
                              const TransitionEvent& event) const;

  /**
   * @brief Call guard callback
   * @param from_state Source state
   * @param to_state Target state
   * @param event_name Event name
   * @return true if guard exists and returned true, otherwise false
   */
  [[nodiscard]] bool callGuard(const std::string& from_state, const std::string& to_state, const std::string& event_name) const;

  /**
   * @brief Call action callback
   * @param action_name Action name
   */
  void callAction(const std::string& action_name) const;

  /**
   * @brief Check if state callback exists
   * @param state_name State name
   * @param callback_type Callback type
   * @return true if callback is registered
   */
  [[nodiscard]] bool hasStateCallback(const std::string& state_name, const std::string& callback_type) const;

  /**
   * @brief Check if transition callback exists
   * @param from_state Source state
   * @param to_state Target state
   * @return true if callback is registered
   */
  [[nodiscard]] bool hasTransitionCallback(const std::string& from_state, const std::string& to_state) const;

  /**
   * @brief Check if guard callback exists
   * @param from_state Source state
   * @param to_state Target state
   * @param event_name Event name
   * @return true if guard is registered
   */
  [[nodiscard]] bool hasGuard(const std::string& from_state, const std::string& to_state, const std::string& event_name) const;

  /**
   * @brief Check if action callback exists
   * @param action_name Action name
   * @return true if callback is registered
   */
  [[nodiscard]] bool hasAction(const std::string& action_name) const;

  /**
   * @brief Clear all callbacks
   */
  void clear();

  /**
   * @brief Get number of registered state callbacks
   * @return Number of state callbacks
   */
  [[nodiscard]] size_t getStateCallbackCount() const;

  /**
   * @brief Get number of registered transition callbacks
   * @return Number of transition callbacks
   */
  [[nodiscard]] size_t getTransitionCallbackCount() const;

  /**
   * @brief Get number of registered guard callbacks
   * @return Number of guard callbacks
   */
  [[nodiscard]] size_t getGuardCount() const;

  /**
   * @brief Get number of registered action callbacks
   * @return Number of action callbacks
   */
  [[nodiscard]] size_t getActionCount() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  /**
   * @brief Helper method to create state callback key
   * @param state_name State name
   * @param callback_type Callback type
   * @return Key in format "state_name:callback_type"
   */
  [[nodiscard]] std::string makeStateCallbackKey(const std::string& state_name, const std::string& callback_type) const;

  /**
   * @brief Helper method to create transition callback key
   * @param from_state Source state
   * @param to_state Target state
   * @return Key in format "from_state:to_state"
   */
  [[nodiscard]] std::string makeTransitionCallbackKey(const std::string& from_state, const std::string& to_state) const;

  /**
   * @brief Helper method to create guard callback key
   * @param from_state Source state
   * @param to_state Target state
   * @param event_name Event name
   * @return Key in format "from_state:to_state:event_name"
   */
  [[nodiscard]] std::string makeGuardKey(const std::string& from_state, const std::string& to_state,
                           const std::string& event_name) const;
};

}  // namespace fsmconfig
