#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "types.hpp"

namespace fsmconfig {

// Forward declarations
class ConfigParser;
class CallbackRegistry;
class VariableManager;
class EventDispatcher;
class State;

/**
 * @file state_machine.hpp
 * @brief Main StateMachine finite state machine class
 */

/**
 * @class StateMachine
 * @brief Finite state machine class for managing states and transitions
 *
 * StateMachine is the main class of the library, providing:
 * - Loading configuration from YAML files or strings
 * - Registration and execution of callbacks
 * - Event dispatching
 * - State variable management
 * - State transitions with guard condition support
 * - State change observation through StateObserver
 */
class StateMachine {
   public:
    /**
     * @brief Constructor from configuration file path
     * @param config_path Path to YAML configuration file
     * @throws ConfigException on load or parse errors
     */
    explicit StateMachine(const std::string& config_path);

    /**
     * @brief Constructor from YAML content string
     * @param yaml_content String with YAML configuration
     * @param is_content Flag indicating that the first parameter is content, not a path
     * @throws ConfigException on parse errors
     */
    explicit StateMachine(const std::string& yaml_content, bool is_content);

    /**
     * @brief Destructor
     */
    ~StateMachine();

    // Copy prohibition
    StateMachine(const StateMachine&) = delete;
    StateMachine& operator=(const StateMachine&) = delete;

    // Move permission
    /**
     * @brief Move constructor
     * @param other StateMachine object to move
     */
    StateMachine(StateMachine&& other) noexcept;

    /**
     * @brief Move assignment operator
     * @param other StateMachine object to move
     * @return Reference to current object
     */
    StateMachine& operator=(StateMachine&& other) noexcept;

    // Lifecycle

    /**
     * @brief Start the finite state machine
     *
     * Transitions to initial state, calls on_enter callback,
     * executes state actions and notifies observers.
     *
     * @throws StateException if machine is already running or initial state not found
     */
    void start();

    /**
     * @brief Stop the finite state machine
     *
     * Calls on_exit callback of current state and notifies observers.
     */
    void stop();

    /**
     * @brief Reset finite state machine to initial state
     *
     * Stops the machine if running and resets all states.
     */
    void reset();

    // State queries

    /**
     * @brief Get current state name
     * @return Current state name
     */
    std::string getCurrentState() const;

    /**
     * @brief Check if state exists
     * @param state_name State name
     * @return true if state exists
     */
    bool hasState(const std::string& state_name) const;

    /**
     * @brief Get list of all states
     * @return Vector of all state names
     */
    std::vector<std::string> getAllStates() const;

    // Event handling

    /**
     * @brief Trigger event without data
     * @param event_name Event name
     * @throws StateException if machine is not running or transition not found
     */
    void triggerEvent(const std::string& event_name);

    /**
     * @brief Trigger event with data
     * @param event_name Event name
     * @param data Event data
     * @throws StateException if machine is not running or transition not found
     */
    void triggerEvent(const std::string& event_name,
                      const std::map<std::string, VariableValue>& data);

    // Callback registration (template methods)

    /**
     * @brief Register state callback
     * @tparam T Class object type
     * @param state_name State name
     * @param callback_type Callback type (e.g., "on_enter", "on_exit")
     * @param callback Callback method
     * @param instance Pointer to class instance
     */
    template <typename T>
    void registerStateCallback(const std::string& state_name, const std::string& callback_type,
                               void (T::*callback)(), T* instance);

    /**
     * @brief Register transition callback
     * @tparam T Class object type
     * @param from_state Source state
     * @param to_state Target state
     * @param callback Callback method
     * @param instance Pointer to class instance
     */
    template <typename T>
    void registerTransitionCallback(const std::string& from_state, const std::string& to_state,
                                    void (T::*callback)(const TransitionEvent&), T* instance);

    /**
     * @brief Register guard callback
     * @tparam T Class object type
     * @param from_state Source state
     * @param to_state Target state
     * @param event_name Event name
     * @param callback Callback method
     * @param instance Pointer to class instance
     */
    template <typename T>
    void registerGuard(const std::string& from_state, const std::string& to_state,
                       const std::string& event_name, bool (T::*callback)(), T* instance);

    /**
     * @brief Register action callback
     * @tparam T Class object type
     * @param action_name Action name
     * @param callback Callback method
     * @param instance Pointer to class instance
     */
    template <typename T>
    void registerAction(const std::string& action_name, void (T::*callback)(), T* instance);

    // Variable management

    /**
     * @brief Set variable value
     * @param name Variable name
     * @param value Variable value
     */
    void setVariable(const std::string& name, const VariableValue& value);

    /**
     * @brief Get variable value
     * @param name Variable name
     * @return Variable value
     * @throws StateException if variable does not exist
     */
    VariableValue getVariable(const std::string& name) const;

    /**
     * @brief Check if variable exists
     * @param name Variable name
     * @return true if variable exists
     */
    bool hasVariable(const std::string& name) const;

    // Observers

    /**
     * @brief Register state change observer
     * @param observer Pointer to observer
     */
    void registerStateObserver(StateObserver* observer);

    /**
     * @brief Unregister observer
     * @param observer Pointer to observer
     */
    void unregisterStateObserver(StateObserver* observer);

    // Error handling

    /**
     * @brief Set error handler
     * @param handler Error handler function
     */
    void setErrorHandler(ErrorHandler handler);

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    // Helper methods
    void performTransition(const TransitionEvent& event);
    bool evaluateGuard(const std::string& from_state, const std::string& to_state,
                       const std::string& event_name);
    void executeStateActions(const std::string& state_name);
    void executeTransitionActions(const std::vector<std::string>& actions);

    // Helper methods for callback registration (for template methods)
    void registerStateCallbackImpl(const std::string& state_name, const std::string& callback_type,
                                   std::function<void()> callback);

    void registerTransitionCallbackImpl(const std::string& from_state, const std::string& to_state,
                                        std::function<void(const TransitionEvent&)> callback);

    void registerGuardImpl(const std::string& from_state, const std::string& to_state,
                           const std::string& event_name, std::function<bool()> callback);

    void registerActionImpl(const std::string& action_name, std::function<void()> callback);
};

// Template method implementations in header

template <typename T>
void StateMachine::registerStateCallback(const std::string& state_name,
                                         const std::string& callback_type, void (T::*callback)(),
                                         T* instance) {
    auto cb = [instance, callback]() { (instance->*callback)(); };
    registerStateCallbackImpl(state_name, callback_type, cb);
}

template <typename T>
void StateMachine::registerTransitionCallback(const std::string& from_state,
                                              const std::string& to_state,
                                              void (T::*callback)(const TransitionEvent&),
                                              T* instance) {
    auto cb = [instance, callback](const TransitionEvent& event) { (instance->*callback)(event); };
    registerTransitionCallbackImpl(from_state, to_state, cb);
}

template <typename T>
void StateMachine::registerGuard(const std::string& from_state, const std::string& to_state,
                                 const std::string& event_name, bool (T::*callback)(),
                                 T* instance) {
    auto cb = [instance, callback]() -> bool { return (instance->*callback)(); };
    registerGuardImpl(from_state, to_state, event_name, cb);
}

template <typename T>
void StateMachine::registerAction(const std::string& action_name, void (T::*callback)(),
                                  T* instance) {
    auto cb = [instance, callback]() { (instance->*callback)(); };
    registerActionImpl(action_name, cb);
}

}  // namespace fsmconfig
