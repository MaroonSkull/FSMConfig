# FSMConfig

![CI](https://github.com/MaroonSkull/FSMConfig/workflows/CI/badge.svg)
![Linters](https://github.com/MaroonSkull/FSMConfig/actions/workflows/linters.yml/badge.svg)
![Docker](https://img.shields.io/badge/docker-supported-blue.svg)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/MaroonSkull/FSMConfig)

A C++ library for YAML-configured finite state machines. Because your spaghetti code deserves better.

## Features

- **YAML-based configuration**: Define state machines using human-readable YAML files
- **Event-driven architecture**: React to events with flexible callback system
- **Type-safe**: Modern C++20 with strong type checking
- **Extensible**: Easy to extend with custom states and transitions
- **Well-tested**: Comprehensive test suite using Google Test
- **Cross-platform**: Works on Linux, macOS, and Windows

## Docker Support

FSMConfig provides full Docker integration for consistent development and CI/CD environments.

### Quick Start with Docker

**DevContainer (Recommended for Development):**
```bash
# Open project in VSCode DevContainer
F1 → Dev Containers: Reopen in Container
```

**Docker for CI/Testing:**
```bash
# Build Docker image
docker build -t fsmconfig:ci .

# Run tests in Docker
docker run --rm fsmconfig:ci bash -c "
  mkdir build && cd build && \
  cmake -DCMAKE_BUILD_TYPE=Release .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

### Two Docker Approaches

| Approach | Use Case | Dockerfile |
|----------|----------|------------|
| **DevContainer** | Interactive development with VSCode | [`.devcontainer/Dockerfile`](.devcontainer/Dockerfile) |
| **CI/CD** | Automated testing and builds | [`Dockerfile`](Dockerfile) |

For complete Docker documentation, see [`DOCKER.md`](DOCKER.md).

## Building

### Prerequisites

- CMake 3.15 or higher
- C++20 compatible compiler (GCC 11+, Clang 12+, MSVC 19.28+)
- yaml-cpp library
- Google Test (for building tests)

### Local Build

```bash
# Configure project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build project
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Docker Build

```bash
# Build Docker image
docker build -t fsmconfig:ci .

# Build project in Docker
docker run --rm -v $(pwd):/workspace -w /workspace fsmconfig:ci bash -c "
  mkdir -p build && cd build && \
  cmake .. && \
  cmake --build . -j\$(nproc)
"

# Run tests in Docker
docker run --rm -v $(pwd):/workspace -w /workspace/build fsmconfig:ci ctest --output-on-failure
```

### Build Options

```bash
# Debug build (with debug symbols)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Disable tests
cmake .. -DBUILD_TESTS=OFF

# Disable examples
cmake .. -DBUILD_EXAMPLES=OFF
```

## Development

### Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: GCC 14+, Clang 17+, or MSVC 19.35+
- **Build System**: CMake 3.15+
- **Libraries**: yaml-cpp, Google Test

### Recommended Tools

- **IDE**: Visual Studio Code with C/C++ and CMake Tools extensions
- **Debugger**: GDB (Linux/macOS) or Visual Studio Debugger (Windows)
- **Code Analysis**: clang-tidy, clang-format
- **Version Control**: Git

### DevContainer Setup

The easiest way to set up a development environment is using VSCode DevContainer:

1. Install [Docker Desktop](https://www.docker.com/products/docker-desktop)
2. Install [Visual Studio Code](https://code.visualstudio.com/)
3. Install [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
4. Open the project in VSCode
5. Press `F1` and select `Dev Containers: Reopen in Container`

The DevContainer automatically includes:
- GCC 14 and CMake
- yaml-cpp and Google Test
- VSCode extensions (C/C++, CMake Tools, clang-format)
- Development tools (gdb, valgrind, clang-tidy)

### Development Workflow

```bash
# Configure (Debug mode)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure

# Run specific test
./tests/test_state_machine

# Format code
clang-format -i src/**/*.cpp include/**/*.hpp
```

## Usage

### Basic Example

Create a YAML configuration file (`config.yaml`):

```yaml
states:
  - name: idle
    initial: true
  - name: active
  - name: paused

transitions:
  - from: idle
    to: active
    event: start
  - from: active
    to: paused
    event: pause
  - from: paused
    to: active
    event: resume
  - from: paused
    to: idle
    event: stop
```

Use the library in your C++ code:

```cpp
#include <fsmconfig/config_parser.hpp>
#include <fsmconfig/state_machine.hpp>

int main() {
    // Load configuration
    auto config = fsmconfig::ConfigParser::parse("config.yaml");
    
    // Create state machine
    auto fsm = fsmconfig::StateMachine::create(config);
    
    // Trigger events
    fsm->dispatch("start");  // idle -> active
    fsm->dispatch("pause");  // active -> paused
    fsm->dispatch("resume"); // paused -> active
    fsm->dispatch("stop");   // active -> idle
    
    return 0;
}
```

For more examples, see the [`examples/`](examples/) directory.

## Documentation

- [API Reference](docs/api_reference.md)
- [Architecture](docs/architecture.md)
- [Examples](docs/examples.md)
- [Docker Integration](DOCKER.md)

## Testing

The project uses Google Test for unit testing.

```bash
# Run all tests
cd build && ctest --output-on-failure

# Run specific test
ctest -R test_name --output-on-failure

# Run tests with verbose output
ctest --verbose

# Run test directly
./build/tests/test_state_machine
```

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass (`ctest --output-on-failure`)
6. Format your code (`clang-format -i src/**/*.cpp include/**/*.hpp`)
7. Commit your changes (`git commit -m 'Add amazing feature'`)
8. Push to the branch (`git push origin feature/amazing-feature`)
9. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [yaml-cpp](https://github.com/jbeder/yaml-cpp) for YAML parsing
- [Google Test](https://github.com/google/googletest) for testing framework
- [CMake](https://cmake.org/) for build system

---

**Version**: 0.1.0 | **C++ Standard**: C++20 | **CMake**: 3.15+
