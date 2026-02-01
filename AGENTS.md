# AGENTS.md

## Table of Contents

1. [Project Overview](#project-overview)
2. [General Work Guidelines](#general-work-guidelines)
3. [Additional Resources](#additional-resources)

---

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

## General Work Guidelines

### IMPORTANT: Using Context

For effective work with the project, ALWAYS use:

1. **"use context7"** — to get context about the current state of the project
2. **"Knowledge Graph Memory"** — for saving and retrieving knowledge about the project

### Rules for Creating .md Files

**MANDATORY RULE:** All `.md` files must be created exclusively inside the `docs/` directory.

**Exceptions:**
- [`README.md`](README.md) at project root — main project description
- [`AGENTS.md`](AGENTS.md) at project root — agent work rules
- [`LICENSE`](LICENSE) at project root — license file
- [`DOCKER.md`](DOCKER.md) at project root — Docker instructions

**Structure of `docs/` directory:**
- You can create any subfolders with any structure
- Examples of existing paths:
  - [`docs/architecture.md`](docs/architecture.md)
  - [`docs/api_reference.md`](docs/api_reference.md)
  - [`docs/examples.md`](docs/examples.md)
  - `docs/architecture/decisions/` — for ADR (Architecture Decision Records)

**Important:** If the user has not explicitly asked to create an .md file in another directory, ALWAYS create .md files inside `docs/`. This ensures centralized storage of all technical project documentation.

### MCP Memory Server

**Purpose:** MCP Memory Server provides LLM with the ability to save and retrieve knowledge about the project in the form of a knowledge graph.

**Configuration:** The server is launched via Docker with persistent storage `claude-memory:/app/dist`.

**Available tools:**
1. `create_entities` — creation of entities (classes, files, libraries)
2. `create_relations` — creation of relations between entities
3. `add_observations` — adding observations to existing entities
4. `read_graph` — reading the entire knowledge graph
5. `search_nodes` — searching entities by name, type, or content
6. `open_nodes` — getting detailed information about specific entities

**Recommended workflow:**

```
Start of task
    ↓
Check entity existence (search_nodes)
    ↓
    ├─ Found → open_nodes → Analyze information
    └─ Not found → create_entities
    ↓
Add new observations (add_observations)
    ↓
Create relations (create_relations)
```

**When to use Memory Server:**

| Action | Tool | Example |
|--------|------|--------|
| First acquaintance with a component | `create_entities` | Create an entity for a new class |
| Discovering a dependency | `create_relations` | Link a class with a used library |
| Studying implementation details | `add_observations` | Add information about methods |
| Getting context | `read_graph` or `search_nodes` | Retrieve knowledge before a task |
| Checking existence | `search_nodes` | Check if an entity has been created |

**Entity types:**
- `project` — project (FSMConfig)
- `class` — class or structure (StateMachine, ConfigParser)
- `file` — source code file (state_machine.cpp)
- `library` — external dependency (yaml-cpp, gtest)
- `tool` — development tool (clang-tidy, cppcheck)
- `concept` — concept or pattern (RAII, Observer Pattern)

**Relation types:**
- `depends_on` — dependency (A depends on B)
- `uses` — usage (A uses B)
- `part_of` — part (A is part of B)
- `implements` — implementation (A implements B)
- `tests` — testing (A tests B)
- `extends` — inheritance/extension

**Usage example:**

```python
# 1. Check existence
result = mcp__memory__search_nodes(query="StateMachine")

# 2. Create if doesn't exist
if not result["nodes"]:
    mcp__memory__create_entities(
        entities=[{
            "name": "StateMachine",
            "entityType": "class",
            "observations": [
                "Main finite state machine class",
                "Located at include/fsmconfig/state_machine.hpp"
            ]
        }]
    )

# 3. Add observations
mcp__memory__add_observations(
    observations=[{
        "entityName": "StateMachine",
        "contents": [
            "Method processEvent() handles events asynchronously"
        ]
    }]
)

# 4. Create relations
mcp__memory__create_relations(
    relations=[
        {"from": "StateMachine", "to": "State", "relationType": "manages"}
    ]
)
```

**Important:** Memory Server is an additional tool for preserving context between sessions. It does NOT replace code analysis and file reading, but complements them.

### High-Level Task Execution Pipeline

```
1. Make a small action
   ↓
2. Critically analyze the results
   (using code-skeptic, code-reviewer modes)
   ↓
3. If there are errors or shortcomings:
   ├─ Prepare a question for the user for clarification
   └─ Return to step 1 and rework the solution
   ↓
4. If no errors — continue with the task
```

### High-Level Task Solving Process

```
1. Create architectural ADRs
   └─ Location: docs/architecture/decisions/
   └─ **Important:** Directory must be created before use
   ↓
2. Add diagrams
   ├─ Component diagrams
   └─ Data flow diagrams
   ↓
3. Document each critical path
   ↓
4. Conduct code audit based on documents
   ├─ Find duplication (DRY)
   ├─ Find incorrect logic
   └─ Find discrepancies with architectural plan
```

**Example ADR structure:**

```markdown
# ADR-001: Choosing an approach for state management

## Status
Proposed

## Context
Need to determine the method for managing transitions between states...

## Decision
Use the State pattern with delegated management...

## Consequences
Pros: ...
Cons: ...
```

### Using Linters

**Principle:** Use the widest possible set of linters to ensure high code quality.

**Mandatory checks:**

```bash
# clang-tidy with modern checks
clang-tidy src/**/*.cpp \
  --checks='*' \
  -warnings-as-errors='*' \
  -- -Iinclude/ -std=c++20

# cppcheck for additional analysis
cppcheck --enable=all --inconclusive --std=c++20 src/

# clang-format for formatting
clang-format -i src/**/*.cpp include/**/*.hpp
```

### Finding Libraries

**Philosophy:** Don't reinvent the wheel — look for ready-made solutions.

**Process:**

1. When new functionality is needed — first search the internet (tavily)
2. Goal: create simple code using existing libraries
3. Don't fear an abundance of dependencies
4. During development, DO NOT implement new methods yourself
5. If a suitable library is found:
   - Propose its integration to the user
   - Wait for a response
   - Only after approval — integrate

**Search examples:**
- For working with JSON → `nlohmann/json`
- For logging → `spdlog`
- For testing → `Catch2` or `gtest`
- For working with time → `date` (Howard Hinnant)

---

## Additional Resources

### Development Guides and Standards

- **Technology Stack:** More info: [`docs/technologies/tech-stack.md`](docs/technologies/tech-stack.md)
- **Task Breakdown Strategy:** More info: [`docs/development/task-strategy.md`](docs/development/task-strategy.md)
- **Coding Standards:** More info: [`docs/development/coding-standards.md`](docs/development/coding-standards.md)
- **Git Workflow:** More info: [`docs/development/workflow.md`](docs/development/workflow.md)
- **Critical Development Approach:** More info: [`docs/development/critical-approach.md`](docs/development/critical-approach.md)

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

---
