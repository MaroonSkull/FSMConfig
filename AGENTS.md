# AGENTS.md

## Project Overview

### Purpose

**FSMConfig** is a C++ library for working with finite state machines through YAML configuration. The library allows declarative description of states, transitions, events, and callbacks in YAML files, simplifying the creation and modification of complex state machines without the need for code recompilation.

### Main Components

#### Header Files (`include/fsmconfig/`)

| File | Purpose |
|------|---------|
| [`state_machine.hpp`](include/fsmconfig/state_machine.hpp) | Main finite state machine class |
| [`state.hpp`](include/fsmconfig/state.hpp) | State representation class |
| [`config_parser.hpp`](include/fsmconfig/config_parser.hpp) | YAML configuration parser |
| [`event_dispatcher.hpp`](include/fsmconfig/event_dispatcher.hpp) | Event dispatcher |
| [`callback_registry.hpp`](include/fsmconfig/callback_registry.hpp) | Callback registry |
| [`variable_manager.hpp`](include/fsmconfig/variable_manager.hpp) | Variable manager |
| [`types.hpp`](include/fsmconfig/types.hpp) | Main types and definitions |

#### Source Files (`src/fsmconfig/`)

| File | Implementation |
|------|----------------|
| [`state_machine.cpp`](src/fsmconfig/state_machine.cpp) | Finite state machine logic |
| [`state.cpp`](src/fsmconfig/state.cpp) | State management |
| [`config_parser.cpp`](src/fsmconfig/config_parser.cpp) | YAML parsing |
| [`event_dispatcher.cpp`](src/fsmconfig/event_dispatcher.cpp) | Event handling |
| [`callback_registry.cpp`](src/fsmconfig/callback_registry.cpp) | Callback registration and invocation |
| [`variable_manager.cpp`](src/fsmconfig/variable_manager.cpp) | Variable management |
| [`types.cpp`](src/fsmconfig/types.cpp) | Auxiliary types |

#### Tests (`tests/`)

- [`test_state_machine.cpp`](tests/test_state_machine.cpp) — finite state machine tests
- [`test_config_parser.cpp`](tests/test_config_parser.cpp) — configuration parser tests
- [`test_callback_registry.cpp`](tests/test_callback_registry.cpp) — callback registry tests
- [`test_integration.cpp`](tests/test_integration.cpp) — integration tests

#### Examples (`examples/`)

- [`simple_fsm/`](examples/simple_fsm/) — simple finite state machine example
- [`game_state/`](examples/game_state/) — game state management example
- [`network_protocol/`](examples/network_protocol/) — network protocol example

#### Additional Configuration Files

| File/Directory | Purpose |
|----------------|---------|
| [`cmake/Findyaml-cpp.cmake`](cmake/Findyaml-cpp.cmake) | Custom yaml-cpp find module |
| [`cmake/fsmconfig-config.cmake.in`](cmake/fsmconfig-config.cmake.in) | CMake configuration template |
| [`Dockerfile`](Dockerfile) | Docker image for development |
| [`.devcontainer/`](.devcontainer/) | VS Code Dev Container configuration |
| [`.github/`](.github/) | GitHub Actions configuration |
| [`.gitignore`](.gitignore) | Git exclusion rules |
| [`LICENSE`](LICENSE) | Project license |

---

## Technology Stack

### C++ Standard and Features

- **Standard:** C++20
- **Compiler:** GCC/Clang with C++20 support
- **Features:**
  - RAII for resource management
  - Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
  - STL containers and algorithms
  - Lambda expressions for callbacks
  - `std::function` and `std::bind` for callbacks

### CMake Configuration

- **Minimum version:** CMake 3.15+
- **Build system:** Modern CMake using target-based approach
- **Main targets:**
  - `fsmconfig` — main library
  - `fsmconfig_tests` — tests
  - `simple_fsm`, `game_state`, `network_protocol` — usage examples

**Build example:**

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### yaml-cpp

- **Version:** 0.7.0 or higher
- **Purpose:** Parsing YAML configurations
- **Integration:** Via CMake with three fallback methods:
  1. `find_package(yaml-cpp CONFIG)` for system packages
  2. `pkg_check_modules(yaml-cpp)` via PkgConfig (for Arch Linux)
  3. Custom module [`cmake/Findyaml-cpp.cmake`](cmake/Findyaml-cpp.cmake) as last resort

**Configuration example:**

```yaml
# Global variables
variables:
  max_attempts: 3

# State definitions
states:
  idle:
    on_enter: on_idle_enter
    on_exit: on_idle_exit
    actions:
      - log_idle_state
  
  active:
    on_enter: on_active_enter
    on_exit: on_active_exit
    actions:
      - log_active_state

# Transition definitions
transitions:
  - from: idle
    to: active
    event: start
    on_transition: on_start_transition
  
  - from: active
    to: idle
    event: stop
    on_transition: on_stop_transition
```

### Testing Framework

- **Framework:** Google Test (gtest)
- **Running tests:**
  ```bash
  cd build
  ctest --output-on-failure
  ```
- **Coverage:** Unit tests for each component + integration tests

### Linters and Code Quality Tools

| Tool | Purpose |
|------|---------|
| **clangd** | Language Server Protocol for C++ |
| **clang-tidy** | Static code analysis |
| **clang-format** | Code formatting |
| **cppcheck** | Additional static analysis |
| **include-what-you-use** | Header inclusion analysis |

**Recommended clang-tidy checks:**

```bash
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++20
```

### Linter Configuration Files

The project has linter configuration files in the root directory:

#### .clang-tidy
```yaml
Checks: >
  -*,
  bugprone-*,
  -bugprone-easily-swappable-parameters,
  cert-*,
  clang-analyzer-*,
  cppcoreguidelines-*,
  -cppcoreguidelines-avoid-magic-numbers,
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,
  -cppcoreguidelines-pro-type-reinterpret-cast,
  modernize-*,
  -modernize-use-trailing-return-type,
  performance-*,
  readability-*,
  -readability-magic-numbers,
  -readability-identifier-length
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
FormatStyle: file
```

#### .clang-format
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
```

#### .cppcheck
```ini
--enable=all
--inconclusive
--std=c++20
-Iinclude/
```

### CI/CD for Linters

The project has automatic linter execution via GitHub Actions:
- File: [`.github/workflows/linters.yml`](.github/workflows/linters.yml)
- Triggers: push and pull request to `main` and `develop` branches
- Checks performed:
  - `clang-tidy-check` — static code analysis
  - `clang-format-check` — formatting verification
  - `cppcheck-check` — additional static analysis

---

## Additional Resources

### Internal Documentation

- [`README.md`](README.md) — general project description
- [`DOCKER.md`](DOCKER.md) — Docker instructions
- [`docs/architecture.md`](docs/architecture.md) — project architecture
- [`docs/api_reference.md`](docs/api_reference.md) — API reference
- [`docs/examples.md`](docs/examples.md) — usage examples

### External Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [yaml-cpp Documentation](https://github.com/jbeder/yaml-cpp/)
- [Google Test Documentation](https://google.github.io/googletest/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)

---

## Quick Reference

### Quick Start

```bash
# Build project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run tests
ctest --output-on-failure

# Run linters
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++20
clang-format -i src/**/*.cpp include/**/*.hpp
```

### Typical Workflow

```bash
# 1. Create branch
git checkout -b feature/my-feature

# 2. Make changes
# ... code ...

# 3. Check with linters
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++20

# 4. Run tests
cd build && ctest --output-on-failure

# 5. Commit
git add .
git commit -m "feat: add my feature"

# 6. Repeat for subtasks
# ...

# 7. Review
git diff main..feature/my-feature

# 8. Merge sub-branches
git merge feature/sub-task-1
```
