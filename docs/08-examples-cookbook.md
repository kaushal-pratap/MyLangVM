# Examples Cookbook

Annotated walkthroughs of every example program included in the `examples/` directory. Each entry shows the source code, a step-by-step stack trace, and the expected output.

---

## Table of Contents

**Arithmetic**
- [add.asm](#addasm)
- [subtract.asm](#subtractasm)
- [multiply.asm](#multiplyasm)
- [divide.asm](#divideasm)
- [modulo.asm](#moduloasm)

**Comparison**
- [equal.asm](#equalasm)
- [not_equal.asm](#not_equalasm)
- [greater_than.asm](#greater_thanasm)
- [greater_equal.asm](#greater_equalasm)
- [less_than.asm](#less_thanasm)
- [less_equal.asm](#less_equalasm)

**Control Flow**
- [jump.asm](#jumpasm)
- [jump_if_zero.asm](#jump_if_zeroasm)
- [jump_if_not_zero.asm](#jump_if_not_zeroasm)
- [call_return.asm](#call_returnasm)
- [skip_instruction.asm](#skip_instructionasm)

**Demos**
- [calculator_demo.asm](#calculator_demoasm)
- [expression.asm](#expressionasm)
- [memory_demo.asm](#memory_demoasm)
- [stack_demo.asm](#stack_demoasm)

**I/O**
- [input.asm](#inputasm)
- [echo.asm](#echoasm)

**Memory**
- [store_load.asm](#store_loadasm)
- [overwrite.asm](#overwriteasm)

**Stack**
- [duplicate.asm](#duplicateasm)
- [pop.asm](#popasm)
- [swap.asm](#swapasm)

**Unary**
- [negative.asm](#negativeasm)
- [positive.asm](#positiveasm)

---

## Arithmetic

### add.asm

**File**: `examples/arithmetic/add.asm`

```asm
PUSH 5
PUSH 3
ADD   
PRINT
HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 5` | `[5]` |
| 1 | `PUSH 3` | `[5, 3]` |
| 2 | `ADD` | `[8]` — pops 3 and 5, pushes 5+3 |
| 3 | `PRINT` | `[]` — prints `8` |
| 4 | `HALT` | — |

**Output**: `8`

---

### subtract.asm

**File**: `examples/arithmetic/subtract.asm`

```asm
PUSH 20
PUSH 6
SUBTRACT
PRINT
HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 20` | `[20]` |
| 1 | `PUSH 6` | `[20, 6]` |
| 2 | `SUBTRACT` | `[14]` — 20 − 6 |
| 3 | `PRINT` | `[]` — prints `14` |
| 4 | `HALT` | — |

**Output**: `14`

---

### multiply.asm

**File**: `examples/arithmetic/multiply.asm`

```asm
PUSH -2
PUSH 7
MULTIPLY
PRINT
HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH -2` | `[-2]` |
| 1 | `PUSH 7` | `[-2, 7]` |
| 2 | `MULTIPLY` | `[-14]` — (−2) × 7 |
| 3 | `PRINT` | `[]` — prints `-14` |
| 4 | `HALT` | — |

**Output**: `-14`

---

### divide.asm

**File**: `examples/arithmetic/divide.asm`

```asm
PUSH 100
PUSH 4
DIVIDE
PRINT
HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 100` | `[100]` |
| 1 | `PUSH 4` | `[100, 4]` |
| 2 | `DIVIDE` | `[25]` — 100 / 4 |
| 3 | `PRINT` | `[]` — prints `25` |
| 4 | `HALT` | — |

**Output**: `25`

---

### modulo.asm

**File**: `examples/arithmetic/modulo.asm`

```asm
PUSH 17
PUSH 5
MODULO
PRINT
HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 17` | `[17]` |
| 1 | `PUSH 5` | `[17, 5]` |
| 2 | `MODULO` | `[2]` — 17 % 5 |
| 3 | `PRINT` | `[]` — prints `2` |
| 4 | `HALT` | — |

**Output**: `2`

---

## Comparison

### equal.asm

**File**: `examples/comparison/equal.asm`

```asm
PUSH 25
PUSH 25
EQUAL
PRINT
HALT
```

Pushes `1` because `25 == 25` is true.

**Output**: `1`

---

### not_equal.asm

**File**: `examples/comparison/not_equal.asm`

```asm
PUSH 10
PUSH 5
NOT_EQUAL
PRINT
HALT
```

Pushes `1` because `10 != 5` is true.

**Output**: `1`

---

### greater_than.asm

**File**: `examples/comparison/greater_than.asm`

```asm
PUSH 15
PUSH 8
GREATER_THAN
PRINT
HALT
```

Pushes `1` because `15 > 8` is true.

**Output**: `1`

---

### greater_equal.asm

**File**: `examples/comparison/greater_equal.asm`

```asm
PUSH 10
PUSH 10
GREATER_EQUAL
PRINT
HALT
```

Pushes `1` because `10 >= 10` is true.

**Output**: `1`

---

### less_than.asm

**File**: `examples/comparison/less_than.asm`

```asm
PUSH 3
PUSH 9
LESS_THAN
PRINT
HALT
```

Pushes `1` because `3 < 9` is true.

**Output**: `1`

---

### less_equal.asm

**File**: `examples/comparison/less_equal.asm`

```asm
PUSH 4
PUSH 4
LESS_EQUAL
PRINT
HALT
```

Pushes `1` because `4 <= 4` is true.

**Output**: `1`

---

## Control Flow

### jump.asm

**File**: `examples/control_flow/jump.asm`

```asm
JUMP 2

PUSH 999

PUSH 42
PRINT
HALT
```

**Instruction numbering**:
- 0: `JUMP 2`
- 1: `PUSH 999` (skipped)
- 2: `PUSH 42`
- 3: `PRINT`
- 4: `HALT`

`JUMP 2` jumps directly to instruction 2 (`PUSH 42`), skipping instruction 1 (`PUSH 999`) entirely.

**Output**: `42`

---

### jump_if_zero.asm

**File**: `examples/control_flow/jump_if_zero.asm`

```asm
PUSH 0
JUMP_IF_ZERO 4

PUSH 999
PRINT

PUSH 42
PRINT
HALT
```

**Instruction numbering**:
- 0: `PUSH 0`
- 1: `JUMP_IF_ZERO 4`
- 2: `PUSH 999` (skipped)
- 3: `PRINT` (skipped)
- 4: `PUSH 42`
- 5: `PRINT`
- 6: `HALT`

The top of the stack is `0`, so `JUMP_IF_ZERO` takes the branch to instruction 4. Instructions 2 and 3 are skipped.

**Output**: `42`

---

### jump_if_not_zero.asm

**File**: `examples/control_flow/jump_if_not_zero.asm`

```asm
PUSH 5
JUMP_IF_NOT_ZERO 4

PUSH 999
PRINT

PUSH 42
PRINT
HALT
```

**Instruction numbering**:
- 0: `PUSH 5`
- 1: `JUMP_IF_NOT_ZERO 4`
- 2: `PUSH 999` (skipped)
- 3: `PRINT` (skipped)
- 4: `PUSH 42`
- 5: `PRINT`
- 6: `HALT`

The top of the stack is `5` (not zero), so `JUMP_IF_NOT_ZERO` takes the branch to instruction 4.

**Output**: `42`

---

### call_return.asm

**File**: `examples/control_flow/call_return.asm`

```asm
CALL 3
PRINT
HALT

PUSH 99
RETURN
```

**Instruction numbering**:
- 0: `CALL 3`
- 1: `PRINT`
- 2: `HALT`
- 3: `PUSH 99` (function body)
- 4: `RETURN`

**Execution flow**:
1. `CALL 3`: pushes return address onto call stack, jumps to instruction 3.
2. `PUSH 99`: pushes `99` onto operand stack.
3. `RETURN`: pops return address from call stack, jumps back to instruction 1.
4. `PRINT`: pops and prints `99`.
5. `HALT`: done.

**Output**: `99`

---

### skip_instruction.asm

**File**: `examples/control_flow/skip_instruction.asm`

```asm
JUMP 3

PUSH 1
PRINT

PUSH 100
PRINT

HALT
```

**Instruction numbering**:
- 0: `JUMP 3`
- 1: `PUSH 1` (skipped)
- 2: `PRINT` (skipped)
- 3: `PUSH 100`
- 4: `PRINT`
- 5: `HALT`

Jumps over instructions 1 and 2, landing at instruction 3. Only `100` is printed.

**Output**: `100`

---

## Demos

### calculator_demo.asm

**File**: `examples/demos/calculator_demo.asm`

```asm
PUSH 5
PUSH 3
ADD

DUPLICATE

PUSH 2
MULTIPLY

PRINT
PRINT

HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 5` | `[5]` |
| 1 | `PUSH 3` | `[5, 3]` |
| 2 | `ADD` | `[8]` |
| 3 | `DUPLICATE` | `[8, 8]` |
| 4 | `PUSH 2` | `[8, 8, 2]` |
| 5 | `MULTIPLY` | `[8, 16]` — 8 × 2 |
| 6 | `PRINT` | `[8]` — prints `16` |
| 7 | `PRINT` | `[]` — prints `8` |
| 8 | `HALT` | — |

**Output**:

```
16
8
```

First prints `16` (the result of `(5+3)*2`), then prints `8` (the original sum that was duplicated before multiplying).

---

### expression.asm

**File**: `examples/demos/expression.asm`

Computes `(10 + 20) * 3 - 15`:

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

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 10` | `[10]` |
| 1 | `PUSH 20` | `[10, 20]` |
| 2 | `ADD` | `[30]` |
| 3 | `PUSH 3` | `[30, 3]` |
| 4 | `MULTIPLY` | `[90]` |
| 5 | `PUSH 15` | `[90, 15]` |
| 6 | `SUBTRACT` | `[75]` |
| 7 | `PRINT` | `[]` — prints `75` |
| 8 | `HALT` | — |

**Output**: `75`

---

### memory_demo.asm

**File**: `examples/demos/memory_demo.asm`

Stores two values in memory and loads them back:

```asm
PUSH 200
STORE 0

PUSH 300
STORE 1

LOAD 0
PRINT

LOAD 1
PRINT

HALT
```

**Execution**:
1. Store `200` at `memory[0]`.
2. Store `300` at `memory[1]`.
3. Load `memory[0]` → push `200` → print.
4. Load `memory[1]` → push `300` → print.

**Output**:

```
200
300
```

---

### stack_demo.asm

**File**: `examples/demos/stack_demo.asm`

Demonstrates `DUPLICATE` and `SWAP` together:

```asm
PUSH 1
PUSH 2
PUSH 3

DUPLICATE
SWAP

PRINT
PRINT
PRINT
PRINT

HALT
```

**Stack trace**:

| Step | Instruction | Stack After |
|:---:|:---|:---|
| 0 | `PUSH 1` | `[1]` |
| 1 | `PUSH 2` | `[1, 2]` |
| 2 | `PUSH 3` | `[1, 2, 3]` |
| 3 | `DUPLICATE` | `[1, 2, 3, 3]` |
| 4 | `SWAP` | `[1, 2, 3, 3]` → pops `3` and `3`, pushes `3` and `3` → `[1, 2, 3, 3]` |
| 5 | `PRINT` | `[1, 2, 3]` — prints `3` |
| 6 | `PRINT` | `[1, 2]` — prints `3` |
| 7 | `PRINT` | `[1]` — prints `2` |
| 8 | `PRINT` | `[]` — prints `1` |
| 9 | `HALT` | — |

Note: Swapping two identical values (`3` and `3`) has no visible effect.

**Output**:

```
3
3
2
1
```

---

## I/O

### input.asm

**File**: `examples/io/input.asm`

```asm
INPUT
PRINT
HALT
```

Prompts the user for one integer, then prints it back.

**Console** (if user enters `42`):

```
Enter the value : 42
42
```

---

### echo.asm

**File**: `examples/io/echo.asm`

```asm
INPUT
DUPLICATE
PRINT
PRINT
HALT
```

Reads one value from the user, duplicates it, and prints it twice.

**Console** (if user enters `7`):

```
Enter the value : 7
7
7
```

---

## Memory

### store_load.asm

**File**: `examples/memory/store_load.asm`

```asm
PUSH 123
STORE 5
LOAD 5
PRINT
HALT
```

1. Push `123`, store it at `memory[5]`.
2. Load from `memory[5]` (pushes `123`), print it.

**Output**: `123`

---

### overwrite.asm

**File**: `examples/memory/overwrite.asm`

```asm
PUSH 10
STORE 0

PUSH 99
STORE 0

LOAD 0
PRINT

HALT
```

1. Store `10` at `memory[0]`.
2. Store `99` at `memory[0]` (overwrites the `10`).
3. Load `memory[0]` → `99`.

**Output**: `99`

---

## Stack

### duplicate.asm

**File**: `examples/stack/duplicate.asm`

```asm
PUSH 99
DUPLICATE
PRINT
PRINT
HALT
```

Pushes `99`, duplicates it, prints both copies.

**Output**:

```
99
99
```

---

### pop.asm

**File**: `examples/stack/pop.asm`

```asm
PUSH 1
PUSH 2
POP
PRINT
HALT
```

Pushes `1` and `2`, pops (discards) `2`, prints `1`.

**Output**: `1`

---

### swap.asm

**File**: `examples/stack/swap.asm`

```asm
PUSH 10
PUSH 20
SWAP
PRINT
PRINT
HALT
```

Pushes `10` and `20`, swaps them (now `10` is on top), prints `10` then `20`.

**Output**:

```
10
20
```

---

## Unary

### negative.asm

**File**: `examples/unary/negative.asm`

```asm
PUSH 7
NEGATIVE
PRINT
HALT
```

Pushes `7`, negates it to `-7`, prints.

**Output**: `-7`

---

### positive.asm

**File**: `examples/unary/positive.asm`

```asm
PUSH -9
POSITIVE
PRINT
HALT
```

Pushes `-9`, converts to positive `9`, prints.

**Output**: `9`

---

## Running Any Example

To run any example:

```bash
./mylang examples/<category>/<name>.asm
```

Or manually:

```bash
./build/assembler/mylangasm examples/<category>/<name>.asm build/bytecode/bytecode.mylangvm
./build/vm/mylangvm build/bytecode/bytecode.mylangvm
```

For example:

```bash
./mylang examples/demos/expression.asm
```
