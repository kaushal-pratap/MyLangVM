# Virtual Machine Internals

This document explains how the MyLangVM virtual machine (`mylangvm`) works internally. It traces the path from loading a bytecode file to executing every instruction, covering every component and data structure along the way.

---

## Table of Contents

1. [Overview](#overview)
2. [Entry Point: main.c](#entry-point-mainc)
3. [The Bytecode Loader](#the-bytecode-loader)
   - [ProgramMemory Structure](#programmemory-structure)
   - [Loading Algorithm](#loading-algorithm)
   - [Little-Endian Decoding](#little-endian-decoding)
4. [VM State: The VM Structure](#vm-state-the-vm-structure)
5. [The Stack](#the-stack)
   - [Stack Structure](#stack-structure)
   - [Stack Operations](#stack-operations)
6. [VM Initialization](#vm-initialization)
   - [Building the Instruction Map](#building-the-instruction-map)
7. [The Execution Loop](#the-execution-loop)
   - [vm_run and vm_step](#vm_run-and-vm_step)
   - [Fetching](#fetching)
   - [Dispatching](#dispatching)
   - [PC Advancement](#pc-advancement)
8. [Instruction Handlers](#instruction-handlers)
   - [Stack Operations](#handler-stack-operations)
   - [Arithmetic Operations](#handler-arithmetic-operations)
   - [Unary Operations](#handler-unary-operations)
   - [Comparison Operations](#handler-comparison-operations)
   - [Control Flow Operations](#handler-control-flow-operations)
   - [Memory Operations](#handler-memory-operations)
   - [I/O Operations](#handler-io-operations)
   - [Program Control](#handler-program-control)
9. [Instruction Metadata](#instruction-metadata)
10. [VM Lifecycle](#vm-lifecycle)
11. [Source File Map](#source-file-map)

---

## Overview

The VM is a **fetch-execute** engine. It loads a binary bytecode file into memory, builds an internal mapping of instruction positions, then enters a loop that repeatedly fetches the next instruction, executes it, and advances the program counter.

```
                     mylangvm lifecycle
                     ─────────────────

   1. Load bytecode file into ProgramMemory
   2. Initialize VM state (stacks, memory, instruction map)
   3. Execute: fetch → dispatch → execute → advance PC
   4. Repeat until HALT or error
   5. Destroy VM (free instruction map)
```

---

## Entry Point: main.c

The VM's `main()` function is minimal:

1. **Argument check**: Expects exactly one argument — the path to a `.mylangvm` bytecode file.
2. **Load**: Creates a `ProgramMemory` struct (zero-initialized), calls `load_program()` to read the bytecode file into it.
3. **Initialize**: Creates a `VM` struct, calls `vm_init()` to set up the runtime state.
4. **Run**: Calls `vm_run()` to execute the program.
5. **Cleanup**: Calls `vm_destroy()` to free allocated memory.

---

## The Bytecode Loader

**Source files**: `loader.c`, `loader.h`

The loader reads a binary `.mylangvm` file and produces a flat array of `int32_t` values that the VM can index directly.

### ProgramMemory Structure

```c
typedef struct ProgramMemory {
    int32_t program[MAX_PROGRAM_SIZE];  // The bytecode stream (opcodes + operands)
    size_t program_size;                 // Number of valid int32_t entries
} ProgramMemory;
```

`MAX_PROGRAM_SIZE` is 1024. Each element is an `int32_t` — opcodes (which are small numbers 0–26) are stored in `int32_t` slots, and operands (which are full 32-bit values) occupy their own slots.

### Loading Algorithm

The loader reads the bytecode byte-by-byte:

1. **Open** the file in binary read mode (`"rb"`).
2. **Loop**: Read one byte at a time. Each byte is an opcode.
   a. Store the opcode as an `int32_t` in the program array.
   b. Check if this opcode's instruction size is 2 (meaning it has an operand). If yes:
      - Read the next 4 bytes as a little-endian `int32_t` operand.
      - Store the operand in the next slot of the program array.
   c. Bounds-check on every insertion to prevent overflowing `MAX_PROGRAM_SIZE`.
3. Check for read errors with `ferror()`.
4. **Close** the file.

After loading, the `program[]` array mirrors the bytecode stream but expanded into `int32_t` slots:

```
Bytecode file:  [0x00] [0x05 0x00 0x00 0x00] [0x01] [0x18] [0x1A]
                 PSH         5 (LE)            ADD    PRNT   HLT

program[] array: [0, 5, 1, 24, 26]
                  PSH 5  ADD PRNT HLT
program_size = 5
```

### Little-Endian Decoding

The `read_int32_le()` function reads 4 bytes and reconstructs a 32-bit integer:

```c
static bool read_int32_le(FILE *file, int32_t *value) {
    uint8_t bytes[4];
    if (fread(bytes, sizeof(uint8_t), 4, file) != 4) return false;

    uint32_t bits =
        ((uint32_t)bytes[0])       |
        ((uint32_t)bytes[1] << 8)  |
        ((uint32_t)bytes[2] << 16) |
        ((uint32_t)bytes[3] << 24);

    *value = (int32_t)bits;
    return true;
}
```

This is the inverse of the assembler's `write_int32_le()`. Both use explicit bit-shifting rather than relying on the host's native byte order, ensuring portability.

---

## VM State: The VM Structure

**Source file**: `vm.h`

The `VM` struct holds the entire runtime state:

```c
typedef struct {
    Stack operandStack;            // Data stack for computation
    Stack callStack;               // Return-address stack for CALL/RETURN
    int32_t memory[MEMORY_SIZE];   // 1024-word RAM
    size_t pc;                     // Program counter (byte index into program[])
    bool running;                  // Execution loop control flag
    size_t programLength;          // Total elements in the program[] array
    const int32_t *program;        // Pointer to the loaded bytecode (not owned)
    size_t instructionCount;       // Number of logical instructions
    size_t *instructionMapArray;   // Logical index → byte position mapping
} VM;
```

Key points:

- **`program`** is a pointer to the `ProgramMemory.program` array — the VM does **not** copy the bytecode; it reads from the loader's buffer directly.
- **`pc`** is an index into the `program[]` array, not a byte offset into the file.
- **`instructionMapArray`** is dynamically allocated during initialization and freed during destruction.
- **`MEMORY_SIZE`** is 1024.

---

## The Stack

**Source files**: `stack.c`, `stack.h`

### Stack Structure

```c
#define MAX_STACK_SIZE 100

typedef struct {
    int32_t arr[MAX_STACK_SIZE];  // Fixed-size array of values
    int top;                      // Index of the top element (-1 = empty)
} Stack;
```

The stack is a static array with an integer top pointer. `top == -1` means empty. `top == MAX_STACK_SIZE - 1` means full.

### Stack Operations

| Function | Behavior |
|:---|:---|
| `stack_init(stack)` | Sets `top = -1` (empty) |
| `stack_is_empty(stack)` | Returns `true` if `top == -1` |
| `stack_is_full(stack)` | Returns `true` if `top >= MAX_STACK_SIZE - 1` |
| `stack_push(stack, value)` | Increments `top`, stores `value`. **Overflow**: prints `Stack Overflow !` and exits. |
| `stack_pop(stack)` | Reads `arr[top]`, decrements `top`, returns the value. **Underflow**: prints `Stack Underflow !` and exits. |
| `stack_peek(stack)` | Returns `arr[top]` without modifying `top`. **Underflow**: prints error and exits. |

Both the operand stack and the call stack use the same `Stack` type and operations. They are two independent instances within the VM struct.

---

## VM Initialization

**Source file**: `vm.c` — `vm_init()`

The `vm_init()` function sets up the VM for execution:

1. Set `pc = 0` (start at the beginning).
2. Set `running = true`.
3. Store the program pointer and length.
4. Initialize both stacks with `stack_init()`.
5. Call `build_instruction_map()` to construct the instruction mapping.

### Building the Instruction Map

This is one of the most critical setup steps. The instruction map solves the problem of **variable-length instructions**.

**The problem**: In the `program[]` array, a `PUSH 5` occupies **2 slots** (opcode + operand), but an `ADD` occupies **1 slot** (opcode only). If a user writes `JUMP 3`, meaning "jump to the 4th instruction", we can't just set `pc = 3` — because index 3 in `program[]` might be in the middle of a `PUSH` operand.

**The solution**: `build_instruction_map()` scans the entire `program[]` array in two passes:

**Pass 1 — Count instructions**:
```
i = 0
while i < programLength:
    instructionCount++
    i += instruction_size(program[i])
```

This determines how many logical instructions there are.

**Pass 2 — Build the map**:
```
Allocate instructionMapArray[instructionCount]
instruction_index = 0
program_index = 0
while program_index < programLength:
    instructionMapArray[instruction_index] = program_index
    program_index += instruction_size(program[program_index])
    instruction_index++
```

**Example**: For the program `PUSH 5, PUSH 3, ADD, PRINT, HALT`:

```
program[] = [PSH, 5, PSH, 3, ADD, PRNT, HLT]
             [0]  [1] [2]  [3] [4]  [5]   [6]

instructionMapArray:
  [0] → 0   (instruction 0 = PSH at index 0)
  [1] → 2   (instruction 1 = PSH at index 2)
  [2] → 4   (instruction 2 = ADD at index 4)
  [3] → 5   (instruction 3 = PRNT at index 5)
  [4] → 6   (instruction 4 = HLT at index 6)
```

When `JUMP 3` executes, the VM looks up `instructionMapArray[3]` to get `5`, and sets `pc = 5`, which points to the `PRNT` instruction.

---

## The Execution Loop

**Source file**: `vm.c`

### vm_run and vm_step

The execution loop is split into two functions:

```c
void vm_run(VM *vm) {
    while (vm->running) {
        vm_step(vm);
    }
}
```

`vm_run()` is the outer loop. `vm_step()` processes a single instruction:

```c
void vm_step(VM *vm) {
    // 1. Bounds check
    if (vm->pc >= vm->programLength) {
        fprintf(stderr, "Program terminated unexpectedly: missing HLT instruction?\n");
        vm->running = false;
        return;
    }

    // 2. Fetch
    VmInstruction instr = vm_fetch_instruction(vm);

    // 3. Execute (dispatch)
    vm_execute_instruction(vm, instr);

    // 4. Advance PC (only if the instruction didn't modify it)
    if (!is_pc_modified(instr)) {
        vm->pc++;
    }
}
```

### Fetching

```c
VmInstruction vm_fetch_instruction(VM *vm) {
    return (VmInstruction)vm->program[vm->pc];
}
```

Simply reads the current `program[]` element and casts it to a `VmInstruction` enum value.

### Dispatching

`vm_execute_instruction()` uses a `switch` statement over all 27 opcode values to call the appropriate handler function:

```c
switch (instr) {
    case HLT:   execute_hlt(vm);   break;
    case PSH:   execute_psh(vm);   break;
    case ADD:   execute_add(vm);   break;
    // ... all 27 cases
    default:
        fprintf(stderr, "Unknown instruction: %d\n", instr);
        vm->running = false;
        break;
}
```

### PC Advancement

After execution, the VM checks `is_pc_modified(instr)`. The following instructions modify `pc` themselves and should **not** be auto-incremented:

- `JMP` — sets `pc` to the target
- `JZ` — either jumps or advances by 2 (to skip the operand)
- `JNZ` — either jumps or advances by 2
- `CALL` — pushes return address, then jumps
- `RET` — pops return address, sets `pc`

All other instructions need `pc++` after execution.

Note that for instructions with operands (like `PSH`), the handler itself increments `pc` to read the operand (`vm->program[++vm->pc]`), so the auto-increment of `pc` by 1 at the end of `vm_step()` moves past the operand correctly.

---

## Instruction Handlers

**Source files**: `instruction_handlers.c`, `instruction_handlers.h`

Each of the 27 instructions has a dedicated handler function.

### Handler: Stack Operations

**`execute_psh(VM *vm)`**
Increments `pc` to read the operand, pushes it: `stack_push(&vm->operandStack, vm->program[++vm->pc])`.

**`execute_pop(VM *vm)`**
Calls `stack_pop(&vm->operandStack)` and discards the result.

**`execute_dup(VM *vm)`**
Peeks at the top, then pushes a copy: `stack_push(&vm->operandStack, stack_peek(&vm->operandStack))`.

**`execute_swp(VM *vm)`**
Pops two values, pushes them back in reverse order.

### Handler: Arithmetic Operations

All follow the same pattern:

```c
int32_t rhs = stack_pop(&vm->operandStack);
int32_t lhs = stack_pop(&vm->operandStack);
stack_push(&vm->operandStack, lhs OP rhs);
```

- **`execute_add`**: `lhs + rhs`
- **`execute_sub`**: `lhs - rhs`
- **`execute_mul`**: `lhs * rhs`
- **`execute_div`**: `lhs / rhs` (with zero-check — pushes both values back and exits on div-by-zero)
- **`execute_mod`**: `lhs % rhs` (with zero-check)

### Handler: Unary Operations

**`execute_neg(VM *vm)`**
If `stack_peek() > 0`: pop, negate, push. Otherwise: no-op.

**`execute_pos(VM *vm)`**
If `stack_peek() < 0`: pop, negate (make positive), push. Otherwise: no-op.

### Handler: Comparison Operations

All follow the same pattern:

```c
int32_t rhs = stack_pop(&vm->operandStack);
int32_t lhs = stack_pop(&vm->operandStack);
stack_push(&vm->operandStack, (lhs OP rhs) ? 1 : 0);
```

- **`execute_gt`**: `lhs > rhs`
- **`execute_ge`**: `lhs >= rhs`
- **`execute_eq`**: `lhs == rhs`
- **`execute_ne`**: `lhs != rhs`
- **`execute_lt`**: `lhs < rhs`
- **`execute_le`**: `lhs <= rhs`

### Handler: Control Flow Operations

**`execute_jmp(VM *vm)`**
1. Increment `pc` to read the target operand.
2. Read `target = vm->program[vm->pc]`.
3. Bounds-check: if `target >= instructionCount`, error and halt.
4. Set `pc = instructionMapArray[target]`.

**`execute_jz(VM *vm)`**
1. Peek at the top of the operand stack.
2. If `== 0`: increment `pc`, read target, set `pc` to `instructionMapArray[target]`.
3. If `!= 0`: advance `pc` by 2 (skip past the operand).
4. In both cases, pop the condition value.

**`execute_jnz(VM *vm)`**
Same as `execute_jz` but the condition is reversed: jumps if `!= 0`, falls through if `== 0`.

**`execute_call(VM *vm)`**
1. Push `pc + 2` (the instruction after the `CALL` and its operand) onto the **call stack**.
2. Delegate to `execute_jmp()` to perform the actual jump.

**`execute_ret(VM *vm)`**
Pop the return address from the **call stack** and set `pc` to that address.

### Handler: Memory Operations

**`execute_store(VM *vm)`**
1. Increment `pc` to read the memory index.
2. Bounds-check: index must be ≥ 0 and < `MEMORY_SIZE` (1024).
3. Pop the top of the operand stack and write it to `vm->memory[index]`.

**`execute_load(VM *vm)`**
1. Increment `pc` to read the memory index.
2. Bounds-check.
3. Push `vm->memory[index]` onto the operand stack.

### Handler: I/O Operations

**`execute_inpt(VM *vm)`**
Prints `Enter the value : `, reads an `int32_t` from stdin using `scanf("%" SCNd32, &user_input)` (using the portable `<inttypes.h>` format macro), and pushes the value.

**`execute_prnt(VM *vm)`**
Pops the top value and prints it using `printf("%" PRId32 "\n", value)` (using the portable `<inttypes.h>` format macro).

### Handler: Program Control

**`execute_hlt(VM *vm)`**
Sets `vm->running = false`, causing the execution loop to terminate on the next iteration.

---

## Instruction Metadata

**Source file**: `instruction.c`

Two helper functions provide metadata about instructions:

### instruction_size()

Returns the number of `program[]` slots an instruction occupies:

| Returns 2 | Returns 1 (default) |
|:---|:---|
| `PSH`, `JMP`, `JZ`, `JNZ`, `CALL`, `STORE`, `LOAD` | All other instructions |

This is used by the loader (to know when to read an operand) and by `build_instruction_map()` (to advance through the program array).

### is_pc_modified()

Returns `true` for instructions that set `pc` themselves:

| Returns `true` | Returns `false` (default) |
|:---|:---|
| `JMP`, `JZ`, `JNZ`, `CALL`, `RET` | All other instructions |

This is used by `vm_step()` to decide whether to auto-increment `pc`.

---

## VM Lifecycle

The complete lifecycle of a VM instance:

```
1. vm_init()
   ├── pc = 0, running = true
   ├── stack_init(operandStack)
   ├── stack_init(callStack)
   └── build_instruction_map()
        ├── Count logical instructions
        ├── malloc instruction map array
        └── Populate map: logical index → program[] index

2. vm_run()
   └── while (running):
        └── vm_step()
             ├── Bounds check pc
             ├── Fetch instruction at program[pc]
             ├── Dispatch to handler
             └── Advance pc (if instruction didn't modify it)

3. vm_destroy()
   ├── free(instructionMapArray)
   └── Reset instructionCount = 0
```

---

## Source File Map

| File | Role |
|:---|:---|
| `vm/src/main.c` | Entry point — load, init, run, destroy |
| `vm/src/loader.c` | Reads `.mylangvm` binary file into `ProgramMemory` |
| `vm/src/vm.c` | VM lifecycle: init, fetch, dispatch, execute loop, destroy |
| `vm/src/stack.c` | Stack data structure: push, pop, peek |
| `vm/src/instruction_handlers.c` | All 27 instruction handler implementations |
| `vm/src/instruction.c` | `instruction_size()` and `is_pc_modified()` queries |
| `vm/include/vm.h` | VM struct definition and lifecycle function prototypes |
| `vm/include/stack.h` | Stack struct and operation prototypes |
| `vm/include/instruction_handlers.h` | Handler function prototypes |
| `vm/include/loader.h` | `ProgramMemory` struct and `load_program()` prototype |
| `common/instruction.h` | Shared — `VmInstruction` enum, `IRInstruction`, `Program` |
