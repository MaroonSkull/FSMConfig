#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace fsmconfig {

/**
 * @file types.hpp
 * @brief Main data types for FSMConfig
 */

/**
 * @brief Enumeration of variable types
 */
enum class VariableType {
  INT,     ///< Integer value
  FLOAT,   ///< Floating point value
  STRING,  ///< String value
  BOOL     ///< Boolean value
};

/**
 * @brief Structure for storing variable value with support for different types
 *
 * Uses union for efficient storage of values of different types.
 * For std::string uses placement new and explicit destructor call.
 */
struct VariableValue {
  VariableType type;
  union {
    int int_value{};
    float float_value;
    std::string string_value;
    bool bool_value;
  };

  /**
   * @brief Default constructor (initializes as INT with value 0)
   */
  VariableValue();

  /**
   * @brief Constructor for integer value
   * @param v Integer value
   */
  VariableValue(int v);

  /**
   * @brief Constructor for floating point value
   * @param v Floating point value
   */
  VariableValue(float v);

  /**
   * @brief Constructor for string value
   * @param v String value
   */
  VariableValue(const std::string& v);

  /**
   * @brief Constructor for boolean value
   * @param v Boolean value
   */
  VariableValue(bool v);

  /**
   * @brief Destructor
   *
   * Explicitly calls destructor for std::string if type is STRING
   */
  ~VariableValue();

  /**
   * @brief Copy constructor
   * @param other Value to copy
   */
  VariableValue(const VariableValue& other);

  /**
   * @brief Copy assignment operator
   * @param other Value to assign
   * @return Reference to current object
   */
  VariableValue& operator=(const VariableValue& other);

  /**
   * @brief Get value as integer
   * @return Integer value
   * @throw std::bad_cast If type is not INT
   */
  [[nodiscard]] int asInt() const;

  /**
   * @brief Get value as floating point number
   * @return Floating point value
   * @throw std::bad_cast If type is not FLOAT
   */
  [[nodiscard]] float asFloat() const;

  /**
   * @brief Get value as string
   * @return String value
   * @throw std::bad_cast If type is not STRING
   */
  [[nodiscard]] std::string asString() const;

  /**
   * @brief Get value as boolean
   * @return Boolean value
   * @throw std::bad_cast If type is not BOOL
   */
  [[nodiscard]] bool asBool() const;

  /**
   * @brief Convert value to string representation
   * @return String representation of value
   */
  [[nodiscard]] std::string toString() const;
};

/**
 * @brief Transition event between states
 *
 * Stores information about transition, including event name,
 * source and target state, event data, and timestamp.
 */
struct TransitionEvent {
  std::string event_name;                           ///< Event name
  std::string from_state;                           ///< Source state
  std::string to_state;                             ///< Target state
  std::map<std::string, VariableValue> data;        ///< Event data
  std::chrono::system_clock::time_point timestamp;  ///< Timestamp

  /**
   * @brief Default constructor
   *
   * Initializes timestamp with current time
   */
  TransitionEvent();
};

/**
 * @brief State information
 *
 * Contains state name, state variables,
 * callbacks and actions associated with the state.
 */
struct StateInfo {
  std::string name;                                ///< State name
  std::map<std::string, VariableValue> variables;  ///< State variables
  std::string on_enter_callback;                   ///< On-enter callback
  std::string on_exit_callback;                    ///< On-exit callback
  std::vector<std::string> actions;                ///< List of actions

  /**
   * @brief Default constructor
   */
  StateInfo();

  /**
   * @brief Constructor with state name
   * @param name State name
   */
  explicit StateInfo(std::string name);
};

/**
 * @brief Transition information
 *
 * Contains information about transition between states,
 * including conditions, callbacks and actions.
 */
struct TransitionInfo {
  std::string from_state;            ///< Source state
  std::string to_state;              ///< Target state
  std::string event_name;            ///< Event name
  std::string guard_callback;        ///< Guard callback
  std::string transition_callback;   ///< Transition callback
  std::vector<std::string> actions;  ///< List of actions

  /**
   * @brief Default constructor
   */
  TransitionInfo();
};

/**
 * @brief Interface for observing state changes
 *
 * Allows subscribing to state change events
 * and receiving notifications about transitions.
 */
class StateObserver {
 public:
  /**
   * @brief Virtual destructor
   */
  virtual ~StateObserver() = default;

  /**
   * @brief Called when entering a state
   * @param state_name State name
   */
  virtual void onStateEnter(const std::string& state_name) = 0;

  /**
   * @brief Called when exiting a state
   * @param state_name State name
   */
  virtual void onStateExit(const std::string& state_name) = 0;

  /**
   * @brief Called during transition between states
   * @param event Transition event
   */
  virtual void onTransition(const TransitionEvent& event) = 0;

  /**
   * @brief Called when an error occurs
   * @param error_message Error message
   */
  virtual void onError(const std::string& error_message) = 0;
};

/**
 * @brief Error handler function type
 */
using ErrorHandler = std::function<void(const std::string&)>;

/**
 * @brief Exception for configuration errors
 *
 * Thrown on configuration parsing errors
 * or when configuration structure is invalid.
 */
class ConfigException : public std::runtime_error {
 public:
  /**
   * @brief Constructor with error message
   * @param message Error message
   */
  explicit ConfigException(const std::string& message);
};

/**
 * @brief Exception for state errors
 *
 * Thrown on errors related to states
 * or transitions between them.
 */
class StateException : public std::runtime_error {
 public:
  /**
   * @brief Constructor with error message
   * @param message Error message
   */
  explicit StateException(const std::string& message);
};

}  // namespace fsmconfig
