# ADR-002: Versioning Strategy for FSMConfig

## Status
Proposed

## Context

FSMConfig is a C++20 library for working with finite state machines through YAML configuration. The project is currently in development phase (version 0.1.0) and preparing for its first release. A comprehensive versioning strategy is needed to:

1. **Provide clear release expectations** to users about API stability and compatibility
2. **Support both pre-release and stable releases** as the library matures
3. **Address C++ ABI compatibility concerns** specific to binary libraries
4. **Enable package manager distribution** (vcpkg, conan, etc.)
5. **Establish a sustainable release workflow** for long-term maintenance

### Current State

- Current version in [`CMakeLists.txt`](../../CMakeLists.txt:2): `0.1.0`
- No formal release process defined
- No version information exposed at compile time
- No ABI compatibility guarantees established

### Challenges

C++ libraries face unique versioning challenges:

- **ABI stability**: Changes to class layouts, virtual functions, or inline code can break binary compatibility
- **API vs ABI**: Source-compatible changes may not be binary-compatible
- **Multiple distribution formats**: Header-only, static library, shared library
- **Compiler/platform differences**: Different builds may have incompatible ABIs

## Decision

### Versioning Scheme: Semantic Versioning 2.0.0

FSMConfig will adopt **Semantic Versioning (SemVer 2.0.0)** with extensions for C++ library specifics.

**Format**: `MAJOR.MINOR.PATCH[-PRERELEASE][+BUILD]`

Examples:
- `1.0.0` - First stable release
- `1.0.0-alpha.1` - First alpha pre-release
- `1.0.0-beta.1` - First beta pre-release
- `1.0.0-rc.1` - First release candidate
- `1.1.0` - New feature release (backward compatible)
- `1.1.1` - Bug fix release
- `2.0.0` - Breaking changes release

### First Release Strategy

**Current**: `0.1.0` (development)

**First Pre-release**: `1.0.0-alpha.1`
- Indicates API is feature-complete but not yet stable
- Users can test and provide feedback
- Breaking changes may still occur

**Stable Release**: `1.0.0`
- API and ABI stability guarantees apply
- Suitable for production use

### Version Increment Rules

#### MAJOR Version (X.0.0)

Increment when:
- **Breaking API changes**: Public function signatures removed or changed
- **Breaking ABI changes**: Class layout changes, virtual function modifications
- **Removed public functionality**: Deprecated features removed after one major cycle

**Example**:
```cpp
// Before: 1.0.0
class StateMachine {
public:
    void transition(const std::string& state);
};

// After: 2.0.0 (breaking change)
class StateMachine {
public:
    void transition(StateId state);  // Changed signature
};
```

#### MINOR Version (1.X.0)

Increment when:
- **New backward-compatible features**: New public APIs added
- **Enhanced functionality**: New capabilities without breaking existing code
- **Deprecations**: Marking features as deprecated (not removed)

**Example**:
```cpp
// Before: 1.0.0
class StateMachine {
public:
    void transition(const std::string& state);
};

// After: 1.1.0 (backward compatible)
class StateMachine {
public:
    void transition(const std::string& state);
    void transitionAsync(const std::string& state);  // New feature
};
```

#### PATCH Version (1.0.X)

Increment when:
- **Bug fixes**: Backward-compatible bug fixes
- **Documentation updates**: Improved docs without code changes
- **Performance improvements**: Optimizations that don't change behavior
- **Internal refactoring**: Code cleanup without API changes

**Example**:
```cpp
// Before: 1.0.0
void StateMachine::transition(const std::string& state) {
    // Has a bug in edge case
}

// After: 1.0.1 (bug fix)
void StateMachine::transition(const std::string& state) {
    // Bug fixed, API unchanged
}
```

### Pre-release Identifiers

Pre-release versions indicate stability levels before a major/minor release:

| Identifier | Purpose | Usage |
|------------|---------|-------|
| `alpha` | Feature incomplete, unstable API | Early testing, feature development |
| `beta` | Feature complete, API stabilizing | Public testing, feedback gathering |
| `rc` | Release candidate, API frozen | Final testing before stable release |

**Pre-release version ordering**:
```
1.0.0-alpha.1 < 1.0.0-alpha.2 < 1.0.0-beta.1 < 1.0.0-beta.2 < 1.0.0-rc.1 < 1.0.0
```

**Examples**:
- `1.0.0-alpha.1` - First alpha of version 1.0.0
- `1.0.0-beta.1` - First beta of version 1.0.0
- `1.0.0-rc.1` - First release candidate of version 1.0.0
- `1.1.0-alpha.1` - First alpha of version 1.1.0

### Build Metadata

Build metadata MAY be appended for build-specific information:

**Format**: `+BUILD`

**Examples**:
- `1.0.0+20240115` - Date-based build
- `1.0.0+sha.abc123` - Git SHA-based build
- `1.0.0+ubuntu.22.04` - Platform-specific build

**Note**: Build metadata MUST be ignored when determining version precedence.

### ABI Compatibility Guidelines

For C++ libraries, ABI compatibility is critical when distributing shared libraries (.so/.dll).

#### Maintaining ABI Compatibility

**ABI-compatible changes** (MINOR or PATCH):
- Adding new non-virtual functions to classes
- Adding new free functions
- Adding new classes/types
- Changing inline function implementations (with care)
- Changing default arguments (with care)

**ABI-breaking changes** (MAJOR):
- Adding/removing/changing virtual functions
- Adding non-static data members to classes
- Changing class size or layout
- Changing template definitions
- Removing any public API
- Changing function signatures

#### Recommended Practices

1. **PIMPL Idiom**: Use for classes likely to change
   ```cpp
   // Header (ABI stable)
   class StateMachine {
   public:
       StateMachine();
       ~StateMachine();
       void transition(const std::string& state);
   private:
       class Impl;
       std::unique_ptr<Impl> pImpl;
   };
   
   // Source (can change without breaking ABI)
   class StateMachine::Impl {
       // Implementation details here
   };
   ```

2. **Versioned namespaces**: For major ABI changes
   ```cpp
   namespace fsmconfig::v1 {
       class StateMachine { /* ... */ };
   }
   namespace fsmconfig::v2 {
       class StateMachine { /* ... */ };
   }
   ```

3. **Shared library versioning** (SONAME):
   - `libfsmconfig.so.1` for MAJOR version 1.x
   - `libfsmconfig.so.2` for MAJOR version 2.x

#### Deprecation Policy

**Deprecation process**:
1. Mark as deprecated in documentation (`@deprecated` tag)
2. Emit compiler warnings if possible (`[[deprecated]]` attribute)
3. Maintain for at least one full MAJOR version cycle
4. Remove in next MAJOR version

**Example**:
```cpp
// 1.0.0: Introduced
[[deprecated("Use transitionAsync() instead")]]
void syncTransition(const std::string& state);

// 1.1.0 - 1.x: Maintained but deprecated
// 2.0.0: Removed
```

### Release Workflow

#### Branch Strategy

```
main (or master)
  ├── Production-ready code
  ├── Tags for releases (v1.0.0, v1.1.0, etc.)
  └── Always stable

develop
  ├── Integration branch for features
  ├── Next release development
  └── May be unstable

feature/xxx
  ├── Feature development
  └── Merged to develop

release/1.0.0
  ├── Release preparation
  ├── Bug fixes only
  └── Merged to main and develop

hotfix/1.0.1
  ├── Emergency fixes for main
  └── Merged to main and develop
```

#### Release Process

**Pre-release (alpha/beta/rc)**:
1. Create release branch from `develop`: `release/1.0.0-alpha.1`
2. Update version in [`CMakeLists.txt`](../../CMakeLists.txt:2)
3. Update CHANGELOG.md
4. Create pre-release tag: `v1.0.0-alpha.1`
5. Publish as pre-release package
6. Merge back to `develop`

**Stable release**:
1. Create release branch from `develop`: `release/1.0.0`
2. Final testing and bug fixes
3. Update version in [`CMakeLists.txt`](../../CMakeLists.txt:2)
4. Update CHANGELOG.md
5. Create release tag: `v1.0.0`
6. Publish release on GitHub
7. Publish to package managers
8. Merge to `main` and `develop`

**Hotfix release**:
1. Create hotfix branch from `main`: `hotfix/1.0.1`
2. Fix the issue
3. Update version and CHANGELOG
4. Create tag: `v1.0.1`
5. Publish release
6. Merge to `main` and `develop`

#### Release Checklist

For each release:
- [ ] All tests passing
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] Version numbers updated
- [ ] ABI compatibility verified (for stable releases)
- [ ] Release notes prepared
- [ ] Tag created and pushed
- [ ] GitHub release published
- [ ] Packages published to package managers

### CMake Integration

#### Version Storage in CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(FSMConfig 
    VERSION 1.0.0
    LANGUAGES CXX
)

# Version components
set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(PROJECT_VERSION_TWEAK 0)

# Pre-release version (if applicable)
# set(PROJECT_VERSION_PRE_RELEASE "alpha.1")
```

#### Generating version.h

Create [`include/fsmconfig/version.hpp.in`](../../include/fsmconfig/version.hpp.in):

```cpp
#pragma once

#define FSMCONFIG_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define FSMCONFIG_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define FSMCONFIG_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define FSMCONFIG_VERSION_TWEAK @PROJECT_VERSION_TWEAK@

#define FSMCONFIG_VERSION_STRING "@PROJECT_VERSION@"
#define FSMCONFIG_VERSION_ID \
    ((FSMCONFIG_VERSION_MAJOR * 1000000) + \
     (FSMCONFIG_VERSION_MINOR * 1000) + \
     (FSMCONFIG_VERSION_PATCH))

namespace fsmconfig {

/// Version information structure
struct Version {
    constexpr Version(int major, int minor, int patch)
        : major(major), minor(minor), patch(patch) {}
    
    int major;
    int minor;
    int patch;
    
    /// Returns the current library version
    static constexpr Version current() {
        return Version(FSMCONFIG_VERSION_MAJOR,
                      FSMCONFIG_VERSION_MINOR,
                      FSMCONFIG_VERSION_PATCH);
    }
    
    /// Version string
    static constexpr const char* string() {
        return FSMCONFIG_VERSION_STRING;
    }
};

} // namespace fsmconfig
```

In [`CMakeLists.txt`](../../CMakeLists.txt):

```cmake
# Generate version.hpp from template
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/fsmconfig/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/fsmconfig/version.hpp"
    @ONLY
)

# Add to include path
target_include_directories(fsmconfig PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

#### Package Configuration

The CMake package configuration file ([`cmake/fsmconfig-config.cmake.in`](../../cmake/fsmconfig-config.cmake.in)) already handles version compatibility:

```cmake
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fsmconfig-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
```

This ensures that projects using `find_package(fsmconfig 1.1 REQUIRED)` will find any 1.x version.

### Version Examples by Scenario

#### Scenario 1: First Stable Release

**Current**: `0.1.0` (development)

**Sequence**:
1. `1.0.0-alpha.1` - First alpha, feature incomplete
2. `1.0.0-alpha.2` - Second alpha, more features
3. `1.0.0-beta.1` - First beta, feature complete
4. `1.0.0-rc.1` - First release candidate
5. `1.0.0` - Stable release

#### Scenario 2: New Feature Release

**Current**: `1.0.0`

**Sequence**:
1. Develop new features on `develop` branch
2. `1.1.0-alpha.1` - Alpha for new features
3. `1.1.0-beta.1` - Beta for new features
4. `1.1.0` - Stable feature release

#### Scenario 3: Bug Fix Release

**Current**: `1.0.0`

**Sequence**:
1. Bug discovered in production
2. `1.0.1` - Patch release with bug fix

#### Scenario 4: Breaking Changes

**Current**: `1.2.0`

**Sequence**:
1. Plan breaking API changes
2. `2.0.0-alpha.1` - Alpha with new API
3. `2.0.0-beta.1` - Beta with new API
4. `2.0.0` - Stable release with breaking changes

## Consequences

### Positive

- **Clear communication**: Users understand API stability guarantees
- **Package manager support**: SemVer is widely supported (vcpkg, conan)
- **Predictable releases**: Version numbers convey meaning
- **ABI awareness**: Explicit guidelines for C++ binary compatibility
- **Professional appearance**: Industry-standard versioning
- **Easy dependency management**: Tools can automatically handle updates

### Negative

- **Initial learning curve**: Team must understand SemVer rules
- **Discipline required**: Must follow versioning rules consistently
- **C++ ABI complexity**: Additional considerations vs. other languages
- **Potential for version fatigue**: Many pre-releases before stable

### Risks

- **Breaking changes misclassification**: Accidental MAJOR bump for MINOR changes
- **ABI breaks in MINOR releases**: Unintended binary incompatibility
- **Pre-release fatigue**: Too many alpha/beta releases
- **Version number anxiety**: Hesitation to reach 1.0.0

### Mitigations

- **Code review checklist**: Verify version bump correctness
- **ABI compatibility tools**: Use `abi-compliance-checker` or `abi-dumper`
- **Clear documentation**: Document all API changes
- **Automated testing**: Comprehensive test coverage for API/ABI
- **Release planning**: Plan releases in advance with clear goals

## Alternatives Considered

### 1. Calendar Versioning (CalVer)

**Format**: `YYYY.MM.DD`

**Example**: `2024.01.15`

**Rejected because**:
- Doesn't convey API compatibility information
- Less common for C++ libraries
- Package managers expect SemVer

### 2. Simple Major.Minor

**Format**: `MAJOR.MINOR`

**Example**: `1.0`, `1.1`, `2.0`

**Rejected because**:
- Doesn't distinguish bug fixes from features
- Industry standard is SemVer
- Package managers prefer three-part versions

### 3. Continuous Versioning

**Format**: Increment build number on every commit

**Example**: `0.0.1234`

**Rejected because**:
- No semantic meaning
- Difficult to determine compatibility
- Not suitable for public releases

### 4. No Formal Versioning

**Approach**: Use git commit SHAs or dates

**Rejected because**:
- No compatibility guarantees
- Cannot publish to package managers
- Unprofessional for a library

## Implementation

### Phase 1: Immediate (for first release)

- [ ] Update [`CMakeLists.txt`](../../CMakeLists.txt:2) to `1.0.0-alpha.1`
- [ ] Create `version.hpp.in` template
- [ ] Add version.hpp generation to build
- [ ] Create CHANGELOG.md
- [ ] Document versioning strategy in [`README.md`](../../README.md)
- [ ] Create this ADR

### Phase 2: Pre-release

- [ ] Set up release branches
- [ ] Create pre-release tags
- [ ] Test pre-release packaging
- [ ] Gather user feedback

### Phase 3: Stable Release

- [ ] Finalize `1.0.0` release
- [ ] Publish to GitHub Releases
- [ ] Prepare for package managers (vcpkg, conan)
- [ ] Establish release schedule

### Phase 4: Long-term

- [ ] Set up automated release workflow (GitHub Actions)
- [ ] Integrate ABI compatibility checking
- [ ] Establish deprecation policy enforcement
- [ ] Consider LTS releases for major versions

## References

- [Semantic Versioning 2.0.0](https://semver.org/)
- [C++ ABI Compatibility](https://stackoverflow.com/questions/2171177/what-is-an-application-binary-interface-abi)
- [PIMPL Idiom in C++](https://en.cppreference.com/w/cpp/language/pimpl)
- [CMake Versioning](https://cmake.org/cmake/help/latest/command/project.html#version)
- [Package Manager Versioning](https://github.com/microsoft/vcpkg/blob/master/docs/users/versioning.md)

## Related Documents

- [`ADR-001: Separation of Permanent and Temporary Documentation`](adr-001-documentation-workflow.md)
- [`CMakeLists.txt`](../../CMakeLists.txt) - Build configuration
- [`README.md`](../../README.md) - Project overview
