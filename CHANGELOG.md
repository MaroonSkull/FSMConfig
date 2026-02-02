# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial release preparation

## [1.0.0-alpha.1] - 2025-02-02

### Added
- Initial release of FSMConfig library
- YAML-based finite state machine configuration
- Core components: StateMachine, State, ConfigParser, EventDispatcher, CallbackRegistry, VariableManager
- Support for state transitions with events
- Callback system for state enter/exit/transition actions
- Variable management for state machine parameters
- CMake build system with modern target-based approach
- Google Test integration for unit and integration tests
- Three example implementations: simple_fsm, game_state, network_protocol
- Comprehensive documentation (API reference, architecture, examples)
- Linter configuration (clang-tidy, clang-format, cppcheck)
- CI/CD workflows for GitHub Actions

### Dependencies
- C++20 compiler (GCC/Clang)
- CMake 3.15+
- yaml-cpp 0.7.0+
- Google Test

[Unreleased]: https://github.com/MaroonSkull/FSMConfig/compare/v1.0.0-alpha.1...HEAD
[1.0.0-alpha.1]: https://github.com/MaroonSkull/FSMConfig/releases/tag/v1.0.0-alpha.1
