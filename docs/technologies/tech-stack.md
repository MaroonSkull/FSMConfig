# Tech Stack

## C++ Standard and Features

- **Standard:** C++23
- **Compiler:** GCC/Clang with C++23 support
- **Features:**
  - RAII for resource management
  - Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
  - STL containers and algorithms
  - Lambda expressions for callbacks
  - `std::function` and `std::bind` for callbacks

## CMake Configuration

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

## yaml-cpp

- **Version:** 0.7.0 or higher
- **Purpose:** Parsing YAML configurations
- **Integration:** Via CMake with three fallback methods:
  1. `find_package(yaml-cpp CONFIG)` for system packages
  2. `pkg_check_modules(yaml-cpp)` via PkgConfig (for Arch Linux)
  3. Custom module [`cmake/Findyaml-cpp.cmake`](../cmake/Findyaml-cpp.cmake) as last resort

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

## Testing Framework

- **Framework:** Google Test (gtest)
- **Running tests:**
  ```bash
  cd build
  ctest --output-on-failure
  ```
- **Coverage:** Unit tests for each component + integration tests

## Linters and Code Quality Tools

| Tool | Purpose |
|------|---------|
| **clangd** | Language Server Protocol for C++ |
| **clang-tidy** | Static code analysis |
| **clang-format** | Code formatting |
| **cppcheck** | Additional static analysis |
| **include-what-you-use** | Header inclusion analysis |

**Recommended clang-tidy checks:**

```bash
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++23
```

## Linter Configuration Files

The project has linter configuration files in the root directory:

### .clang-tidy
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

### .clang-format
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
```

### .cppcheck
```ini
--enable=all
--inconclusive
--std=c++23
-Iinclude/
```

## CI/CD for Linters

The project has automatic linter execution via GitHub Actions:
- File: [`.github/workflows/linters.yml`](../.github/workflows/linters.yml)
- Triggers: push and pull request to `main` and `develop` branches
- Checks performed:
  - `clang-tidy-check` — static code analysis
  - `clang-format-check` — formatting verification
  - `cppcheck-check` — additional static analysis
