# Assembly Language Guide

This is a complete tutorial for writing programs in the MyLangVM assembly language. By the end, you will know how to use every instruction, build loops, write functions, use memory, and handle user input.

---

## Table of Contents

1. [The Basics](#the-basics)
   - [What Does a Program Look Like?](#what-does-a-program-look-like)
   - [How the Stack Works](#how-the-stack-works)
   - [File Format Rules](#file-format-rules)
2. [Pushing and Printing Values](#pushing-and-printing-values)
3. [Arithmetic](#arithmetic)
   - [Addition](#addition)
   - [Subtraction](#subtraction)
   - [Multiplication](#multiplication)
   - [Division](#division)
   - [Modulo](#modulo)
4. [Unary Operations](#unary-operations)
   - [Negative](#negative)
   - [Positive](#positive)
5. [Stack Manipulation](#stack-manipulation)
   - [Duplicate](#duplicate)
   - [Swap](#swap)
   - [Pop](#pop)
6. [Comparisons](#comparisons)
7. [Control Flow](#control-flow)
   - [Unconditional Jump](#unconditional-jump)
   - [Conditional Jumps](#conditional-jumps)
   - [Building a Loop](#building-a-loop)
   - [Building If-Else Logic](#building-if-else-logic)
8. [Functions (CALL / RETURN)](#functions-call--return)
9. [Memory (STORE / LOAD)](#memory-store--load)
10. [User Input and Output](#user-input-and-output)
11. [Putting It All Together](#putting-it-all-together)

---

## The Basics

### What Does a Program Look Like?

A MyLangVM assembly program is a plain text file (typically with a `.asm` extension) containing one instruction per line. Here is the simplest complete program:

```asm
PUSH 42
PRINT
HALT
```

This program does three things:
1. Pushes the number `42` onto the stack.
2. Pops the top of the stack and prints it.
3. Halts execution.

**Output:** `42`

Every program **must** end with a `HALT` instruction. If the VM reaches the end of the bytecode without encountering `HALT`, it will print an error message and stop.

### How the Stack Works

MyLangVM is a **stack-based** virtual machine. There are no named variables or registers. Instead, all values are pushed onto and popped off of a stack — a last-in, first-out (LIFO) data structure.

Think of the stack as a pile of plates:

```
              ┌─────┐
              │  3  │  ← top (most recent push)
              ├─────┤
              │  7  │
              ├─────┤
              │  42 │  ← bottom (first push)
              └─────┘
```

- **`PUSH`** puts a new plate on top.
- **`POP`**, **`PRINT`**, and most other instructions take plates off the top.
- Binary operations (like `ADD`) take the top **two** plates, combine them, and put the result back.

The key rule: **operands are consumed from top to bottom**. When `ADD` runs on the stack above, it pops `3` (top) and `7` (second), adds them as `7 + 3 = 10`, and pushes `10`.

The stack has a maximum capacity of **100 elements**. Exceeding this causes a stack overflow error. Popping from an empty stack causes a stack underflow error.

### File Format Rules

- **One instruction per line** (with its operand, if any).
- **Blank lines are allowed** — they are ignored.
- **Case-insensitive** — `PUSH`, `push`, `Push`, and `pUsH` are all valid.
- **Whitespace between instruction and operand** — `PUSH 5` (space-separated).
- **Negative numbers** are supported — `PUSH -10` works correctly.
- **Integer range** — values must fit in a 32-bit signed integer: −2,147,483,648 to 2,147,483,647.
- **No comments syntax** — the assembler does not currently support inline comments. Every non-blank token is treated as an instruction or operand.

---

## Pushing and Printing Values

The most fundamental instructions are `PUSH` and `PRINT`.

**`PUSH <value>`** — Pushes an integer onto the operand stack.

**`PRINT`** — Pops the top value from the stack and prints it to the console, followed by a newline.

```asm
PUSH 100
PRINT
HALT
```

**Output:** `100`

You can push and print multiple values:

```asm
PUSH 10
PUSH 20
PUSH 30
PRINT
PRINT
PRINT
HALT
```

**Output:**

```
30
20
10
```

Notice the order: `30` is printed first because it was on top of the stack (last in, first out).

---

## Arithmetic

All arithmetic instructions pop two values from the stack, perform the operation, and push the result back. The important thing to understand is **operand order**.

When the VM pops two values for a binary operation:
- The **first pop** yields the **right-hand operand** (it was on top).
- The **second pop** yields the **left-hand operand** (it was below).

So the result is always: `left OP right`, which matches the order you pushed them in.

### Addition

```asm
PUSH 5
PUSH 3
ADD
PRINT
HALT
```

Stack trace:
```
PUSH 5  →  [5]
PUSH 3  →  [5, 3]
ADD     →  pop 3, pop 5, push 5+3=8  →  [8]
PRINT   →  pop 8, print "8"  →  []
```

**Output:** `8`

### Subtraction

```asm
PUSH 20
PUSH 6
SUBTRACT
PRINT
HALT
```

Stack trace:
```
PUSH 20    →  [20]
PUSH 6     →  [20, 6]
SUBTRACT   →  pop 6, pop 20, push 20-6=14  →  [14]
```

**Output:** `14`

### Multiplication

```asm
PUSH -2
PUSH 7
MULTIPLY
PRINT
HALT
```

**Output:** `-14`

Negative numbers work naturally. `PUSH -2` pushes `−2`, and `−2 × 7 = −14`.

### Division

```asm
PUSH 100
PUSH 4
DIVIDE
PRINT
HALT
```

**Output:** `25`

Division is **integer division** (truncated toward zero). Dividing by zero causes an immediate runtime error — the VM prints `Division by zero !!` and exits.

### Modulo

```asm
PUSH 17
PUSH 5
MODULO
PRINT
HALT
```

**Output:** `2`

Modulo computes the remainder after division. `17 % 5 = 2`. Modulo by zero causes an immediate runtime error.

---

## Unary Operations

Unary operations modify a single value — the top of the stack — without popping a second value.

### Negative

**`NEGATIVE`** — If the top value is positive, it becomes negative. If it is already zero or negative, it is left unchanged.

```asm
PUSH 7
NEGATIVE
PRINT
HALT
```

**Output:** `-7`

### Positive

**`POSITIVE`** — If the top value is negative, it becomes positive (absolute value). If it is already zero or positive, it is left unchanged.

```asm
PUSH -9
POSITIVE
PRINT
HALT
```

**Output:** `9`

---

## Stack Manipulation

These instructions rearrange or remove values on the stack without performing arithmetic.

### Duplicate

**`DUPLICATE`** — Copies the top value and pushes the copy. The original remains.

```asm
PUSH 99
DUPLICATE
PRINT
PRINT
HALT
```

**Output:**

```
99
99
```

`DUPLICATE` is useful when you need to use a value in two different operations without losing it.

### Swap

**`SWAP`** — Exchanges the top two values on the stack.

```asm
PUSH 10
PUSH 20
SWAP
PRINT
PRINT
HALT
```

**Output:**

```
10
20
```

Without `SWAP`, `PRINT` would output `20` then `10`. The swap reverses their positions.

### Pop

**`POP`** — Removes the top value from the stack and discards it.

```asm
PUSH 1
PUSH 2
POP
PRINT
HALT
```

**Output:** `1`

The `2` was pushed then immediately popped (discarded). Only `1` remains.

---

## Comparisons

All comparison instructions pop two values, compare them, and push the result as a boolean integer: **`1`** for true, **`0`** for false.

Like arithmetic, the operand order is: the **first push** becomes the **left operand**, and the **second push** becomes the **right operand**.

| Instruction | Meaning | Example |
|:---|:---|:---|
| `EQUAL` | `left == right` | `PUSH 5`, `PUSH 5`, `EQUAL` → `1` |
| `NOT_EQUAL` | `left != right` | `PUSH 10`, `PUSH 5`, `NOT_EQUAL` → `1` |
| `GREATER_THAN` | `left > right` | `PUSH 15`, `PUSH 8`, `GREATER_THAN` → `1` |
| `GREATER_EQUAL` | `left >= right` | `PUSH 10`, `PUSH 10`, `GREATER_EQUAL` → `1` |
| `LESS_THAN` | `left < right` | `PUSH 3`, `PUSH 9`, `LESS_THAN` → `1` |
| `LESS_EQUAL` | `left <= right` | `PUSH 4`, `PUSH 4`, `LESS_EQUAL` → `1` |

Example:

```asm
PUSH 25
PUSH 25
EQUAL
PRINT
HALT
```

**Output:** `1` (true — 25 equals 25)

```asm
PUSH 3
PUSH 9
LESS_THAN
PRINT
HALT
```

**Output:** `1` (true — 3 is less than 9)

Comparisons are essential for conditional jumps, which we will cover next.

---

## Control Flow

### Understanding Instruction Numbers

Before we discuss jumps, you need to understand **logical instruction numbering**.

Every instruction in your program has a **logical index**, counting from `0`. This is simply the order of instructions, ignoring blank lines:

```asm
PUSH 5       ← instruction 0
PUSH 3       ← instruction 1
ADD          ← instruction 2
PRINT        ← instruction 3
HALT         ← instruction 4
```

When you write `JUMP 3`, you are saying "jump to logical instruction 3" — which is `PRINT` in this example. You are **not** jumping to line 3 of the file or byte 3 in the bytecode. The VM handles the translation from logical instruction numbers to actual byte positions automatically.

**Important**: Be careful when counting instructions. `PUSH 5` is **one** instruction (instruction 0), not two. Even though it has an operand, it counts as a single logical instruction.

### Unconditional Jump

**`JUMP <target>`** — Jumps to the specified logical instruction number. Execution continues from there.

```asm
JUMP 2        ← instruction 0: jump to instruction 2

PUSH 999      ← instruction 1: SKIPPED

PUSH 42       ← instruction 2: execution resumes here
PRINT         ← instruction 3
HALT          ← instruction 4
```

**Output:** `42`

Instruction 1 (`PUSH 999`) is never executed because the `JUMP` skips past it.

### Conditional Jumps

**`JUMP_IF_ZERO <target>`** — Peeks at the top of the stack. If the value is `0`, it jumps to `<target>`. If the value is not `0`, execution continues to the next instruction. Either way, the top value is **consumed** (popped).

**`JUMP_IF_NOT_ZERO <target>`** — The opposite: jumps if the top value is **not** `0`.

```asm
PUSH 0              ← instruction 0
JUMP_IF_ZERO 4      ← instruction 1: top is 0, so jump to instruction 4

PUSH 999            ← instruction 2: SKIPPED
PRINT               ← instruction 3: SKIPPED

PUSH 42             ← instruction 4: execution lands here
PRINT               ← instruction 5
HALT                ← instruction 6
```

**Output:** `42`

Since we pushed `0` and then used `JUMP_IF_ZERO`, the condition was true, so execution jumped to instruction 4 and skipped instructions 2 and 3.

### Building a Loop

Loops are built by combining a conditional jump with a backward `JUMP`. Here is a program that prints the numbers 5 down to 1:

```asm
PUSH 5              ← instruction 0: counter = 5

DUPLICATE           ← instruction 1: copy counter for printing
PRINT               ← instruction 2: print the counter

PUSH 1              ← instruction 3: push 1
SUBTRACT            ← instruction 4: counter = counter - 1

DUPLICATE           ← instruction 5: copy counter for the condition check
JUMP_IF_NOT_ZERO 1  ← instruction 6: if counter != 0, jump back to instruction 1

POP                 ← instruction 7: clean up the remaining 0
HALT                ← instruction 8
```

**Output:**

```
5
4
3
2
1
```

**How it works:**
1. Push `5` (the counter).
2. Duplicate it so we can print it without losing it.
3. Print the copy.
4. Subtract `1` from the counter.
5. Duplicate the counter again to check if it's zero.
6. If it's not zero, jump back to instruction 1 (the start of the loop body).
7. When the counter reaches 0, `JUMP_IF_NOT_ZERO` falls through, and we pop the leftover `0` and halt.

### Building If-Else Logic

You can build simple if-else structures by combining comparison and conditional jump instructions:

```asm
PUSH 10             ← instruction 0: value A
PUSH 20             ← instruction 1: value B
GREATER_THAN        ← instruction 2: is A > B? → pushes 0 (false)
JUMP_IF_NOT_ZERO 6  ← instruction 3: if true, jump to "then" branch

PUSH 20             ← instruction 4: ELSE branch — print B (the larger one)
JUMP 7              ← instruction 5: skip past the "then" branch

PUSH 10             ← instruction 6: THEN branch — print A (the larger one)

PRINT               ← instruction 7: shared — print whichever was chosen
HALT                ← instruction 8
```

**Output:** `20` (because 10 is not greater than 20, so the else branch runs)

---

## Functions (CALL / RETURN)

**`CALL <target>`** — Saves the return address on the call stack, then jumps to `<target>`. Think of it as "go do this other thing, then come back."

**`RETURN`** — Pops the return address from the call stack and jumps back to it. Think of it as "I'm done, go back to where I was called from."

```asm
CALL 3        ← instruction 0: call the function at instruction 3
PRINT         ← instruction 1: runs after the function returns
HALT          ← instruction 2

PUSH 99       ← instruction 3: function body — push 99
RETURN        ← instruction 4: return to caller (instruction 1)
```

**Output:** `99`

**How it works:**
1. `CALL 3` pushes the return address (the instruction **after** the call, which is instruction 1) onto the call stack, then jumps to instruction 3.
2. At instruction 3, `PUSH 99` pushes `99` onto the operand stack.
3. `RETURN` pops the return address from the call stack and jumps back to instruction 1.
4. At instruction 1, `PRINT` pops and prints `99`.
5. `HALT` ends the program.

The call stack is separate from the operand stack, so the function's `PUSH 99` doesn't interfere with the return address. This is a core design strength of MyLangVM's dual-stack architecture.

Functions can use the operand stack to pass arguments and return values. The calling convention is entirely up to you — there is no enforced ABI. A simple approach: push the arguments before `CALL`, and have the function leave the result on the operand stack when it executes `RETURN`.

---

## Memory (STORE / LOAD)

The stack is temporary — values are constantly being pushed and popped. For longer-lived storage, MyLangVM provides a **1024-word memory array** (indices 0 through 1023).

**`STORE <index>`** — Pops the top of the operand stack and writes it to `memory[index]`.

**`LOAD <index>`** — Reads the value at `memory[index]` and pushes a copy onto the operand stack. The memory cell is not modified.

```asm
PUSH 123       ← push the value we want to store
STORE 5        ← pop 123 and write it to memory[5]
LOAD 5         ← read memory[5] (which is 123) and push it
PRINT          ← print 123
HALT
```

**Output:** `123`

You can overwrite a memory cell by storing a new value to the same index:

```asm
PUSH 10
STORE 0        ← memory[0] = 10

PUSH 99
STORE 0        ← memory[0] = 99 (overwrites the 10)

LOAD 0         ← pushes 99
PRINT
HALT
```

**Output:** `99`

Memory is useful for implementing variables. You can mentally assign names to indices:

```
memory[0] = "counter"
memory[1] = "result"
memory[2] = "temp"
```

There is nothing enforcing these names — it's just a convention you follow in your head (or in comments). The VM just sees numbers.

---

## User Input and Output

**`INPUT`** — Prompts the user for an integer and pushes it onto the stack. The prompt text is `Enter the value : `.

**`PRINT`** — Pops the top value and prints it to the console, followed by a newline.

### Simple Input-Output

```asm
INPUT
PRINT
HALT
```

**Console:**

```
Enter the value : 42
42
```

### Echo (Print Twice)

```asm
INPUT
DUPLICATE
PRINT
PRINT
HALT
```

**Console:**

```
Enter the value : 7
7
7
```

The `DUPLICATE` copies the input value so we can print it twice.

---

## Putting It All Together

Here is a more complex program that demonstrates multiple features: a multi-step expression evaluator.

This program computes `(10 + 20) * 3 - 15`:

```asm
PUSH 10
PUSH 20
ADD

PUSH 3
MULTIPLY

PUSH 15
SUBTRACT

PRINT
HALT
```

**Stack trace:**

```
PUSH 10     →  [10]
PUSH 20     →  [10, 20]
ADD         →  [30]           (10 + 20)
PUSH 3      →  [30, 3]
MULTIPLY    →  [90]           (30 * 3)
PUSH 15     →  [90, 15]
SUBTRACT    →  [75]           (90 - 15)
PRINT       →  prints 75
```

**Output:** `75`

### Tips for Writing Programs

1. **Always end with `HALT`**. Forgetting `HALT` causes a runtime error.
2. **Count your pushes and pops**. Every value you push must eventually be popped (by an arithmetic instruction, `PRINT`, `POP`, `STORE`, or a comparison). Leftover values on the stack are not harmful, but an imbalance usually indicates a logic error.
3. **Use `DUPLICATE` liberally**. If you need the same value for two operations, duplicate it before the first operation consumes it.
4. **Plan your jump targets on paper**. Number your instructions starting from 0 and make sure your `JUMP`, `CALL`, and conditional jump targets point to the correct instruction.
5. **Use memory for named values**. If your program gets complex, store intermediate results in memory with `STORE` and retrieve them with `LOAD` rather than trying to juggle everything on the stack.

---

## Quick Reference Card

| Category | Instruction | Operand? | Stack Before → After |
|:---|:---|:---:|:---|
| **Data** | `PUSH <val>` | Yes | `[] → [val]` |
| | `POP` | No | `[val] → []` |
| | `DUPLICATE` | No | `[val] → [val, val]` |
| | `SWAP` | No | `[a, b] → [b, a]` |
| **Arithmetic** | `ADD` | No | `[a, b] → [a+b]` |
| | `SUBTRACT` | No | `[a, b] → [a-b]` |
| | `MULTIPLY` | No | `[a, b] → [a*b]` |
| | `DIVIDE` | No | `[a, b] → [a/b]` |
| | `MODULO` | No | `[a, b] → [a%b]` |
| **Unary** | `NEGATIVE` | No | `[val] → [-val]` if val > 0 |
| | `POSITIVE` | No | `[val] → [abs(val)]` if val < 0 |
| **Comparison** | `EQUAL` | No | `[a, b] → [a==b ? 1 : 0]` |
| | `NOT_EQUAL` | No | `[a, b] → [a!=b ? 1 : 0]` |
| | `GREATER_THAN` | No | `[a, b] → [a>b ? 1 : 0]` |
| | `GREATER_EQUAL` | No | `[a, b] → [a>=b ? 1 : 0]` |
| | `LESS_THAN` | No | `[a, b] → [a<b ? 1 : 0]` |
| | `LESS_EQUAL` | No | `[a, b] → [a<=b ? 1 : 0]` |
| **Control** | `JUMP <target>` | Yes | `[] → []` |
| | `JUMP_IF_ZERO <target>` | Yes | `[cond] → []` |
| | `JUMP_IF_NOT_ZERO <target>` | Yes | `[cond] → []` |
| | `CALL <target>` | Yes | `[] → []` (pushes to call stack) |
| | `RETURN` | No | `[] → []` (pops from call stack) |
| **Memory** | `STORE <index>` | Yes | `[val] → []` |
| | `LOAD <index>` | Yes | `[] → [val]` |
| **I/O** | `INPUT` | No | `[] → [input]` |
| | `PRINT` | No | `[val] → []` |
| **Halt** | `HALT` | No | Stops execution |

---

## Next Steps

- **[Instruction Set Reference](04-instruction-set-reference.md)** — Detailed specification for each instruction
- **[Examples Cookbook](08-examples-cookbook.md)** — Annotated walkthroughs of the included example programs
- **[Bytecode Format](07-bytecode-format.md)** — What the assembler actually produces
