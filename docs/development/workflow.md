# Git Workflow (Trunk-Based Development)

## General Process

```
1. Briefly analyze the task (orchestrator mode)
   ↓
2. Come up with a task name
   ↓
3. Create a new branch for the task
   ↓
4. Create additional branches for individual subtasks
   ↓
5. After completing a subtask — merge into the working branch
```

## Creating Branches

**Branch naming format:**

```
feature/short-task-description
bugfix/bug-description
refactor/refactoring-description
docs/documentation-description
```

**Examples:**

```bash
git checkout -b feature/add-state-persistence
git checkout -b bugfix/fix-memory-leak-in-parser
git checkout -b refactor/optimize-event-dispatcher
```

## Commits

**Commit rules:**

1. **Frequency:** Commit as often as possible
2. **History:** Commit history should be rich and informative
3. **RULE:** Before creating a commit, ensure:
   - Code works correctly OR
   - Code at least compiles without errors

**Commit message format:**

```
<type>: <short description>

<detailed description (optional)>

<task references (optional)>
```

**Commit types:**

- `feat` — new functionality
- `fix` — bug fix
- `refactor` — refactoring
- `docs` — documentation
- `test` — tests
- `chore` — routine tasks

**Examples:**

```
feat: add support for nested states

Implemented hierarchical state structure with inheritance.
Added tests in test_state_machine.cpp.

Closes #42
```

```
fix: fix memory leak in ConfigParser

The problem was missing YAML::Node cleanup.
Now using smart pointer.
```

## Completing a Task

```
1. Critical analysis of git diff
   └─ Use review mode
   ↓
2. Check and update documentation
   ├─ Are all affected places documented?
   └─ Is AGENTS.md updated to reflect the current project state?
       │
       └─ **IMPORTANT:** Before creating a commit, a separate task must be created
                   to update the relevant sections of AGENTS.md if the
                   completed work affects the project structure, technologies,
                   development processes, or other aspects described in this file.
   ↓
3. Merge branches
   └─ Merge all additional branches into the main working branch
   ↓
4. The working branch does not need to be merged anywhere
   └─ The programmer will analyze it
```

**Review process:**

```bash
# Show all changes
git diff main..feature/branch-name

# Interactive review
git review main..feature/branch-name
```
