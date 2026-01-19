# FSMConfig API Reference

## Table of Contents

- [Types](#types)
- [ConfigParser](#configparser)
- [CallbackRegistry](#callbackregistry)
- [VariableManager](#variablemanager)
- [EventDispatcher](#eventdispatcher)
- [StateMachine](#statemachine)
- [State](#state)
- [StateObserver](#stateobserver)

## Types

### VariableType

Enum defining the types of variables that can be stored.

```cpp
enum class VariableType {
    INT,      ///< Integer type
    FLOAT,    ///< Floating point type
    STRING,   ///< String type
    BOOL      ///< Boolean type
};
```

### VariableValue

Structure for storing values with support for multiple types.

```cpp
struct VariableValue {
    VariableType type;
    union {
        int int_value;
        float float_value;
        std::string string_value;
        bool bool_value;
    };
    
    VariableValue();
    VariableValue(int v);
    VariableValue(float v);
    VariableValue(const std::string& v);
    VariableValue(bool v);
    ~VariableValue();
    
    VariableValue(const VariableValue& other);
    VariableValue& operator=(const VariableValue& other);
    
    int asInt() const;
    float asFloat() const;
    std::string asString() const;
    bool asBool() const;
    std::string toString() const;
};
```

### TransitionEvent

Structure representing a transition between states.

```cpp
struct TransitionEvent {
    std::string event_name;
    std::string from_state;
    std::string to_state;
    std::map<std::string, VariableValue> data;
    std::chrono::system_clock::time_point timestamp;
    
    TransitionEvent();
};
```

### StateInfo

Structure containing information about a state.

```cpp
struct StateInfo {
    std::string name;
    std::map<std::string, VariableValue> variables;
    std::string on_enter_callback;
    std::string on_exit_callback;
    std::vector<std::string> actions;
    
    StateInfo();
    explicit StateInfo(const std::string& name);
};
```

### TransitionInfo

Structure containing information about a transition.

```cpp
struct TransitionInfo {
    std::string from_state;
    std::string to_state;
    std::string event_name;
    std::string guard_callback;
    std::string transition_callback;
    std::vector<std::string> actions;
    
    TransitionInfo();
};
```

## ConfigParser

### Constructor

```cpp
ConfigParser();
```

### Methods

#### loadFromFile

```cpp
void loadFromFile(const std::string& file_path);
```

Load configuration from a YAML file.

**Parameters:**
- `file_path` - Path to the YAML configuration file

**Throws:**
- `ConfigException` if the file cannot be loaded or parsed

#### loadFromString

```cpp
void loadFromString(const std::string& yaml_content);
```

Load configuration from a YAML string.

**Parameters:**
- `yaml_content` - YAML configuration as a string

**Throws:**
- `ConfigException` if the content cannot be parsed

#### getGlobalVariables

```cpp
const std::map<std::string, VariableValue>& getGlobalVariables() const;
```

Get all global variables.

**Returns:** Reference to the map of global variables

#### getStates

```cpp
const std::map<std::string, StateInfo>& getStates() const;
```

Get all state definitions.

**Returns:** Reference to the map of state definitions

#### getTransitions

```cpp
const std::vector<TransitionInfo>& getTransitions() const;
```

Get all transition definitions.

**Returns:** Reference to the vector of transition definitions

#### hasState

```cpp
bool hasState(const std::string& state_name) const;
```

Check if a state exists.

**Parameters:**
- `state_name` - Name of the state to check

**Returns:** `true` if the state exists, `false` otherwise

#### getState

```cpp
const StateInfo& getState(const std::string& state_name) const;
```

Get information about a specific state.

**Parameters:**
- `state_name` - Name of the state

**Returns:** Reference to the state information

**Throws:**
- `ConfigException` if the state does not exist

#### getTransitionsFrom

```cpp
std::vector<TransitionInfo> getTransitionsFrom(const std::string& state_name) const;
```

Get all transitions from a specific state.

**Parameters:**
- `state_name` - Name of the source state

**Returns:** Vector of transitions from the specified state

#### findTransition

```cpp
const TransitionInfo* findTransition(const std::string& from_state, const std::string& event_name) const;
```

Find a transition by event name.

**Parameters:**
- `from_state` - Name of the source state
- `event_name` - Name of the event

**Returns:** Pointer to the transition, or `nullptr` if not found

#### clear

```cpp
void clear();
```

Clear all loaded configuration.

## CallbackRegistry

### Constructor

```cpp
CallbackRegistry();
```

### Methods

#### registerStateCallback

```cpp
void registerStateCallback(
    const std::string& state_name,
    const std::string& callback_type,
    StateCallback callback
);
```

Register a state callback.

**Parameters:**
- `state_name` - Name of the state
- `callback_type` - Type of callback ("on_enter" or "on_exit")
- `callback` - Callback function

#### registerTransitionCallback

```cpp
void registerTransitionCallback(
    const std::string& from_state,
    const std::string& to_state,
    TransitionCallback callback
);
```

Register a transition callback.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `callback` - Callback function

#### registerGuard

```cpp
void registerGuard(
    const std::string& from_state,
    const std::string& to_state,
    const std::string& event_name,
    GuardCallback callback
);
```

Register a guard callback.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `event_name` - Name of the event
- `callback` - Guard callback function

#### registerAction

```cpp
void registerAction(
    const std::string& action_name,
    ActionCallback callback
);
```

Register an action callback.

**Parameters:**
- `action_name` - Name of the action
- `callback` - Action callback function

#### callStateCallback

```cpp
void callStateCallback(
    const std::string& state_name,
    const std::string& callback_type
) const;
```

Call a state callback.

**Parameters:**
- `state_name` - Name of the state
- `callback_type` - Type of callback ("on_enter" or "on_exit")

#### callTransitionCallback

```cpp
void callTransitionCallback(
    const std::string& from_state,
    const std::string& to_state,
    const TransitionEvent& event
) const;
```

Call a transition callback.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `event` - Transition event data

#### callGuard

```cpp
bool callGuard(
    const std::string& from_state,
    const std::string& to_state,
    const std::string& event_name
) const;
```

Call a guard callback.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `event_name` - Name of the event

**Returns:** `true` if guard allows transition, `false` otherwise (or if guard not found)

#### callAction

```cpp
void callAction(const std::string& action_name) const;
```

Call an action callback.

**Parameters:**
- `action_name` - Name of the action

#### hasStateCallback

```cpp
bool hasStateCallback(
    const std::string& state_name,
    const std::string& callback_type
) const;
```

Check if a state callback exists.

**Parameters:**
- `state_name` - Name of the state
- `callback_type` - Type of callback

**Returns:** `true` if callback exists, `false` otherwise

#### hasTransitionCallback

```cpp
bool hasTransitionCallback(
    const std::string& from_state,
    const std::string& to_state
) const;
```

Check if a transition callback exists.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state

**Returns:** `true` if callback exists, `false` otherwise

#### hasGuard

```cpp
bool hasGuard(
    const std::string& from_state,
    const std::string& to_state,
    const std::string& event_name
) const;
```

Check if a guard callback exists.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `event_name` - Name of the event

**Returns:** `true` if guard exists, `false` otherwise

#### hasAction

```cpp
bool hasAction(const std::string& action_name) const;
```

Check if an action callback exists.

**Parameters:**
- `action_name` - Name of the action

**Returns:** `true` if action exists, `false` otherwise

#### clear

```cpp
void clear();
```

Clear all registered callbacks.

## VariableManager

### Constructor

```cpp
VariableManager();
```

### Methods

#### setGlobalVariable

```cpp
void setGlobalVariable(const std::string& name, const VariableValue& value);
```

Set a global variable.

**Parameters:**
- `name` - Variable name
- `value` - Variable value

#### setStateVariable

```cpp
void setStateVariable(
    const std::string& state_name,
    const std::string& name,
    const VariableValue& value
);
```

Set a state-local variable.

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name
- `value` - Variable value

#### getVariable

```cpp
std::optional<VariableValue> getVariable(
    const std::string& state_name,
    const std::string& name
) const;
```

Get a variable (checks state-local first, then global).

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name

**Returns:** Optional variable value

#### getGlobalVariable

```cpp
std::optional<VariableValue> getGlobalVariable(const std::string& name) const;
```

Get a global variable.

**Parameters:**
- `name` - Variable name

**Returns:** Optional variable value

#### getStateVariable

```cpp
std::optional<VariableValue> getStateVariable(
    const std::string& state_name,
    const std::string& name
) const;
```

Get a state-local variable.

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name

**Returns:** Optional variable value

#### hasVariable

```cpp
bool hasVariable(
    const std::string& state_name,
    const std::string& name
) const;
```

Check if a variable exists (checks state-local first, then global).

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name

**Returns:** `true` if variable exists, `false` otherwise

#### hasGlobalVariable

```cpp
bool hasGlobalVariable(const std::string& name) const;
```

Check if a global variable exists.

**Parameters:**
- `name` - Variable name

**Returns:** `true` if variable exists, `false` otherwise

#### hasStateVariable

```cpp
bool hasStateVariable(
    const std::string& state_name,
    const std::string& name
) const;
```

Check if a state-local variable exists.

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name

**Returns:** `true` if variable exists, `false` otherwise

#### removeVariable

```cpp
bool removeVariable(
    const std::string& state_name,
    const std::string& name
);
```

Remove a variable (checks state-local first, then global).

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name

**Returns:** `true` if variable was removed, `false` if not found

#### removeGlobalVariable

```cpp
bool removeGlobalVariable(const std::string& name);
```

Remove a global variable.

**Parameters:**
- `name` - Variable name

**Returns:** `true` if variable was removed, `false` if not found

#### removeStateVariable

```cpp
bool removeStateVariable(
    const std::string& state_name,
    const std::string& name
);
```

Remove a state-local variable.

**Parameters:**
- `state_name` - Name of the state
- `name` - Variable name

**Returns:** `true` if variable was removed, `false` if not found

#### getGlobalVariables

```cpp
const std::map<std::string, VariableValue>& getGlobalVariables() const;
```

Get all global variables.

**Returns:** Reference to the map of global variables

#### getStateVariables

```cpp
const std::map<std::string, VariableValue>& getStateVariables(
    const std::string& state_name
) const;
```

Get all state-local variables for a state.

**Parameters:**
- `state_name` - Name of the state

**Returns:** Reference to the map of state-local variables

#### clear

```cpp
void clear();
```

Clear all variables.

#### clearStateVariables

```cpp
void clearStateVariables(const std::string& state_name);
```

Clear all state-local variables for a state.

**Parameters:**
- `state_name` - Name of the state

#### clearGlobalVariables

```cpp
void clearGlobalVariables();
```

Clear all global variables.

#### copyStateVariables

```cpp
void copyStateVariables(
    const std::string& from_state,
    const std::string& to_state
);
```

Copy variables from one state to another.

**Parameters:**
- `from_state` - Source state name
- `to_state` - Target state name

## EventDispatcher

### Constructor

```cpp
EventDispatcher();
```

### Methods

#### dispatchEvent

```cpp
void dispatchEvent(
    const std::string& event_name,
    const TransitionEvent& event
);
```

Dispatch an event for processing.

**Parameters:**
- `event_name` - Name of the event
- `event` - Transition event data

#### processEvents

```cpp
void processEvents();
```

Process all events in the queue (synchronously).

#### processOneEvent

```cpp
bool processOneEvent();
```

Process one event from the queue.

**Returns:** `true` if an event was processed, `false` if queue was empty

#### getEventQueueSize

```cpp
size_t getEventQueueSize() const;
```

Get the number of events in the queue.

**Returns:** Number of events in the queue

#### clearEventQueue

```cpp
void clearEventQueue();
```

Clear the event queue.

#### hasPendingEvents

```cpp
bool hasPendingEvents() const;
```

Check if there are pending events in the queue.

**Returns:** `true` if there are pending events, `false` otherwise

#### setEventHandler

```cpp
void setEventHandler(EventHandler handler);
```

Set the event handler function.

**Parameters:**
- `handler` - Event handler function

#### hasEventHandler

```cpp
bool hasEventHandler() const;
```

Check if an event handler is set.

**Returns:** `true` if handler is set, `false` otherwise

#### start

```cpp
void start();
```

Start the dispatcher (for future async support).

#### stop

```cpp
void stop();
```

Stop the dispatcher.

#### isRunning

```cpp
bool isRunning() const;
```

Check if the dispatcher is running.

**Returns:** `true` if running, `false` otherwise

#### waitForEmptyQueue

```cpp
void waitForEmptyQueue() const;
```

Wait for the event queue to become empty.

## StateMachine

### Constructors

```cpp
explicit StateMachine(const std::string& config_path);
explicit StateMachine(const std::string& yaml_content, bool is_content);
```

Create a state machine from a YAML configuration file or string.

**Parameters:**
- `config_path` - Path to the YAML configuration file
- `yaml_content` - YAML configuration as a string
- `is_content` - Set to `true` if `yaml_content` is YAML content

**Throws:**
- `ConfigException` if the configuration cannot be loaded or parsed

### Destructor

```cpp
~StateMachine();
```

### Methods

#### start

```cpp
void start();
```

Start the state machine (transition to initial state).

**Throws:**
- `StateException` if the machine is already started or if initial state not found

#### stop

```cpp
void stop();
```

Stop the state machine.

#### reset

```cpp
void reset();
```

Reset the state machine to initial state.

#### getCurrentState

```cpp
std::string getCurrentState() const;
```

Get the current state name.

**Returns:** Current state name

#### hasState

```cpp
bool hasState(const std::string& state_name) const;
```

Check if a state exists.

**Parameters:**
- `state_name` - Name of the state to check

**Returns:** `true` if the state exists, `false` otherwise

#### getAllStates

```cpp
std::vector<std::string> getAllStates() const;
```

Get all state names.

**Returns:** Vector of all state names

#### triggerEvent

```cpp
void triggerEvent(const std::string& event_name);
void triggerEvent(const std::string& event_name, const std::map<std::string, VariableValue>& data);
```

Trigger an event.

**Parameters:**
- `event_name` - Name of the event
- `data` - Optional event data

**Throws:**
- `StateException` if the machine is not started or if transition is not valid

#### registerStateCallback

```cpp
template<typename T>
void registerStateCallback(
    const std::string& state_name,
    const std::string& callback_type,
    void (T::*callback)(),
    T* instance
);
```

Register a state callback.

**Parameters:**
- `state_name` - Name of the state
- `callback_type` - Type of callback ("on_enter" or "on_exit")
- `callback` - Member function pointer
- `instance` - Pointer to the object instance

#### registerTransitionCallback

```cpp
template<typename T>
void registerTransitionCallback(
    const std::string& from_state,
    const std::string& to_state,
    void (T::*callback)(const TransitionEvent&),
    T* instance
);
```

Register a transition callback.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `callback` - Member function pointer
- `instance` - Pointer to the object instance

#### registerGuard

```cpp
template<typename T>
void registerGuard(
    const std::string& from_state,
    const std::string& to_state,
    const std::string& event_name,
    bool (T::*callback)(),
    T* instance
);
```

Register a guard callback.

**Parameters:**
- `from_state` - Name of the source state
- `to_state` - Name of the target state
- `event_name` - Name of the event
- `callback` - Member function pointer
- `instance` - Pointer to the object instance

#### registerAction

```cpp
template<typename T>
void registerAction(
    const std::string& action_name,
    void (T::*callback)(),
    T* instance
);
```

Register an action callback.

**Parameters:**
- `action_name` - Name of the action
- `callback` - Member function pointer
- `instance` - Pointer to the object instance

#### setVariable

```cpp
void setVariable(const std::string& name, const VariableValue& value);
```

Set a variable (global or state-local).

**Parameters:**
- `name` - Variable name
- `value` - Variable value

#### getVariable

```cpp
VariableValue getVariable(const std::string& name) const;
```

Get a variable (checks state-local first, then global).

**Parameters:**
- `name` - Variable name

**Returns:** Variable value

**Throws:**
- `StateException` if the variable is not found

#### hasVariable

```cpp
bool hasVariable(const std::string& name) const;
```

Check if a variable exists (checks state-local first, then global).

**Parameters:**
- `name` - Variable name

**Returns:** `true` if variable exists, `false` otherwise

#### registerStateObserver

```cpp
void registerStateObserver(StateObserver* observer);
```

Register a state observer.

**Parameters:**
- `observer` - Pointer to the observer

#### unregisterStateObserver

```cpp
void unregisterStateObserver(StateObserver* observer);
```

Unregister a state observer.

**Parameters:**
- `observer` - Pointer to the observer

#### setErrorHandler

```cpp
void setErrorHandler(ErrorHandler handler);
```

Set the error handler function.

**Parameters:**
- `handler` - Error handler function

## State

### Constructor

```cpp
explicit State(const StateInfo& info);
```

Create a state from state information.

**Parameters:**
- `info` - State information

### Methods

#### getName

```cpp
const std::string& getName() const;
```

Get the state name.

**Returns:** Reference to the state name

#### getVariables

```cpp
const std::map<std::string, VariableValue>& getVariables() const;
```

Get the state variables.

**Returns:** Reference to the state variables map

#### getOnEnterCallback

```cpp
const std::string& getOnEnterCallback() const;
```

Get the on_enter callback name.

**Returns:** Reference to the callback name

#### getOnExitCallback

```cpp
const std::string& getOnExitCallback() const;
```

Get the on_exit callback name.

**Returns:** Reference to the callback name

#### getActions

```cpp
const std::vector<std::string>& getActions() const;
```

Get the state actions.

**Returns:** Reference to the actions vector

#### hasVariable

```cpp
bool hasVariable(const std::string& name) const;
```

Check if a variable exists in the state.

**Parameters:**
- `name` - Variable name

**Returns:** `true` if variable exists, `false` otherwise

#### getVariable

```cpp
VariableValue getVariable(const std::string& name) const;
```

Get a variable from the state.

**Parameters:**
- `name` - Variable name

**Returns:** Variable value

**Throws:**
- `StateException` if the variable is not found

#### setVariable

```cpp
void setVariable(const std::string& name, const VariableValue& value);
```

Set a variable in the state.

**Parameters:**
- `name` - Variable name
- `value` - Variable value

#### getAllVariables

```cpp
const std::map<std::string, VariableValue>& getAllVariables() const;
```

Get all variables in the state.

**Returns:** Reference to the variables map

## StateObserver

### Virtual Methods

```cpp
virtual void onStateEnter(const std::string& state_name) = 0;
virtual void onStateExit(const std::string& state_name) = 0;
virtual void onTransition(const TransitionEvent& event) = 0;
virtual void onError(const std::string& error_message) = 0;
```

Observer methods that must be implemented by derived classes.

**Parameters:**
- `state_name` - Name of the state
- `event` - Transition event data
- `error_message` - Error message

## Type Definitions

### StateCallback

```cpp
using StateCallback = std::function<void()>;
```

Type for state callbacks (on_enter, on_exit).

### TransitionCallback

```cpp
using TransitionCallback = std::function<void(const TransitionEvent&)>;
```

Type for transition callbacks.

### GuardCallback

```cpp
using GuardCallback = std::function<bool()>;
```

Type for guard callbacks.

### ActionCallback

```cpp
using ActionCallback = std::function<void()>;
```

Type for action callbacks.

### EventHandler

```cpp
using EventHandler = std::function<void(const std::string& event_name, const TransitionEvent& event)>;
```

Type for event handlers.

### ErrorHandler

```cpp
using ErrorHandler = std::function<void(const std::string&)>;
```

Type for error handlers.

## Exceptions

### ConfigException

```cpp
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message);
};
```

Exception thrown for configuration errors.

### StateException

```cpp
class StateException : public std::runtime_error {
public:
    explicit StateException(const std::string& message);
};
```

Exception thrown for state machine errors.
