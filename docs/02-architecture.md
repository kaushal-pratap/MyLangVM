# Architecture

This document is a deep dive into the design and architecture of MyLangVM. It explains why the system is built the way it is, how the pieces fit together, and the reasoning behind each design decision.

---

## Table of Contents

1. [High-Level Overview](#high-level-overview)
2. [Core Concepts](#core-concepts)
   - [What is a Language Virtual Machine?](#what-is-a-language-virtual-machine)
   - [What is Bytecode?](#what-is-bytecode)
   - [Stack-Based vs. Register-Based VMs](#stack-based-vs-register-based-vms)
3. [The Two-Program Architecture](#the-two-program-architecture)
4. [The Assembler Pipeline](#the-assembler-pipeline)
5. [The VM Runtime](#the-vm-runtime)
   - [Dual-Stack Model](#dual-stack-model)
   - [Memory Model](#memory-model)
   - [Instruction Mapping](#instruction-mapping)
   - [The Fetch-Execute Cycle](#the-fetch-execute-cycle)
6. [Data Representation](#data-representation)
7. [Portability Design](#portability-design)

---

## High-Level Overview

MyLangVM is a two-program system:

```
┌───────────────────────────────────────────────────────────────────────┐
│                          MyLangVM System                              │
│                                                                       │
│   ┌─────────────────────┐         ┌─────────────────────┐            │
│   │      ASSEMBLER       │         │    VIRTUAL MACHINE   │            │
│   │     (mylangasm)      │         │     (mylangvm)       │            │
│   │                      │         │                      │            │
│   │  .asm ──▶ tokenizer  │         │  .mylangvm ──▶ loader│            │
│   │       ──▶ parser     │  ─────▶ │            ──▶ init  │            │
│   │       ──▶ serializer │bytecode │            ──▶ run   │            │
│   │       ──▶ .mylangvm  │  file   │            ──▶ output│            │
│   └─────────────────────┘         └─────────────────────┘            │
└───────────────────────────────────────────────────────────────────────┘
```

The **assembler** is a compiler. It reads text, understands the language, and emits binary. The **VM** is an execution engine. It reads binary and runs it. They share a common definition of what the instructions mean (via `common/instruction.h`), but they are otherwise completely independent programs.

---

## Core Concepts

### What is a Language Virtual Machine?

A **Language Virtual Machine** (also called a *Process VM*) is a software program that emulates a computer. Unlike system virtual machines (like VirtualBox or QEMU) which emulate real hardware to run guest operating systems, a language VM defines its own custom instruction set and executes programs written for that instruction set.

Real-world examples:

| VM | Language | Architecture |
|:---|:---|:---|
| **JVM** | Java, Kotlin, Scala | Stack-based |
| **CPython** | Python | Stack-based |
| **BEAM** | Erlang, Elixir | Register-based |
| **Lua VM** | Lua | Register-based |
| **CLR** | C#, F# | Stack-based |

MyLangVM is in this family. It defines its own 27-instruction ISA and executes programs compiled to that ISA.

### What is Bytecode?

**Bytecode** is a compact, binary representation of a program. It sits between human-readable source code and hardware-native machine code:

```
Source Code (.asm)       Bytecode (.mylangvm)         Machine Code
   PUSH 42         →      [0x00] [0x2A 0x00 0x00 0x00]     (CPU-specific)
   PRINT           →      [0x18]
   HALT            →      [0x1A]
```

The advantage of bytecode is **portability**: the same `.mylangvm` file can run on any machine that has the MyLangVM virtual machine, regardless of the CPU architecture.

### Stack-Based vs. Register-Based VMs

Virtual machines generally fall into two categories:

**Stack-based** (MyLangVM's approach):
- All values live on a stack
- Instructions implicitly operate on the top of the stack
- Adding two numbers: `PUSH 5`, `PUSH 3`, `ADD` — the `ADD` knows to pop two values and push the result
- Simpler to implement, simpler to compile for

**Register-based**:
- Values live in named virtual registers (R0, R1, R2, ...)
- Instructions explicitly name their operands
- Adding two numbers: `ADD R0, R1, R2` — add R1 and R2, store in R0
- Typically fewer instructions needed, but each instruction is larger

| Aspect | Stack-Based | Register-Based |
|:---|:---|:---|
| Instruction encoding | Compact (no register fields) | Larger (register indices in each instruction) |
| Compiler backend | Simple — just push/pop in order | Complex — requires register allocation |
| Code density | Higher (more instructions, but each is small) | Lower (fewer instructions, but each is larger) |
| Example VMs | JVM, CPython, CLR, **MyLangVM** | Lua VM, Dalvik (Android) |

MyLangVM uses a stack-based design because it is clean, modular, and ideal for learning how virtual machines work.

---

## The Two-Program Architecture

The system is deliberately split into two separate programs:

### Why Two Programs?

1. **Separation of concerns**: The assembler knows about text processing, parsing, and syntax. The VM knows about execution, stacks, and memory. Neither needs to know about the other's internals.

2. **Offline compilation**: You assemble once and run many times. The bytecode file is the artifact.

3. **Binary interface**: The `.mylangvm` file format is the contract between the two programs. As long as both agree on the format, they can evolve independently.

### The Shared Contract: `common/instruction.h`

The `common/` directory contains the one piece of shared knowledge: the instruction definitions. Both the assembler and the VM include this header. It defines:

- **`VmInstruction` enum** — the numeric opcodes (0–26) that appear in bytecode
- **`IRInstruction` struct** — the assembler's intermediate representation (opcode + optional operand)
- **`Program` struct** — an array of IR instructions with a count
- **Helper function signatures** — `instruction_size()` and `is_pc_modified()`

This ensures both programs agree on what opcode `0` means (`PSH`), what opcode `26` means (`HLT`), and which opcodes take an operand.

---

## The Assembler Pipeline

The assembler transforms text into binary through a four-stage pipeline:

```
 Source Text (.asm)
       │
       ▼
 ┌─────────────┐
 │  TOKENIZER   │   Splits raw text into tokens
 │              │   (identifiers and integers)
 └──────┬──────┘
        │ Token[]
        ▼
 ┌─────────────┐
 │   PARSER     │   Converts tokens into IR instructions
 │              │   (opcode + optional operand)
 └──────┬──────┘
        │ Program (IRInstruction[])
        ▼
 ┌─────────────┐
 │ SERIALIZER   │   Writes IR to binary bytecode
 │              │   (endian-portable format)
 └──────┬──────┘
        │
        ▼
 Bytecode File (.mylangvm)
```

Each stage is implemented in its own source file and has a clean, focused responsibility. For the full details, see [Assembler Internals](05-assembler-internals.md).

---

## The VM Runtime

### Dual-Stack Model

MyLangVM maintains **two completely separate stacks**:

```
┌──────────────────────────────────────────────────────────────┐
│                          VM State                             │
│                                                               │
│  ┌─────────────────────┐    ┌─────────────────────┐          │
│  │    Operand Stack      │    │     Call Stack        │          │
│  │   (operandStack)     │    │    (callStack)       │          │
│  │                      │    │                      │          │
│  │  Used by:            │    │  Used by:            │          │
│  │  • PUSH / POP        │    │  • CALL              │          │
│  │  • ADD, SUB, MUL ... │    │  • RETURN            │          │
│  │  • PRINT / INPUT     │    │                      │          │
│  │  • Comparisons       │    │  Stores:             │          │
│  │  • DUPLICATE / SWAP  │    │  Return addresses    │          │
│  │  • STORE / LOAD      │    │  (program counter    │          │
│  │                      │    │   positions)         │          │
│  │  Capacity: 100       │    │  Capacity: 100       │          │
│  └─────────────────────┘    └─────────────────────┘          │
│                                                               │
│  ┌──────────────────────────────────────────────────────┐    │
│  │                  Memory Array                          │    │
│  │                 memory[1024]                           │    │
│  │                                                        │    │
│  │  Addressed by STORE <index> and LOAD <index>          │    │
│  │  Index range: 0 to 1023                               │    │
│  │  Each cell: 32-bit signed integer                     │    │
│  └──────────────────────────────────────────────────────┘    │
│                                                               │
│  Program Counter (pc) ─── current position in bytecode       │
│  Running flag ─── true while the VM should keep executing    │
└──────────────────────────────────────────────────────────────┘
```

**Why two stacks?**

If subroutine return addresses were mixed with operand values on the same stack, a function could accidentally pop a return address thinking it was a data value (or vice versa). The dual-stack design eliminates this class of errors entirely. Each stack serves one purpose:

- The **operand stack** is for computation — arithmetic, comparisons, I/O values.
- The **call stack** is for control flow — it only ever holds return addresses pushed by `CALL` and popped by `RETURN`.

### Memory Model

In addition to the two stacks, the VM provides a **1024-word flat memory array**:

- Accessible via `STORE <index>` (pop from operand stack, write to memory) and `LOAD <index>` (read from memory, push to operand stack).
- Each memory cell holds a 32-bit signed integer (`int32_t`).
- Index range: `0` to `1023`.
- Bounds-checked at runtime — accessing an invalid index terminates the program with an error.

This memory allows programs to store and retrieve values independently of the stack, which is essential for algorithms that need to remember multiple named values.

### Instruction Mapping

This is one of the most interesting design aspects of MyLangVM.

**The problem**: Instructions in the bytecode stream have **variable lengths**. A `PUSH 42` occupies 5 bytes (1-byte opcode + 4-byte operand), but an `ADD` occupies just 1 byte. If you want to write `JUMP 3` meaning "jump to the 4th instruction", you can't just set `pc = 3` — because byte position 3 might land in the middle of a `PUSH` operand.

**The solution**: During `vm_init()`, the VM performs a one-time scan of the entire bytecode stream and builds an **instruction map array**. This array maps logical instruction indices (0, 1, 2, 3, ...) to their actual byte positions in the bytecode.

```
Bytecode:   [PSH] [0x05 0x00 0x00 0x00] [PSH] [0x03 0x00 0x00 0x00] [ADD] [PRNT] [HLT]
Byte pos:     0     1    2    3    4       5     6    7    8    9      10     11     12

Instruction map:
  Logical 0 → Byte 0  (PUSH 5)
  Logical 1 → Byte 5  (PUSH 3)
  Logical 2 → Byte 10 (ADD)
  Logical 3 → Byte 11 (PRINT)
  Logical 4 → Byte 12 (HALT)
```

When the VM encounters `JMP 3`, it looks up `instructionMapArray[3]` and gets byte position `11`, then sets `pc = 11`. This means assembly programs can use clean, logical instruction numbers for jump targets without worrying about the binary layout.

### The Fetch-Execute Cycle

The VM runs in a tight loop (`vm_run()` calls `vm_step()` repeatedly):

```
┌──────────────────────────────────────────┐
│              vm_step()                    │
│                                          │
│  1. BOUNDS CHECK                         │
│     Is pc within program bounds?         │
│     If not → error + halt                │
│                                          │
│  2. FETCH                                │
│     Read opcode at program[pc]           │
│                                          │
│  3. EXECUTE                              │
│     Dispatch to handler function         │
│     via switch statement                 │
│     (execute_add, execute_psh, etc.)     │
│                                          │
│  4. ADVANCE PC                           │
│     If the instruction did NOT modify    │
│     pc (jumps/call/ret do),              │
│     then increment pc by 1              │
│                                          │
│  5. REPEAT                               │
│     While vm->running == true            │
└──────────────────────────────────────────┘
```

The key subtlety is step 4: some instructions (like `JMP`, `JZ`, `JNZ`, `CALL`, `RET`) set `pc` themselves as part of their execution. For these, the VM must **not** auto-increment `pc` after execution. The `is_pc_modified()` function identifies these instructions.

---

## Data Representation

All data in MyLangVM is represented as **32-bit signed integers** (`int32_t`):

| Data element | Type | Size |
|:---|:---|:---|
| Operand stack values | `int32_t` | 4 bytes |
| Memory cells | `int32_t` | 4 bytes |
| Program array elements | `int32_t` | 4 bytes |
| Bytecode opcodes | `uint8_t` | 1 byte (in file), stored as `int32_t` in memory |
| Bytecode operands | `int32_t` | 4 bytes (little-endian in file) |

The range of values is **−2,147,483,648** to **2,147,483,647** (the full `int32_t` range).

Boolean results from comparison operations (`EQUAL`, `GREATER_THAN`, etc.) are represented as integers: `1` for true, `0` for false.

---

## Portability Design

MyLangVM is designed to be portable across different platforms and CPU architectures.

### What is Already Portable

**Endianness**: The bytecode file format uses **explicit little-endian encoding**. The serializer in the assembler writes each 32-bit operand byte-by-byte in little-endian order (least significant byte first), and the loader in the VM reads them back byte-by-byte and reconstructs the value using bit shifts. This means a bytecode file assembled on a big-endian machine will run correctly on a little-endian machine, and vice versa.

**Fixed-width integers**: All integer types use `<stdint.h>` fixed-width types (`int32_t`, `uint8_t`, `uint32_t`), not platform-dependent types like `int` or `long`. This ensures consistent behavior regardless of the host's native word size.

**Standard C11**: The entire codebase uses only C11 standard library functions. There are no `#ifdef _WIN32` blocks, no POSIX-specific calls, no system-specific headers. Any conforming C11 compiler should compile the project.

**Binary file I/O**: Files are opened with `"rb"` and `"wb"` modes, which ensures correct behavior on Windows (where text mode would corrupt binary data).

### What Remains to be Done

**Build system portability**: The Makefiles use GNU Make conventions and forward-slash paths, which work on Linux, macOS, and MSYS2/MinGW on Windows, but not with MSVC's `nmake` or Visual Studio's build system directly.

**Cross-platform CI**: Automated testing across Linux, macOS, and Windows is not yet configured.

---

## Further Reading

- [Assembler Internals](05-assembler-internals.md) — detailed walkthrough of each assembler stage
- [Virtual Machine Internals](06-virtual-machine-internals.md) — detailed walkthrough of VM components
- [Bytecode Format](07-bytecode-format.md) — specification of the `.mylangvm` binary format
