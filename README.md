<p align="center">
  <h1 align="center">MyLangVM</h1>
  <p align="center">
    <strong>A stack-based bytecode virtual machine and assembler toolchain, built from scratch in C.</strong>
  </p>
  <p align="center">
    <a href="#-quick-start">Quick Start</a> · <a href="docs/03-assembly-language-guide.md">Write Programs</a> · <a href="docs/04-instruction-set-reference.md">ISA Reference</a> · <a href="#-documentation">Full Docs</a>
  </p>
</p>

---

## What is MyLangVM?

**MyLangVM** is a complete, from-scratch implementation of a language virtual machine system in portable C11. It consists of two independent programs that work together:

1. **`mylangasm`** — An assembler that reads human-readable `.asm` source files and compiles them into a compact binary bytecode format (`.mylangvm`).
2. **`mylangvm`** — A virtual machine that loads and executes those bytecode files using a stack-based runtime engine.

You write programs in a clean, readable assembly language. The assembler translates your mnemonics into bytecode. The VM runs the bytecode. The two programs communicate exclusively through the bytecode file — a well-defined binary interface.

```
                          MyLangVM Toolchain
  ┌─────────────┐       ┌─────────────┐       ┌─────────────┐
  │  .asm file   │──────▶│  mylangasm   │──────▶│  .mylangvm   │
  │  (source)    │       │  (assembler) │       │  (bytecode)  │
  └─────────────┘       └─────────────┘       └──────┬──────┘
                                                      │
                                                      ▼
                                               ┌─────────────┐
                                               │  mylangvm    │
                                               │  (vm engine) │
                                               └──────┬──────┘
                                                      │
                                                      ▼
                                                   Output
```

---

## ✨ Features

- **27 instructions** covering arithmetic, comparison, control flow, memory, I/O, and stack manipulation
- **Dual-stack architecture** — separate operand stack and call stack for clean subroutine support
- **1024-word addressable memory** — `STORE` / `LOAD` instructions for persistent data
- **Subroutine calls** — `CALL` / `RETURN` with automatic return-address management
- **Conditional and unconditional jumps** — `JUMP`, `JUMP_IF_ZERO`, `JUMP_IF_NOT_ZERO`
- **User I/O** — `INPUT` reads integers from stdin, `PRINT` writes to stdout
- **Logical instruction addressing** — jump targets are logical instruction numbers, not raw byte offsets
- **Human-readable assembly** — full-word mnemonics like `PUSH`, `MULTIPLY`, `GREATER_THAN`
- **Case-insensitive assembler** — write `PUSH`, `push`, or `Push` — all are valid
- **Endian-portable bytecode** — the serializer writes operands in little-endian format regardless of host byte order
- **Clean C11 codebase** — no platform-specific APIs, standard C only

---

## 📁 Project Structure

```
MyLangVM/
├── Makefile                          Root build orchestrator
├── mylang                            Shell script — assemble + run in one step
├── LICENSE                           GPL-3.0
│
├── common/                           Shared definitions
│   └── instruction.h                 VM instruction enum, IR structures, limits
│
├── assembler/                        Assembler toolchain (mylangasm)
│   ├── Makefile                      Assembler build script
│   ├── include/
│   │   ├── token.h                   Token types and structures
│   │   ├── tokenizer.h               Tokenizer API
│   │   ├── opcode_table.h            Opcode enum and lookup table
│   │   ├── parser.h                  Parser API
│   │   └── serializer.h              Serializer API
│   └── src/
│       ├── main.c                    Assembler entry point and pipeline
│       ├── tokenizer.c               Whitespace-splitting tokenizer
│       ├── opcode_table.c            Mnemonic → opcode lookup
│       ├── parser.c                  Token stream → IR instruction array
│       └── serializer.c              IR → binary bytecode emitter
│
├── vm/                               Virtual machine (mylangvm)
│   ├── Makefile                      VM build script
│   ├── include/
│   │   ├── vm.h                      VM state structure and lifecycle API
│   │   ├── stack.h                   Stack data structure and operations
│   │   ├── instruction_handlers.h    Handler function prototypes
│   │   └── loader.h                  Bytecode file loader
│   └── src/
│       ├── main.c                    VM entry point
│       ├── vm.c                      Core VM — init, fetch, decode, execute loop
│       ├── stack.c                   Stack push, pop, peek, overflow checks
│       ├── instruction_handlers.c    All 27 instruction implementations
│       ├── instruction.c             Instruction size and PC-modification queries
│       └── loader.c                  Binary bytecode file reader
│
├── examples/                         Example programs organized by category
│   ├── arithmetic/                   add, subtract, multiply, divide, modulo
│   ├── comparison/                   equal, not_equal, greater_than, less_than, ...
│   ├── control_flow/                 jump, jump_if_zero, jump_if_not_zero, call_return, ...
│   ├── demos/                        Multi-instruction demos (calculator, expression, ...)
│   ├── io/                           input, echo
│   ├── memory/                       store_load, overwrite
│   ├── stack/                        duplicate, pop, swap
│   └── unary/                        negative, positive
│
├── tests/                            Test programs
│   └── push_test.asm                 Edge-case test (INT32_MIN, negative multiply)
│
├── build/                            Build output (generated)
│   ├── assembler/                    mylangasm binary
│   ├── bytecode/                     Compiled .mylangvm files
│   └── vm/                           mylangvm binary
│
└── docs/                             In-depth documentation
    ├── 01-getting-started.md         Installation, build, first program
    ├── 02-architecture.md            System design deep-dive
    ├── 03-assembly-language-guide.md Learn to write programs
    ├── 04-instruction-set-reference.md  Complete ISA reference
    ├── 05-assembler-internals.md     How the assembler works
    ├── 06-virtual-machine-internals.md  How the VM works
    ├── 07-bytecode-format.md         Binary format specification
    └── 08-examples-cookbook.md        Annotated example walkthroughs
```

---

## 🚀 Quick Start

### Prerequisites

| Requirement | Notes |
|:---|:---|
| **C compiler** | Any C11-capable compiler — `gcc`, `clang`, or `cc` |
| **GNU Make** | `make` on Linux/macOS, `mingw32-make` on Windows with MinGW |
| **Shell** (optional) | `sh` for the `mylang` convenience script |

### Build Everything

From the project root:

```bash
make
```

This builds both `mylangasm` (the assembler) and `mylangvm` (the virtual machine). The binaries are placed in `build/assembler/` and `build/vm/`.

### Assemble and Run a Program

**Step 1 — Assemble** your `.asm` source file into bytecode:

```bash
./build/assembler/mylangasm examples/arithmetic/add.asm build/bytecode/bytecode.mylangvm
```

**Step 2 — Execute** the bytecode:

```bash
./build/vm/mylangvm build/bytecode/bytecode.mylangvm
```

**Output:**

```
8
```

### One-Step Shortcut

Use the `mylang` shell script to assemble and run in a single command:

```bash
./mylang examples/arithmetic/add.asm
```

### Clean Build Artifacts

```bash
make clean
```

---

## 🔤 Your First Program

Create a file called `hello.asm`:

```asm
PUSH 10
PUSH 20
ADD
PRINT
HALT
```

Assemble and run it:

```bash
./build/assembler/mylangasm hello.asm build/bytecode/bytecode.mylangvm
./build/vm/mylangvm build/bytecode/bytecode.mylangvm
```

**Output:**

```
30
```

That's it. Five instructions, one result. To learn how to write more complex programs — with loops, conditionals, functions, memory, and user input — head over to the [Assembly Language Guide](docs/03-assembly-language-guide.md).

---

## 🏗️ Architecture at a Glance

MyLangVM is a **stack-based** virtual machine. All operations work by pushing values onto and popping values off of a stack — there are no named registers.

**Key design decisions:**

| Concept | Implementation |
|:---|:---|
| **Execution model** | Stack-based (not register-based) |
| **Dual stacks** | Separate operand stack and call stack |
| **Memory** | 1024-word flat address space |
| **Instruction sizes** | Variable — 1 byte (opcode only) or 5 bytes (opcode + 4-byte operand) |
| **Jump addressing** | Logical instruction indices, not raw byte offsets |
| **Bytecode endianness** | Serialized as little-endian, decoded on any host |
| **Data type** | 32-bit signed integers (`int32_t`) |

For the full architecture deep-dive, see [Architecture](docs/02-architecture.md).

---

## 🖥️ Platform Support

MyLangVM is written in standard C11 with no platform-specific system calls. It builds and runs on any system with a conforming C11 compiler and `make`.

| Platform | Compiler | Status |
|:---|:---|:---|
| **macOS** (Apple Silicon / Intel) | `clang` (Xcode CLI tools) | ✅ Fully tested |
| **Linux** (x86_64, ARM64) | `gcc`, `clang` | ✅ Builds and runs |
| **Windows** (MinGW / MSYS2) | `gcc` (MinGW-w64) | ✅ Builds and runs |
| **Windows** (MSVC) | `cl.exe` | Requires manual compilation (no Makefile support) |

### Portability Status

| Area | Status |
|:---|:---|
| **Source code** | Standard C11, no platform-specific APIs |
| **Bytecode endianness** | ✅ Portable — little-endian serialization/deserialization regardless of host byte order |
| **Build system** | GNU Make — works on Linux, macOS, and MinGW on Windows |
| **File I/O** | Standard `fopen` / `fread` / `fwrite` with binary mode (`"rb"`, `"wb"`) |
| **Integer types** | Fixed-width `int32_t` / `uint8_t` / `uint32_t` via `<stdint.h>` |
| **Full cross-platform CI** | Not yet configured — planned for future releases |

---

## 📚 Documentation

The `docs/` directory contains comprehensive, in-depth documentation covering every aspect of MyLangVM. Here is the recommended reading order:

| # | Document | Description |
|:---:|:---|:---|
| 1 | **[Getting Started](docs/01-getting-started.md)** | Installation, building, running your first program |
| 2 | **[Architecture](docs/02-architecture.md)** | System design, dual-stack model, execution pipeline |
| 3 | **[Assembly Language Guide](docs/03-assembly-language-guide.md)** | Complete tutorial — learn to write programs from scratch |
| 4 | **[Instruction Set Reference](docs/04-instruction-set-reference.md)** | Detailed reference for all 27 instructions |
| 5 | **[Assembler Internals](docs/05-assembler-internals.md)** | How the assembler pipeline works (tokenizer → parser → serializer) |
| 6 | **[Virtual Machine Internals](docs/06-virtual-machine-internals.md)** | How the VM loads, maps, and executes bytecode |
| 7 | **[Bytecode Format](docs/07-bytecode-format.md)** | Binary format specification for `.mylangvm` files |
| 8 | **[Examples Cookbook](docs/08-examples-cookbook.md)** | Annotated walkthroughs of every example program |

> **Reading tip:** If you just want to start writing programs, jump straight to the [Assembly Language Guide](docs/03-assembly-language-guide.md). If you want to understand how the system works under the hood, start with [Architecture](docs/02-architecture.md) and then read the internals documents.

---

## 📄 License

This project is licensed under the [GNU General Public License v3.0](LICENSE).

---

<p align="center">
  <strong>Built from scratch in C · No dependencies · No magic</strong>
</p>
