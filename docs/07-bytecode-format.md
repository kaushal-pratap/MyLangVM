# Bytecode Format

This document specifies the binary format of `.mylangvm` bytecode files — the binary artifact produced by the assembler and consumed by the virtual machine.

---

## Table of Contents

1. [Overview](#overview)
2. [File Structure](#file-structure)
3. [Opcode Encoding](#opcode-encoding)
4. [Operand Encoding](#operand-encoding)
5. [Instruction Encoding Table](#instruction-encoding-table)
6. [Worked Example](#worked-example)
7. [Endianness](#endianness)
8. [File Size Calculation](#file-size-calculation)

---

## Overview

A `.mylangvm` bytecode file is a **raw binary stream** of encoded instructions. There is:

- **No file header** — no magic number, no version field, no metadata.
- **No alignment or padding** — instructions are packed tightly.
- **No instruction count** — the VM reads until end-of-file.

The file is simply a sequence of encoded instructions, one after another.

---

## File Structure

```
┌──────────────────────────────────────────────────────────┐
│                  .mylangvm bytecode file                   │
│                                                            │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐     ┌──────────┐ │
│  │ instr 0  │ │ instr 1  │ │ instr 2  │ ... │ instr N  │ │
│  └──────────┘ └──────────┘ └──────────┘     └──────────┘ │
│                                                            │
│  Each instruction is either:                               │
│    • 1 byte  (opcode only)                                │
│    • 5 bytes (opcode + 4-byte LE operand)                 │
└──────────────────────────────────────────────────────────┘
```

---

## Opcode Encoding

Each opcode is encoded as a **single unsigned byte** (`uint8_t`). The value is the numeric value of the `VmInstruction` enum:

| Opcode | Byte Value | Hex |
|:---|:---:|:---:|
| `PSH` | 0 | `0x00` |
| `ADD` | 1 | `0x01` |
| `SUB` | 2 | `0x02` |
| `MUL` | 3 | `0x03` |
| `DIV` | 4 | `0x04` |
| `DUP` | 5 | `0x05` |
| `SWP` | 6 | `0x06` |
| `MOD` | 7 | `0x07` |
| `NEG` | 8 | `0x08` |
| `POS` | 9 | `0x09` |
| `GT` | 10 | `0x0A` |
| `GE` | 11 | `0x0B` |
| `EQ` | 12 | `0x0C` |
| `NE` | 13 | `0x0D` |
| `LT` | 14 | `0x0E` |
| `LE` | 15 | `0x0F` |
| `JMP` | 16 | `0x10` |
| `JZ` | 17 | `0x11` |
| `JNZ` | 18 | `0x12` |
| `CALL` | 19 | `0x13` |
| `RET` | 20 | `0x14` |
| `STORE` | 21 | `0x15` |
| `LOAD` | 22 | `0x16` |
| `INPT` | 23 | `0x17` |
| `PRNT` | 24 | `0x18` |
| `POP` | 25 | `0x19` |
| `HLT` | 26 | `0x1A` |

---

## Operand Encoding

Instructions that take an operand encode it as a **4-byte signed integer in little-endian byte order** immediately following the opcode byte.

Little-endian means the **least significant byte comes first**:

```
Value: 42 (decimal) = 0x0000002A (hex)

Bytes in file: [0x2A] [0x00] [0x00] [0x00]
                LSB                   MSB
```

Negative example:

```
Value: -1 (decimal) = 0xFFFFFFFF (hex, two's complement)

Bytes in file: [0xFF] [0xFF] [0xFF] [0xFF]
```

Another example:

```
Value: 256 (decimal) = 0x00000100 (hex)

Bytes in file: [0x00] [0x01] [0x00] [0x00]
```

The operand is reconstructed by the VM loader using explicit bit-shifting:

```
value = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24)
```

---

## Instruction Encoding Table

| Instruction | Encoding | Size |
|:---|:---|:---:|
| `PSH <val>` | `[0x00] [val₀] [val₁] [val₂] [val₃]` | 5 bytes |
| `ADD` | `[0x01]` | 1 byte |
| `SUB` | `[0x02]` | 1 byte |
| `MUL` | `[0x03]` | 1 byte |
| `DIV` | `[0x04]` | 1 byte |
| `DUP` | `[0x05]` | 1 byte |
| `SWP` | `[0x06]` | 1 byte |
| `MOD` | `[0x07]` | 1 byte |
| `NEG` | `[0x08]` | 1 byte |
| `POS` | `[0x09]` | 1 byte |
| `GT` | `[0x0A]` | 1 byte |
| `GE` | `[0x0B]` | 1 byte |
| `EQ` | `[0x0C]` | 1 byte |
| `NE` | `[0x0D]` | 1 byte |
| `LT` | `[0x0E]` | 1 byte |
| `LE` | `[0x0F]` | 1 byte |
| `JMP <target>` | `[0x10] [tgt₀] [tgt₁] [tgt₂] [tgt₃]` | 5 bytes |
| `JZ <target>` | `[0x11] [tgt₀] [tgt₁] [tgt₂] [tgt₃]` | 5 bytes |
| `JNZ <target>` | `[0x12] [tgt₀] [tgt₁] [tgt₂] [tgt₃]` | 5 bytes |
| `CALL <target>` | `[0x13] [tgt₀] [tgt₁] [tgt₂] [tgt₃]` | 5 bytes |
| `RET` | `[0x14]` | 1 byte |
| `STORE <idx>` | `[0x15] [idx₀] [idx₁] [idx₂] [idx₃]` | 5 bytes |
| `LOAD <idx>` | `[0x16] [idx₀] [idx₁] [idx₂] [idx₃]` | 5 bytes |
| `INPT` | `[0x17]` | 1 byte |
| `PRNT` | `[0x18]` | 1 byte |
| `POP` | `[0x19]` | 1 byte |
| `HLT` | `[0x1A]` | 1 byte |

---

## Worked Example

Consider this assembly program:

```asm
PUSH 5
PUSH 3
ADD
PRINT
HALT
```

### Encoding Step-by-Step

**Instruction 0: `PUSH 5`**
- Opcode: `PSH` = `0x00`
- Operand: `5` = `0x00000005` → LE bytes: `[0x05, 0x00, 0x00, 0x00]`
- Bytes: `00 05 00 00 00`

**Instruction 1: `PUSH 3`**
- Opcode: `PSH` = `0x00`
- Operand: `3` = `0x00000003` → LE bytes: `[0x03, 0x00, 0x00, 0x00]`
- Bytes: `00 03 00 00 00`

**Instruction 2: `ADD`**
- Opcode: `ADD` = `0x01`
- No operand
- Bytes: `01`

**Instruction 3: `PRINT`**
- Opcode: `PRNT` = `0x18`
- No operand
- Bytes: `18`

**Instruction 4: `HALT`**
- Opcode: `HLT` = `0x1A`
- No operand
- Bytes: `1A`

### Complete Hex Dump

```
Offset  Hex                                  Decoded
------  ---                                  -------
0x0000  00 05 00 00 00                       PSH 5
0x0005  00 03 00 00 00                       PSH 3
0x000A  01                                   ADD
0x000B  18                                   PRNT
0x000C  1A                                   HLT
```

**Total file size: 13 bytes**

### Verifying with a Hex Viewer

You can inspect any `.mylangvm` file with `xxd`:

```bash
xxd build/bytecode/bytecode.mylangvm
```

Or with `hexdump`:

```bash
hexdump -C build/bytecode/bytecode.mylangvm
```

---

## Endianness

The bytecode format uses **little-endian** encoding for all multi-byte values. This is achieved through explicit byte-by-byte operations in both the serializer and the loader, making the format independent of the host CPU's native byte order.

| Component | Encoding |
|:---|:---|
| Opcodes | 1 byte (endianness irrelevant) |
| Operands | 4 bytes, little-endian (LSB first) |

A bytecode file assembled on a big-endian machine will produce identical bytes to one assembled on a little-endian machine, and can be executed on either.

---

## File Size Calculation

The file size of a `.mylangvm` bytecode file can be calculated as:

```
file_size = Σ (instruction_size for each instruction)

where:
  instruction_size = 5 bytes  for: PUSH, JUMP, JUMP_IF_ZERO, JUMP_IF_NOT_ZERO,
                                    CALL, STORE, LOAD
  instruction_size = 1 byte   for: all other instructions
```

Or more simply:

```
file_size = (number of operand instructions × 5) + (number of non-operand instructions × 1)
```

For example, a program with 3 `PUSH` instructions, 1 `ADD`, 1 `PRINT`, and 1 `HALT`:
```
file_size = (3 × 5) + (3 × 1) = 15 + 3 = 18 bytes
```
