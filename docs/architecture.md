# FSMConfig Architecture

## Overview

FSMConfig is a C++26 library for declarative finite state machine definition using YAML configurations with C++ callback registration. The library separates state logic (YAML) from business logic (C++) while providing type-safe callback registration and high performance.

## Design Principles

1. **Separation of Concerns**: State definitions in YAML, business logic in C++
2. **Type Safety**: Template-based callback registration with compile-time type checking
3. **Performance**: Minimal overhead, no runtime scripting
4. **Simplicity**: Clean, intuitive API
5. **Extensibility**: Easy to extend with new features

## Core Components

### 1. ConfigParser

The `ConfigParser` class is responsible for loading and parsing YAML configuration files.

**Responsibilities:**
- Parse YAML configuration files using yaml-cpp
- Validate configuration structure
- Extract state definitions
- Extract transition definitions
- Extract variable definitions
- Handle errors gracefully with descriptive messages

**Key Classes:**
- `ConfigParser` - Main parser class
- `StateInfo` - Information about a state
- `TransitionInfo` - Information about a transition

### 2. CallbackRegistry

The `CallbackRegistry` class manages registration and invocation of callbacks.

**Responsibilities:**
- Store registered callbacks with type erasure
- Support member function callbacks
- Support free function callbacks
- Support lambda callbacks (via std::function)
- Thread-safe callback invocation
- Handle missing callbacks gracefully

**Key Classes:**
- `CallbackRegistry` - Main registry class
- `StateCallback` - Type for state callbacks
- `TransitionCallback` - Type for transition callbacks
- `GuardCallback` - Type for guard callbacks
- `ActionCallback` - Type for action callbacks

### 3. VariableManager

The `VariableManager` class manages global and state-local variables.

**Responsibilities:**
- Store global variables
- Store state-local variables
- Support variable type conversion
- Handle variable scope resolution
- Thread-safe variable access

**Key Classes:**
- `VariableManager` - Main manager class
- `VariableValue` - Value with support for multiple types
- `VariableType` - Enum for variable types

### 4. EventDispatcher

The `EventDispatcher` class manages event queue and processing.

**Responsibilities:**
- Queue events for processing
- Process events sequentially
- Support immediate event triggering
- Support synchronous event processing

**Key Classes:**
- `EventDispatcher` - Main dispatcher class
- `EventHandler` - Type for event handlers

### 5. StateMachine

The `StateMachine` class is the main entry point for using the library.

**Responsibilities:**
- Maintain current state
- Execute state transitions
- Invoke guards before transitions
- Invoke on_enter/on_exit callbacks
- Execute transition callbacks
- Manage variable scope (global vs state-local)
- Handle invalid transitions
- Support state observers

**Key Classes:**
- `StateMachine` - Main state machine class
- `State` - Represents a state in the machine
- `StateObserver` - Interface for observing state changes

## Data Flow

```
YAML Config → ConfigParser → StateInfo, TransitionInfo
                                           ↓
                                    StateMachine → CallbackRegistry
                                           ↓
                                    VariableManager
                                           ↓
                                    EventDispatcher
```

## Thread Safety

The following components are thread-safe:
- `CallbackRegistry` - All operations are protected by mutex
- `VariableManager` - All operations are protected by mutex
- `EventDispatcher` - All operations are protected by mutex

Note: `StateMachine` itself is not thread-safe. If you need thread-safe state machine operations, you should add external synchronization.

## Memory Management

All components use RAII and smart pointers for memory management:
- `std::unique_ptr` for exclusive ownership
- `std::shared_ptr` for shared ownership (if needed)
- No manual memory management
- No memory leaks (verified with valgrind/address sanitizer)

## Error Handling

The library uses a two-tier error handling strategy:
1. **Critical errors**: Throw exceptions (`ConfigException`, `StateException`)
2. **Non-critical errors**: Return error codes or optional values

All exceptions include descriptive error messages with context.

## Performance Considerations

1. **Minimal Overhead**: Callbacks are invoked directly without any scripting layer
2. **Efficient Lookups**: State and transition lookups use hash maps (O(1) average case)
3. **No Dynamic Allocation**: Callback storage uses type erasure with minimal overhead
4. **Cache-Friendly**: Data structures are optimized for cache locality

## Extension Points

The library is designed to be easily extended:

1. **Custom Variable Types**: Extend `VariableType` enum and `VariableValue` struct
2. **Custom Event Handlers**: Implement custom `EventHandler` functions
3. **Custom Observers**: Extend `StateObserver` interface
4. **Custom Parsers**: Extend `ConfigParser` for other configuration formats

## Dependencies

- **yaml-cpp**: YAML parsing library (required)
- **C++26**: Modern C++ features (required)
- **CMake 3.15+**: Build system (required)
- **GoogleTest**: Testing framework (optional, for tests)

## Build System

The library supports both static and shared library builds:
- Static library: `libfsmconfig.a`
- Shared library: `libfsmconfig.so` (Linux) / `fsmconfig.dll` (Windows)

CMake targets are exported for easy integration into other projects.
