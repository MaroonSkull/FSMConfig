# Task Strategy

## Principle

Critical and complex tasks should be broken down into many small, independent subtasks.

## Why This Is Needed

### 1. Reducing Context Load

- Neural networks have limited context size
- Smaller tasks require less context
- This improves the quality of each subtask completion

### 2. Preparation for Parallel Execution

- Small independent tasks can be executed in parallel
- This speeds up overall completion time
- In the future, support for parallel agent calls will be implemented

## How to Break Down Tasks

### Rule 1: One Task — One Specific Goal

- ❌ Bad: "Analyze the entire project and fix all problems"
- ✅ Good: "Check C++ standard compliance in CMakeLists.txt"

### Rule 2: Task Should Be Complete

- Each subtask should have a clear completion criterion
- The result of a subtask should be independently useful

### Rule 3: Minimize Dependencies Between Subtasks

- Subtasks should be as independent as possible
- If subtask B depends on subtask A, they should be executed sequentially

## Breakdown Examples

### Example 1: Critical Documentation Analysis

Instead of one large task "Analyze AGENTS.md":

- ✅ Subtask 1: "Check C++ standard compliance"
- ✅ Subtask 2: "Check YAML configuration format"
- ✅ Subtask 3: "Check for linter configuration files"
- ✅ Subtask 4: "Check file structure compliance"

### Example 2: Code Refactoring

Instead of "Refactor all project classes":

- ✅ Subtask 1: "Refactor StateMachine class"
- ✅ Subtask 2: "Refactor ConfigParser class"
- ✅ Subtask 3: "Refactor EventDispatcher class"

### Example 3: Adding Documentation

Instead of "Document the entire project":

- ✅ Subtask 1: "Document StateMachine methods"
- ✅ Subtask 2: "Document ConfigParser methods"
- ✅ Subtask 3: "Document EventDispatcher methods"

## When NOT to Break Down

- If the task is trivial and takes less than 2-3 minutes
- If breaking down would create more complexity than it solves
- If subtasks would be heavily dependent on each other

## Practical Recommendations

1. **Time target:** Ideal subtask takes 5-15 minutes
2. **Context target:** Subtask should use no more than 20-30% of available context
3. **Result target:** Each subtask should produce a concrete measurable result

## For Orchestrator Mode

When delegating tasks:

1. Analyze task complexity
2. If the task is complex — break it down into subtasks
3. Create separate subtasks for different aspects
4. Coordinate subtask execution
5. Gather results into a coherent whole
