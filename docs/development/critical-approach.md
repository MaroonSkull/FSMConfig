# Critical Approach

## Philosophy

**Question every decision**

- Every decision must be justified
- Relate decisions to existing parts of the system
- Relate to external decisions outside the codebase
- Find errors in plans and iteratively eliminate them

## Development Cycle

```
Think a little
   ↓
Criticize
   ↓
Rework
   ↓
Criticize again
   ↓
Rework again
   ↓
(Repeat until no more errors are found)
   ↓
Smooth integration into code
```

## During Integration

**Re-verify the codebase:**

1. **DRY (Don't Repeat Yourself)**
   - Search for duplicate code
   - Extract common parts into separate functions/classes

2. **Search for Existing Elements**
   - Check if the required functionality already exists
   - Use existing utilities and helpers

3. **Search for Ready-Made Libraries on the Internet**
   - Use tavily to search
   - Find 2-3 alternatives
   - Propose library integration to the user
   - Wait for response

## Example of Critical Analysis

**Questions to ask yourself:**

1. Why exactly this solution?
2. Is there a simpler way?
3. Does this violate existing patterns?
4. Will this create problems in the future?
5. Do ready-made libraries exist for this task?
6. Am I duplicating existing code?
7. Will tests cover this change?
8. Is this change sufficiently documented?

## Tools for Critical Analysis

| Mode | Purpose |
|------|---------|
| **code-skeptic** | Critical code analysis for problems |
| **code-reviewer** | Thorough review of changes |
| **code-simplifier** | Finding opportunities for simplification |
| **debug** | Analyzing potential issues |
