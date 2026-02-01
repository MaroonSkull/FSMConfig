# Docker and DevContainer for FSMConfig

## Introduction

The FSMConfig project integrates with Docker to provide an isolated and reproducible development and continuous integration environment. Docker allows:

- **Unified development environment**: All developers use the same versions of compilers, libraries, and tools
- **Build reproducibility**: CI/CD pipelines use the same images as local development
- **Quick start**: New developers can start working without installing dependencies
- **Isolation**: Project dependencies don't affect the host system

The project uses two different approaches:
- **DevContainer** - for interactive development in VSCode
- **Docker for CI** - optimized image for automated building and testing

---

## DevContainer (Development)

### What is DevContainer

DevContainer (Development Container) is a VSCode feature that allows you to develop code inside a Docker container. This provides:

- Fully configured development environment "out of the box"
- Automatic installation of necessary VSCode extensions
- Isolation of project dependencies from the host system
- Unified settings for the entire development team

### How to open project in DevContainer (VSCode)

**Requirements:**
- Installed [Docker Desktop](https://www.docker.com/products/docker-desktop) or Docker Engine
- Installed [Visual Studio Code](https://code.visualstudio.com/)
- Installed [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

**Steps:**

1. Open the FSMConfig project in VSCode
2. Press `F1` (or `Ctrl+Shift+P`) to open the command palette
3. Select `Dev Containers: Reopen in Container`
4. Wait for the image to build and the container to start (first run will take a few minutes)

On first run, VSCode will automatically:
- Build a Docker image based on [`.devcontainer/Dockerfile`](.devcontainer/Dockerfile)
- Install specified extensions
- Execute the command from `postCreateCommand`

### Which extensions are automatically installed

The following extensions are automatically installed in DevContainer:

| Extension | Description |
|-----------|-------------|
| `ms-vscode.cpptools` | Official C/C++ tools package for VSCode (IntelliSense, debugging) |
| `ms-vscode.cmake-tools` | Tools for working with CMake (building, debugging, running tests) |
| `twxs.cmake` | CMake file syntax highlighting |
| `xaver.clang-format` | Code formatting with clang-format |
| `cheshirekow.cmake-format` | CMake file formatting |

### Which tools are available inside the container

DevContainer includes the following development tools:

**Compilers and build system:**
- GCC 14 (default compiler)
- G++ 14
- CMake (latest version from Ubuntu repositories)

**Libraries:**
- libyaml-cpp-dev (for working with YAML configurations)
- libgtest-dev (Google Test for writing tests)

**Development utilities:**
- pkg-config (for finding libraries)
- git (version control system)
- vim (text editor)
- curl, wget (file download utilities)

**Debugging and analysis tools:**
- gdb (debugger)
- valgrind (memory analyzer)
- clang-format (code formatting)
- clang-tidy (static analysis)

**Additionally:**
- Zsh with Oh My Zsh (enhanced shell)
- Common utilities from devcontainers/features/common-utils

### How to build project inside DevContainer

After opening the project in DevContainer, run the following commands:

**1. Project configuration (executed automatically on first run):**
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**2. Build project:**
```bash
cd build
cmake --build . -j$(nproc)
```

Or use the CMake Tools panel in VSCode:
- Open the CMake panel (Ctrl+Shift+P → CMake: Configure)
- Click "Build All"

**3. Build in Release mode:**
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### How to run tests inside DevContainer

**Run all tests:**
```bash
cd build
ctest --output-on-failure
```

**Run specific test:**
```bash
cd build
ctest -R test_name --output-on-failure
```

**Run tests with verbose output:**
```bash
cd build
ctest --verbose
```

**Run test directly:**
```bash
cd build/tests
./test_state_machine
```

---

## Docker for CI (Continuous Integration)

### Description of Dockerfile for CI

The [`Dockerfile`](Dockerfile) in the project root is optimized for CI/CD pipelines:

**Features:**
- Minimal image size (only necessary dependencies)
- Ubuntu 24.04 as base image
- GCC 14 as default compiler
- C++26 standard
- Release build mode by default
- No vscode user (run as root for CI)

**Installed packages:**
- build-essential (gcc, g++, make)
- cmake
- libyaml-cpp-dev
- libgtest-dev
- pkg-config
- git

### How to build Docker image for CI

**Build image locally:**
```bash
docker build -t fsmconfig:ci .
```

**Build using cache:**
```bash
docker build --cache-from fsmconfig:ci -t fsmconfig:ci .
```

**Build without cache (full rebuild):**
```bash
docker build --no-cache -t fsmconfig:ci .
```

### How to run tests in Docker container

**Run tests in Debug mode:**
```bash
docker run --rm fsmconfig:ci bash -c "
  mkdir build && \
  cd build && \
  cmake -DCMAKE_BUILD_TYPE=Debug .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

**Run tests in Release mode:**
```bash
docker run --rm fsmconfig:ci bash -c "
  mkdir build && \
  cd build && \
  cmake -DCMAKE_BUILD_TYPE=Release .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

**Run with source code mount (for quick iteration):**
```bash
docker run --rm -v $(pwd):/app -w /app fsmconfig:ci bash -c "
  mkdir -p build && \
  cd build && \
  cmake .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

### Command examples

**Check compiler version:**
```bash
docker run --rm fsmconfig:ci bash -c "g++ --version"
```

**Check CMake version:**
```bash
docker run --rm fsmconfig:ci bash -c "cmake --version"
```

**Check system information:**
```bash
docker run --rm fsmconfig:ci bash -c "uname -a && cat /etc/os-release"
```

**Interactive container run:**
```bash
docker run --rm -it fsmconfig:ci bash
```

---

## GitHub Actions CI/CD

### Workflow description

The CI pipeline is described in [`.github/workflows/ci.yml`](.github/workflows/ci.yml) and performs the following tasks:

1. **Code checkout**: Gets source code from repository
2. **Docker Buildx setup**: Prepares extended Docker builder
3. **Docker image build**: Creates `fsmconfig:ci` image with caching
4. **Version checks**: Outputs compiler, CMake versions and system information
5. **Build and test**: Compiles project and runs tests
6. **Result upload**: Saves test results as artifacts

### Which checks are performed

**Build matrix:**
- Debug configuration
- Release configuration

**Checks:**
- Project build with CMake
- Running all tests via CTest
- Compilation error check
- Linking error check
- Test passing check

### How to view CI results

1. Open the repository on GitHub
2. Go to the **Actions** tab
3. Select the desired workflow run
4. View details of each step
5. Download artifacts with test results (if available)

Artifacts are available for download for 90 days.

### Triggers for CI execution

CI automatically runs on the following events:

**Push events:**
- Push to `main` branch
- Push to `master` branch
- Push to `develop` branch

**Pull Request events:**
- Pull Request to `main` branch
- Pull Request to `master` branch
- Pull Request to `develop` branch

---

## Local Docker usage

### How to build Docker image locally

**Build image for development (DevContainer):**
```bash
cd .devcontainer
docker build -t fsmconfig:dev .
```

**Build image for CI:**
```bash
docker build -t fsmconfig:ci .
```

**Build with version tag:**
```bash
docker build -t fsmconfig:1.0.0 .
```

### How to run container for development

**Run with source code mount:**
```bash
docker run --rm -it \
  -v $(pwd):/workspace \
  -w /workspace \
  fsmconfig:dev \
  bash
```

**Run with debugger access:**
```bash
docker run --rm -it \
  -v $(pwd):/workspace \
  -w /workspace \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  fsmconfig:dev \
  bash
```

**Run with saving changes in image:**
```bash
docker run -it \
  -v $(pwd):/workspace \
  -w /workspace \
  --name fsmconfig-dev \
  fsmconfig:dev \
  bash
```

### How to mount source code

**Mount current directory:**
```bash
docker run --rm -v $(pwd):/workspace -w /workspace fsmconfig:dev bash
```

**Mount with user access rights:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace \
  -u $(id -u):$(id -g) \
  fsmconfig:dev \
  bash
```

**Mount multiple directories:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -v $(pwd)/build:/workspace/build \
  -w /workspace \
  fsmconfig:dev \
  bash
```

### Command examples

**Build project in container:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace \
  fsmconfig:ci \
  bash -c "mkdir -p build && cd build && cmake .. && cmake --build . -j\$(nproc)"
```

**Run tests in container:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace/build \
  fsmconfig:ci \
  ctest --output-on-failure
```

**Run example:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace/build/examples/simple_fsm \
  fsmconfig:ci \
  ./simple_fsm
```

---

## Troubleshooting

### Common problems and their solutions

**Problem: Container fails to build due to lack of permissions**

**Solution:**
```bash
sudo usermod -aG docker $USER
newgrp docker
```

**Problem: "Permission denied" error when working with files in mounted directory**

**Solution 1:** Run container with current user's UID:
```bash
docker run --rm -v $(pwd):/workspace -u $(id -u):$(id -g) fsmconfig:dev bash
```

**Solution 2:** Change file access rights:
```bash
sudo chown -R $(id -u):$(id -g) .
```

**Problem: DevContainer doesn't start in VSCode**

**Solutions:**
1. Make sure Docker Desktop is running
2. Check for "Dev Containers" extension
3. Open DevContainer logs (F1 → Dev Containers: Show Container Log)
4. Try rebuilding container (F1 → Dev Containers: Rebuild Container)

**Problem: Tests fail inside container but work locally**

**Solution:**
1. Check that all dependencies are installed in the image
2. Make sure compiler versions match
3. Check environment variables (CXXFLAGS, CMAKE_BUILD_TYPE)
4. Run tests with verbose output: `ctest --verbose`

**Problem: Memory error when running Valgrind**

**Solution:**
```bash
docker run --rm -v $(pwd):/workspace -w /workspace \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  fsmconfig:dev \
  valgrind --leak-check=full ./build/tests/test_state_machine
```

### How to rebuild Docker image

**Full rebuild without cache:**
```bash
docker build --no-cache -t fsmconfig:ci .
```

**Rebuild with old layer cleanup:**
```bash
docker build --no-cache --pull -t fsmconfig:ci .
```

**Rebuild DevContainer:**
```bash
cd .devcontainer
docker build --no-cache -t fsmconfig:dev .
```

### How to clear Docker cache

**Remove all stopped containers:**
```bash
docker container prune
```

**Remove unused images:**
```bash
docker image prune
```

**Remove all unused resources:**
```bash
docker system prune -a
```

**Remove specific image:**
```bash
docker rmi fsmconfig:ci
```

**Force remove image:**
```bash
docker rmi -f fsmconfig:ci
```

**Clear BuildKit cache:**
```bash
docker builder prune -a
```

---

## Docker integration architecture

### Differences between devcontainer and CI Dockerfile

| Characteristic | DevContainer Dockerfile | CI Dockerfile |
|----------------|------------------------|---------------|
| **Location** | [`.devcontainer/Dockerfile`](.devcontainer/Dockerfile) | [`Dockerfile`](Dockerfile) (project root) |
| **Purpose** | Interactive development | Automated CI/CD |
| **Image size** | Larger (~500-700 MB) | Minimal (~200-300 MB) |
| **User** | `vscode` (UID 1000) | root |
| **Tools** | Full set (gdb, valgrind, clang-format, vim, zsh) | Minimal set (only for building) |
| **Extensions** | Automatic installation in VSCode | Not applicable |
| **sudo rights** | Yes (passwordless) | Not required |
| **Build mode** | Debug by default | Release by default |
| **Shell** | Zsh with Oh My Zsh | Bash |
| **Mounting** | Automatic via VSCode | Manual via `-v` |

### Why two different Dockerfiles are used

**DevContainer Dockerfile** is optimized for:
- Development convenience with full tool set
- Interactive debugging and profiling
- Code formatting and static analysis
- Comfortable terminal work (Zsh, Oh My Zsh)
- VSCode integration (IntelliSense, CMake Tools)

**CI Dockerfile** is optimized for:
- Minimal image size (fast download in CI)
- Fast build (fewer layers, fewer dependencies)
- Build result reproducibility
- Minimal CI pipeline execution time
- GitHub Actions resource savings

Splitting into two Dockerfiles allows:
- Using different optimizations for different scenarios
- Not installing extra tools in CI
- Providing developers with maximum convenience
- Reducing CI pipeline execution time
- Simplifying image support and updates

---

## Links

### Official Docker documentation

- [Docker Documentation](https://docs.docker.com/)
- [Dockerfile Reference](https://docs.docker.com/engine/reference/builder/)
- [Docker Best Practices](https://docs.docker.com/develop/dev-best-practices/)
- [Docker BuildKit](https://docs.docker.com/build/buildkit/)

### VSCode DevContainers documentation

- [Visual Studio Code Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers)
- [Dev Containers Reference](https://code.visualstudio.com/docs/devcontainers/reference)
- [Creating a Dev Container](https://code.visualstudio.com/docs/devcontainers/create-dev-container)
- [Dev Container Features](https://code.visualstudio.com/docs/devcontainers/features)

### GitHub Actions documentation

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
- [Building and testing code](https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-code)
- [Docker Build Push Action](https://github.com/docker/build-push-action)

### Additional resources

- [CMake Documentation](https://cmake.org/documentation/)
- [Google Test Documentation](https://google.github.io/googletest/)
- [libyaml-cpp](https://github.com/jbeder/yaml-cpp)
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)

---

## Command quick reference

### DevContainer
```bash
# Open project in DevContainer
F1 → Dev Containers: Reopen in Container

# Rebuild container
F1 → Dev Containers: Rebuild Container

# Show container logs
F1 → Dev Containers: Show Container Log
```

### Docker
```bash
# Build image
docker build -t fsmconfig:ci .

# Run container
docker run --rm -v $(pwd):/workspace -w /workspace fsmconfig:ci bash

# View images
docker images

# Remove image
docker rmi fsmconfig:ci

# System cleanup
docker system prune -a
```

### CMake
```bash
# Configure project
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Build project
cmake --build build -j$(nproc)

# Run tests
cd build && ctest --output-on-failure
```

---

**Documentation updated:** 2025-01-20
**Version:** 1.0.0
