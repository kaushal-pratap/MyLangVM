# Instruction Set Reference

Complete reference for all 27 instructions in the MyLangVM instruction set. Each entry includes the assembly mnemonic, the internal VM opcode, byte encoding, operand requirements, stack effects, and precise behavioral semantics.

---

## How to Read This Reference

Each instruction entry follows this format:

- **Assembly mnemonic** — what you write in `.asm` source files (case-insensitive)
- **VM opcode** — the internal numeric value used in bytecode
- **Bytecode size** — how many bytes the instruction occupies in the binary file
- **Stack effect** — the state of the operand stack before and after execution
- **Description** — precise behavioral specification

**Stack notation**: `[a, b]` means `a` was pushed first (bottom) and `b` was pushed second (top). The **top** of the stack is always the rightmost element.

---

## 1 · Stack Operations

### PUSH

| Property | Value |
|:---|:---|
| **Assembly** | `PUSH <value>` |
| **VM opcode** | `PSH` = `0` |
| **Bytecode** | 5 bytes: `[0x00] [val_byte0] [val_byte1] [val_byte2] [val_byte3]` |
| **Operand** | Required — a 32-bit signed integer (−2,147,483,648 to 2,147,483,647) |
| **Stack** | `... → ..., value` |

Pushes an immediate integer value onto the operand stack. The operand is encoded as a 4-byte little-endian signed integer in the bytecode.

---

### POP

| Property | Value |
|:---|:---|
| **Assembly** | `POP` |
| **VM opcode** | `POP` = `25` |
| **Bytecode** | 1 byte: `[0x19]` |
| **Operand** | None |
| **Stack** | `..., value → ...` |

Removes and discards the top element from the operand stack. The value is not used or printed. Triggers a stack underflow error if the stack is empty.

---

### DUPLICATE

| Property | Value |
|:---|:---|
| **Assembly** | `DUPLICATE` |
| **VM opcode** | `DUP` = `5` |
| **Bytecode** | 1 byte: `[0x05]` |
| **Operand** | None |
| **Stack** | `..., value → ..., value, value` |

Reads (peeks at) the top element without removing it, then pushes a copy. After execution, there are two copies of the value on the stack. Triggers a stack underflow error if the stack is empty.

---

### SWAP

| Property | Value |
|:---|:---|
| **Assembly** | `SWAP` |
| **VM opcode** | `SWP` = `6` |
| **Bytecode** | 1 byte: `[0x06]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., b, a` |

Pops the top two elements and pushes them back in reversed order. The element that was on top is now second, and vice versa. Requires at least 2 elements on the stack.

---

## 2 · Arithmetic Operations

All arithmetic instructions pop two values from the operand stack. The **first pop** yields the right-hand operand, and the **second pop** yields the left-hand operand. The result is pushed back onto the stack.

### ADD

| Property | Value |
|:---|:---|
| **Assembly** | `ADD` |
| **VM opcode** | `ADD` = `1` |
| **Bytecode** | 1 byte: `[0x01]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a + b)` |

Pops `b` (top), pops `a` (second), pushes `a + b`.

---

### SUBTRACT

| Property | Value |
|:---|:---|
| **Assembly** | `SUBTRACT` |
| **VM opcode** | `SUB` = `2` |
| **Bytecode** | 1 byte: `[0x02]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a − b)` |

Pops `b` (top), pops `a` (second), pushes `a − b`.

---

### MULTIPLY

| Property | Value |
|:---|:---|
| **Assembly** | `MULTIPLY` |
| **VM opcode** | `MUL` = `3` |
| **Bytecode** | 1 byte: `[0x03]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a × b)` |

Pops `b` (top), pops `a` (second), pushes `a × b`.

---

### DIVIDE

| Property | Value |
|:---|:---|
| **Assembly** | `DIVIDE` |
| **VM opcode** | `DIV` = `4` |
| **Bytecode** | 1 byte: `[0x04]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a / b)` |

Pops `b` (top), pops `a` (second), pushes `a / b` (integer division, truncated toward zero).

**Division by zero**: If `b` is `0`, the VM pushes both values back onto the stack, prints `Division by zero !!` to stderr, and terminates immediately.

---

### MODULO

| Property | Value |
|:---|:---|
| **Assembly** | `MODULO` |
| **VM opcode** | `MOD` = `7` |
| **Bytecode** | 1 byte: `[0x07]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a % b)` |

Pops `b` (top), pops `a` (second), pushes `a % b` (remainder of integer division).

**Modulo by zero**: If `b` is `0`, the VM pushes both values back onto the stack, prints `Modulo by zero !!` to stderr, and terminates immediately.

---

## 3 · Unary Operations

### NEGATIVE

| Property | Value |
|:---|:---|
| **Assembly** | `NEGATIVE` |
| **VM opcode** | `NEG` = `8` |
| **Bytecode** | 1 byte: `[0x08]` |
| **Operand** | None |
| **Stack** | `..., value → ..., (−value)` (only if `value > 0`) |

If the top element is **positive** (strictly greater than 0), it is popped, negated, and the result is pushed. If the top element is zero or already negative, the stack is left unchanged.

---

### POSITIVE

| Property | Value |
|:---|:---|
| **Assembly** | `POSITIVE` |
| **VM opcode** | `POS` = `9` |
| **Bytecode** | 1 byte: `[0x09]` |
| **Operand** | None |
| **Stack** | `..., value → ..., abs(value)` (only if `value < 0`) |

If the top element is **negative** (strictly less than 0), it is popped, negated (making it positive), and the result is pushed. If the top element is zero or already positive, the stack is left unchanged.

---

## 4 · Comparison Operations

All comparison instructions pop two values from the operand stack, compare them, and push the result as an integer: **`1`** for true, **`0`** for false.

As with arithmetic, the **first pop** yields the right-hand operand and the **second pop** yields the left-hand operand. The comparison is `left OP right`.

### GREATER_THAN

| Property | Value |
|:---|:---|
| **Assembly** | `GREATER_THAN` |
| **VM opcode** | `GT` = `10` |
| **Bytecode** | 1 byte: `[0x0A]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a > b ? 1 : 0)` |

---

### GREATER_EQUAL

| Property | Value |
|:---|:---|
| **Assembly** | `GREATER_EQUAL` |
| **VM opcode** | `GE` = `11` |
| **Bytecode** | 1 byte: `[0x0B]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a >= b ? 1 : 0)` |

---

### EQUAL

| Property | Value |
|:---|:---|
| **Assembly** | `EQUAL` |
| **VM opcode** | `EQ` = `12` |
| **Bytecode** | 1 byte: `[0x0C]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a == b ? 1 : 0)` |

---

### NOT_EQUAL

| Property | Value |
|:---|:---|
| **Assembly** | `NOT_EQUAL` |
| **VM opcode** | `NE` = `13` |
| **Bytecode** | 1 byte: `[0x0D]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a != b ? 1 : 0)` |

---

### LESS_THAN

| Property | Value |
|:---|:---|
| **Assembly** | `LESS_THAN` |
| **VM opcode** | `LT` = `14` |
| **Bytecode** | 1 byte: `[0x0E]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a < b ? 1 : 0)` |

---

### LESS_EQUAL

| Property | Value |
|:---|:---|
| **Assembly** | `LESS_EQUAL` |
| **VM opcode** | `LE` = `15` |
| **Bytecode** | 1 byte: `[0x0F]` |
| **Operand** | None |
| **Stack** | `..., a, b → ..., (a <= b ? 1 : 0)` |

---

## 5 · Control Flow

### JUMP

| Property | Value |
|:---|:---|
| **Assembly** | `JUMP <target>` |
| **VM opcode** | `JMP` = `16` |
| **Bytecode** | 5 bytes: `[0x10] [target_byte0] [target_byte1] [target_byte2] [target_byte3]` |
| **Operand** | Required — logical instruction index (0-based) |
| **Stack** | No change |

Unconditionally sets the program counter to the byte position of logical instruction `<target>`. The target is resolved through the instruction map array built during VM initialization. If the target is out of bounds (greater than or equal to the total instruction count), the VM prints an error and halts.

---

### JUMP_IF_ZERO

| Property | Value |
|:---|:---|
| **Assembly** | `JUMP_IF_ZERO <target>` |
| **VM opcode** | `JZ` = `17` |
| **Bytecode** | 5 bytes: `[0x11] [target_byte0] [target_byte1] [target_byte2] [target_byte3]` |
| **Operand** | Required — logical instruction index (0-based) |
| **Stack** | `..., condition → ...` |

Peeks at the top of the stack. If the value equals `0`, jumps to logical instruction `<target>`. If the value is not `0`, execution continues to the next instruction. **In both cases**, the top value is popped (consumed).

---

### JUMP_IF_NOT_ZERO

| Property | Value |
|:---|:---|
| **Assembly** | `JUMP_IF_NOT_ZERO <target>` |
| **VM opcode** | `JNZ` = `18` |
| **Bytecode** | 5 bytes: `[0x12] [target_byte0] [target_byte1] [target_byte2] [target_byte3]` |
| **Operand** | Required — logical instruction index (0-based) |
| **Stack** | `..., condition → ...` |

Peeks at the top of the stack. If the value is **not** `0`, jumps to logical instruction `<target>`. If the value is `0`, execution continues to the next instruction. **In both cases**, the top value is popped (consumed).

---

### CALL

| Property | Value |
|:---|:---|
| **Assembly** | `CALL <target>` |
| **VM opcode** | `CALL` = `19` |
| **Bytecode** | 5 bytes: `[0x13] [target_byte0] [target_byte1] [target_byte2] [target_byte3]` |
| **Operand** | Required — logical instruction index (0-based) |
| **Stack** | No change to operand stack. Pushes return address to **call stack**. |

Pushes the return address (`pc + 2`, which is the byte position of the instruction immediately following the `CALL`) onto the **call stack**, then jumps to logical instruction `<target>` (same behavior as `JUMP` for the jump itself).

---

### RETURN

| Property | Value |
|:---|:---|
| **Assembly** | `RETURN` |
| **VM opcode** | `RET` = `20` |
| **Bytecode** | 1 byte: `[0x14]` |
| **Operand** | None |
| **Stack** | No change to operand stack. Pops return address from **call stack**. |

Pops the return address from the **call stack** and sets the program counter to that address. Execution continues from the instruction at the returned-to address. If the call stack is empty, this causes a stack underflow error.

---

## 6 · Memory Operations

### STORE

| Property | Value |
|:---|:---|
| **Assembly** | `STORE <index>` |
| **VM opcode** | `STORE` = `21` |
| **Bytecode** | 5 bytes: `[0x15] [index_byte0] [index_byte1] [index_byte2] [index_byte3]` |
| **Operand** | Required — memory index (0 to 1023) |
| **Stack** | `..., value → ...` |

Pops the top value from the operand stack and writes it to `memory[index]`. If the index is out of range (negative or ≥ 1024), the VM prints `Invalid memory access !!` and halts.

---

### LOAD

| Property | Value |
|:---|:---|
| **Assembly** | `LOAD <index>` |
| **VM opcode** | `LOAD` = `22` |
| **Bytecode** | 5 bytes: `[0x16] [index_byte0] [index_byte1] [index_byte2] [index_byte3]` |
| **Operand** | Required — memory index (0 to 1023) |
| **Stack** | `... → ..., value` |

Reads the value at `memory[index]` and pushes a copy onto the operand stack. The memory cell is not modified. If the index is out of range, the VM prints an error and halts.

---

## 7 · I/O Operations

### INPUT

| Property | Value |
|:---|:---|
| **Assembly** | `INPUT` |
| **VM opcode** | `INPT` = `23` |
| **Bytecode** | 1 byte: `[0x17]` |
| **Operand** | None |
| **Stack** | `... → ..., input_value` |

Prints the prompt `Enter the value : ` to stdout, reads a 32-bit signed integer from stdin (via `scanf`), and pushes the value onto the operand stack.

---

### PRINT

| Property | Value |
|:---|:---|
| **Assembly** | `PRINT` |
| **VM opcode** | `PRNT` = `24` |
| **Bytecode** | 1 byte: `[0x18]` |
| **Operand** | None |
| **Stack** | `..., value → ...` |

Pops the top value from the operand stack and prints it to stdout as a decimal integer, followed by a newline character.

---

## 8 · Program Control

### HALT

| Property | Value |
|:---|:---|
| **Assembly** | `HALT` |
| **VM opcode** | `HLT` = `26` |
| **Bytecode** | 1 byte: `[0x1A]` |
| **Operand** | None |
| **Stack** | No change |

Sets the VM's running flag to `false`, causing the execution loop to terminate. Every program must include a `HALT` instruction to terminate cleanly. If the VM reaches the end of the bytecode stream without encountering `HALT`, it prints `Program terminated unexpectedly: missing HLT instruction?` and stops.

---

## Opcode Summary Table

| Opcode | Value | Assembly Mnemonic | Size (bytes) | Has Operand |
|:---|:---:|:---|:---:|:---:|
| `PSH` | 0 | `PUSH` | 5 | Yes |
| `ADD` | 1 | `ADD` | 1 | No |
| `SUB` | 2 | `SUBTRACT` | 1 | No |
| `MUL` | 3 | `MULTIPLY` | 1 | No |
| `DIV` | 4 | `DIVIDE` | 1 | No |
| `DUP` | 5 | `DUPLICATE` | 1 | No |
| `SWP` | 6 | `SWAP` | 1 | No |
| `MOD` | 7 | `MODULO` | 1 | No |
| `NEG` | 8 | `NEGATIVE` | 1 | No |
| `POS` | 9 | `POSITIVE` | 1 | No |
| `GT` | 10 | `GREATER_THAN` | 1 | No |
| `GE` | 11 | `GREATER_EQUAL` | 1 | No |
| `EQ` | 12 | `EQUAL` | 1 | No |
| `NE` | 13 | `NOT_EQUAL` | 1 | No |
| `LT` | 14 | `LESS_THAN` | 1 | No |
| `LE` | 15 | `LESS_EQUAL` | 1 | No |
| `JMP` | 16 | `JUMP` | 5 | Yes |
| `JZ` | 17 | `JUMP_IF_ZERO` | 5 | Yes |
| `JNZ` | 18 | `JUMP_IF_NOT_ZERO` | 5 | Yes |
| `CALL` | 19 | `CALL` | 5 | Yes |
| `RET` | 20 | `RETURN` | 1 | No |
| `STORE` | 21 | `STORE` | 5 | Yes |
| `LOAD` | 22 | `LOAD` | 5 | Yes |
| `INPT` | 23 | `INPUT` | 1 | No |
| `PRNT` | 24 | `PRINT` | 1 | No |
| `POP` | 25 | `POP` | 1 | No |
| `HLT` | 26 | `HALT` | 1 | No |

---

## Instructions Sorted by Category

| Category | Instructions |
|:---|:---|
| **Stack** | `PUSH`, `POP`, `DUPLICATE`, `SWAP` |
| **Arithmetic** | `ADD`, `SUBTRACT`, `MULTIPLY`, `DIVIDE`, `MODULO` |
| **Unary** | `NEGATIVE`, `POSITIVE` |
| **Comparison** | `GREATER_THAN`, `GREATER_EQUAL`, `EQUAL`, `NOT_EQUAL`, `LESS_THAN`, `LESS_EQUAL` |
| **Control flow** | `JUMP`, `JUMP_IF_ZERO`, `JUMP_IF_NOT_ZERO`, `CALL`, `RETURN` |
| **Memory** | `STORE`, `LOAD` |
| **I/O** | `INPUT`, `PRINT` |
| **Program control** | `HALT` |

---

## Instructions with Operands

These 7 instructions require an operand (an integer argument):

| Instruction | Operand meaning |
|:---|:---|
| `PUSH <value>` | The value to push onto the stack |
| `JUMP <target>` | Logical instruction index to jump to |
| `JUMP_IF_ZERO <target>` | Logical instruction index to jump to |
| `JUMP_IF_NOT_ZERO <target>` | Logical instruction index to jump to |
| `CALL <target>` | Logical instruction index to call |
| `STORE <index>` | Memory address to write to (0–1023) |
| `LOAD <index>` | Memory address to read from (0–1023) |

The remaining 20 instructions take no operand.

---

## Instructions that Modify the Program Counter

These 5 instructions set the program counter directly (the VM does **not** auto-increment `pc` after executing them):

- `JUMP`
- `JUMP_IF_ZERO`
- `JUMP_IF_NOT_ZERO`
- `CALL`
- `RETURN`

All other instructions allow the VM to auto-increment `pc` after execution.
