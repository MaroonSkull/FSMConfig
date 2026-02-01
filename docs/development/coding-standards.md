# Coding Standards

## General Rules

- **Documentation language:** English

**Note:** Technical terms, command names, libraries, and standards should remain in English (e.g., RAII, smart pointers, `std::unique_ptr`, C++20). Code and commands are always in English.

- **Code mentions:** Wrap in backticks (\`)
- **Doxygen format:** Java style (`/** @... */`)

## Single-Line Comments

```cpp
/// Registers a new callback for the specified event
void registerCallback(const std::string& event, CallbackFunc callback);
```

**Important:** The `///` comment must ALWAYS be BEFORE the element being commented.

## Multi-Line Comments

```cpp
/**
 * @brief Parses YAML configuration and creates a finite state machine
 * 
 * @param config_path Path to the YAML configuration file
 * @return std::unique_ptr<StateMachine> Pointer to the created state machine
 * @throws std::runtime_error If the configuration is invalid
 * 
 * @example
 * auto fsm = parser.parseConfig("config.yaml");
 * fsm->start();
 */
std::unique_ptr<StateMachine> parseConfig(const std::string& config_path);
```

## Comment Content

**Describe business logic, not algorithms:**

❌ **Bad:**
```cpp
/// Iterate through all array elements and check the condition
void processEvents();
```

✅ **Good:**
```cpp
/// Processes all events from the queue in the order they were received
/// and executes the corresponding state transitions
void processEvents();
```

## Documentation Requirements

Document ALL functions that are touched during work:

- Public class methods
- Protected methods (if they are part of the API)
- Private methods (if they contain non-trivial logic)
- Free functions
- Structures and classes
- Enumerations
