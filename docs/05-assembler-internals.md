# Assembler Internals

This document explains how the MyLangVM assembler (`mylangasm`) works internally. It traces the entire pipeline from reading a `.asm` source file to emitting a binary `.mylangvm` bytecode file.

---

## Table of Contents

1. [Pipeline Overview](#pipeline-overview)
2. [Entry Point: main.c](#entry-point-mainc)
3. [Stage 1: Tokenizer](#stage-1-tokenizer)
   - [Token Types](#token-types)
   - [Token Structure](#token-structure)
   - [Tokenization Algorithm](#tokenization-algorithm)
   - [Integer Detection](#integer-detection)
4. [Stage 2: Opcode Lookup](#stage-2-opcode-lookup)
   - [The Opcode Table](#the-opcode-table)
   - [Case-Insensitive Matching](#case-insensitive-matching)
5. [Stage 3: Parser](#stage-3-parser)
   - [IR Instruction Structure](#ir-instruction-structure)
   - [Parsing Algorithm](#parsing-algorithm)
   - [Operand Consumption](#operand-consumption)
   - [Integer Conversion](#integer-conversion)
6. [Stage 4: Serializer](#stage-4-serializer)
   - [Opcode Mapping (IR → VM)](#opcode-mapping-ir--vm)
   - [Little-Endian Encoding](#little-endian-encoding)
   - [Binary Emission](#binary-emission)
7. [Error Reporting](#error-reporting)
8. [Limits and Constants](#limits-and-constants)
9. [Source File Map](#source-file-map)

---

## Pipeline Overview

The assembler transforms source text into bytecode through four sequential stages:

```
         my_program.asm
              │
              ▼
    ┌──────────────────┐
    │   Read File       │  main.c
    │   into buffer     │  fopen → fseek → ftell → malloc → fread
    └────────┬─────────┘
             │ char *buffer
             ▼
    ┌──────────────────┐
    │   TOKENIZER       │  tokenizer.c
    │                    │
    │   Splits buffer    │  Input:  "PUSH 5\nADD\nPRINT\nHALT\n"
    │   on whitespace    │  Output: [PUSH] [5] [ADD] [PRINT] [HALT]
    │   into tokens      │          (Token array)
    └────────┬─────────┘
             │ Token token_array[]
             ▼
    ┌──────────────────┐
    │   PARSER          │  parser.c + opcode_table.c
    │                    │
    │   Converts tokens  │  Input:  [PUSH] [5] [ADD] [PRINT] [HALT]
    │   into IR instrs   │  Output: [{OP_PUSH,5,true}, {OP_ADD,0,false}, ...]
    └────────┬─────────┘
             │ Program (IRInstruction array)
             ▼
    ┌──────────────────┐
    │   SERIALIZER      │  serializer.c
    │                    │
    │   Maps IR opcodes  │  Input:  [{OP_PUSH,5,true}, {OP_ADD,0,false}, ...]
    │   to VM opcodes    │  Output: [0x00][0x05,0x00,0x00,0x00][0x01][0x18][0x1A]
    │   and writes binary│          (bytecode file)
    └────────┬─────────┘
             │
             ▼
       bytecode.mylangvm
```

Each stage has a single, focused responsibility. The data flows forward through clearly defined interfaces: a character buffer, a token array, an IR instruction array, and finally a binary file.

---

## Entry Point: main.c

The assembler's `main()` function orchestrates the pipeline:

1. **Argument validation**: Expects exactly two arguments — the input `.asm` file path and the output `.mylangvm` file path. Prints usage and exits if the count is wrong.

2. **File reading**: Opens the input file, determines its size using `fseek`/`ftell`, allocates a buffer, reads the entire file into memory with `fread`, and null-terminates the buffer. All error conditions (file not found, allocation failure, read failure) are handled with diagnostic messages.

3. **Tokenization**: Calls `tokenize()` with the buffer, producing a filled `Token` array and a count.

4. **Parsing**: Calls `parse()` with the token array, producing a filled `Program` structure containing IR instructions.

5. **Serialization**: Calls `serialize()` with the `Program` and the output file path. Returns `EXIT_FAILURE` if serialization fails.

6. **Cleanup**: Frees the buffer and exits.

---

## Stage 1: Tokenizer

**Source files**: `tokenizer.c`, `tokenizer.h`, `token.h`

The tokenizer's job is to break the raw source text into discrete tokens. It is a simple **whitespace-splitting** scanner — it does not understand the language's grammar, it just finds boundaries between words.

### Token Types

The `TokenType` enum defines the possible token categories:

| Type | Meaning |
|:---|:---|
| `TOKEN_IDENTIFIER` | A word — an instruction mnemonic like `PUSH`, `ADD`, `HALT` |
| `TOKEN_INTEGER` | A numeric literal like `42`, `-7`, `2147483647` |
| `TOKEN_SEMI_COLON` | Reserved for future comment support |
| `TOKEN_COMMENT` | Reserved for future comment support |
| `TOKEN_NEWLINE` | Reserved |
| `TOKEN_END_OF_FILE` | Reserved |
| `TOKEN_INVALID` | Reserved |

In the current implementation, the tokenizer only produces `TOKEN_IDENTIFIER` and `TOKEN_INTEGER` tokens. The other types are defined for future expansion.

### Token Structure

Each token carries:

```c
typedef struct Token {
    TokenType type;       // TOKEN_IDENTIFIER or TOKEN_INTEGER
    const char *start;    // Pointer into the source buffer (no copy)
    size_t length;        // Number of characters
    size_t line;          // 1-based line number
    size_t column;        // 1-based column number
} Token;
```

Tokens are **zero-copy** — they point directly into the original source buffer rather than allocating separate strings. This is efficient and avoids unnecessary memory management.

### Tokenization Algorithm

The tokenizer works by scanning the buffer character by character:

1. Maintain two pointers: `start` (beginning of the current token) and `current` (scanning position).
2. Advance `current` through the buffer.
3. When a whitespace character is encountered (space, tab, newline, carriage return):
   - If `start != current`, there is a non-empty token between them — emit it.
   - Reset `start` to `current + 1`.
   - If the whitespace was a newline, increment the line counter and update the line-start pointer (for column calculation).
4. After the loop ends, emit any trailing token (handles files that don't end with a newline).

The `emit_token()` function computes the column number as `(start - line_start) + 1`, determines the token's length as `current - start`, and delegates to `push_token()`.

The `push_token()` function classifies the token (identifier vs. integer) and appends it to the token array, checking for overflow.

### Integer Detection

The `is_integer()` function determines whether a token is a numeric literal:

1. Skip an optional leading `-` (negative sign).
2. Every remaining character must be a digit (`'0'` through `'9'`).
3. A lone `-` with no digits is **not** an integer.

This correctly handles positive integers, negative integers, and zero.

---

## Stage 2: Opcode Lookup

**Source files**: `opcode_table.c`, `opcode_table.h`

The opcode table maps human-readable mnemonics to internal `Opcode` enum values.

### The Opcode Table

The table is a static array of `OpcodeEntry` structs:

```c
const OpcodeEntry opcode_table[] = {
    {"PUSH",              OP_PUSH},
    {"ADD",               OP_ADD},
    {"SUBTRACT",          OP_SUBTRACT},
    {"MULTIPLY",          OP_MULTIPLY},
    {"DIVIDE",            OP_DIVIDE},
    {"DUPLICATE",         OP_DUPLICATE},
    {"SWAP",              OP_SWAP},
    {"MODULO",            OP_MODULO},
    {"NEGATIVE",          OP_NEGATIVE},
    {"POSITIVE",          OP_POSITIVE},
    {"GREATER_THAN",      OP_GREATER_THAN},
    {"GREATER_EQUAL",     OP_GREATER_EQUAL},
    {"EQUAL",             OP_EQUAL},
    {"NOT_EQUAL",         OP_NOT_EQUAL},
    {"LESS_THAN",         OP_LESS_THAN},
    {"LESS_EQUAL",        OP_LESS_EQUAL},
    {"JUMP",              OP_JUMP},
    {"JUMP_IF_ZERO",      OP_JUMP_IF_ZERO},
    {"JUMP_IF_NOT_ZERO",  OP_JUMP_IF_NOT_ZERO},
    {"CALL",              OP_CALL},
    {"RETURN",            OP_RETURN},
    {"STORE",             OP_STORE},
    {"LOAD",              OP_LOAD},
    {"INPUT",             OP_INPUT},
    {"PRINT",             OP_PRINT},
    {"POP",               OP_POP},
    {"HALT",              OP_HALT}
};
```

All 27 mnemonics are stored in uppercase in the table.

### Case-Insensitive Matching

The `lookup_opcode()` function performs a linear scan through the table, using `token_equal()` for comparison. The `token_equal()` function compares character-by-character, converting each character of the input token to uppercase via `toupper()` before comparing with the table entry (which is already uppercase).

This means `push`, `PUSH`, `Push`, and `pUsH` all match `OP_PUSH`.

If no match is found, `OP_INVALID` is returned, and the parser will report an error.

---

## Stage 3: Parser

**Source files**: `parser.c`, `parser.h`

The parser converts the flat token array into a structured array of IR (Intermediate Representation) instructions.

### IR Instruction Structure

Each IR instruction is defined in `common/instruction.h`:

```c
typedef struct IRInstruction {
    Opcode opcode;       // The assembler-level opcode (OP_PUSH, OP_ADD, etc.)
    int32_t operand;     // The operand value (meaningful only if has_operand is true)
    bool has_operand;    // Whether this instruction carries an operand
} IRInstruction;
```

Instructions are collected into a `Program` struct:

```c
typedef struct Program {
    IRInstruction instructions[MAX_INSTRUCTION_SIZE];  // Array of IR instructions
    size_t instruction_count;                           // Number of valid entries
} Program;
```

The maximum instruction count is 1024 (`MAX_INSTRUCTION_SIZE`).

### Parsing Algorithm

The parser iterates through the token array with a `current` index:

1. Look up the current token as an opcode using `lookup_opcode()`.
2. Based on the opcode, determine whether it takes an operand:
   - **Instructions with operands** (`PUSH`, `JUMP`, `JUMP_IF_ZERO`, `JUMP_IF_NOT_ZERO`, `CALL`, `STORE`, `LOAD`): Call `push_instruction_with_operand()`, which consumes the **next** token as the integer operand.
   - **Instructions without operands** (all others): Call `push_instruction()` with operand `0` and `has_operand = false`.
   - **`OP_INVALID`**: Print an error with the token's line and column number, then exit.
3. Advance `current` and repeat.

### Operand Consumption

The `consume_integer()` function:

1. Checks that there **is** a next token (otherwise: `Error: Integer missing` with line/column).
2. Checks that the next token **is** of type `TOKEN_INTEGER` (otherwise: `Error: Expected integer` with line/column).
3. Advances the current index and returns a pointer to the integer token.

### Integer Conversion

The `token_to_int()` function manually parses the token's character span into a `long long` value:

1. Handle an optional leading `-` sign.
2. Accumulate digits one by one: `value = value * 10 + digit`.
3. Apply the sign.
4. Range-check against `INT_MAX` and `INT_MIN` to ensure it fits in a 32-bit signed integer.
5. Return as `int`.

This manual parsing avoids `atoi` / `strtol` and gives precise control over error handling.

---

## Stage 4: Serializer

**Source files**: `serializer.c`, `serializer.h`

The serializer takes the `Program` (array of IR instructions) and writes a binary bytecode file.

### Opcode Mapping (IR → VM)

The assembler uses its own `Opcode` enum (`OP_PUSH`, `OP_ADD`, etc.) which is separate from the VM's `VmInstruction` enum (`PSH`, `ADD`, etc.). These are intentionally distinct — the assembler's opcodes are descriptive names used during compilation, while the VM's opcodes are compact numeric values used at runtime.

The serializer maintains an `opcode_map[]` array that maps each assembler opcode to its corresponding VM opcode:

```
OP_PUSH     → PSH  (0)
OP_ADD      → ADD  (1)
OP_SUBTRACT → SUB  (2)
...and so on for all 27 opcodes
```

The `assembler_opcode_to_vm_opcode()` function performs a linear scan through this table. If an unmapped opcode is encountered (which should never happen), it prints an error and exits.

### Little-Endian Encoding

Operands (32-bit signed integers) are written in **little-endian byte order**, regardless of the host machine's native byte order. The `write_int32_le()` function does this manually:

```c
static bool write_int32_le(FILE *file, int32_t value) {
    uint32_t bits = (uint32_t)value;
    uint8_t bytes[4];
    bytes[0] = (uint8_t)(bits & 0xFF);          // least significant byte
    bytes[1] = (uint8_t)((bits >> 8) & 0xFF);
    bytes[2] = (uint8_t)((bits >> 16) & 0xFF);
    bytes[3] = (uint8_t)((bits >> 24) & 0xFF);   // most significant byte
    return fwrite(bytes, sizeof(uint8_t), 4, file) == 4;
}
```

This bit-shifting approach is endian-neutral — it produces the same byte sequence on both little-endian (x86, ARM) and big-endian (some MIPS, POWER) architectures. This is a key portability feature.

### Binary Emission

The `serialize()` function iterates over the IR instruction array:

1. **Open** the output file in binary write mode (`"wb"`).
2. For each instruction:
   a. Convert the assembler opcode to a VM opcode.
   b. Cast the VM opcode to `uint8_t` and write **1 byte** (the opcode).
   c. If the instruction has an operand (`has_operand == true`), write the operand as **4 bytes** in little-endian order.
3. **Close** the file, checking for errors.

The result is a compact binary stream with no padding, no headers, and no metadata — just raw opcodes and operands.

---

## Error Reporting

The assembler reports errors with line and column information wherever possible:

| Error | When | Message Format |
|:---|:---|:---|
| Unknown instruction | Token doesn't match any mnemonic | `Unknown instruction` + line/column |
| Missing operand | Instruction that requires an operand is at end of file | `Error: Integer missing` + line/column |
| Wrong operand type | Token after an operand instruction is not an integer | `Error: Expected integer` + line/column |
| Value overflow | Parsed integer exceeds `INT32` range | `Error: Maximum instruction limit exceeded` |
| Token overflow | More than 100 tokens | `Tokenizer Overflow !!` |
| Instruction overflow | More than 1024 instructions | `Instructions overflow !!` |
| File errors | Various I/O failures | Specific messages to stderr |

All errors are printed to `stderr`. Fatal errors call `exit(EXIT_FAILURE)`.

---

## Limits and Constants

| Constant | Value | Defined In | Meaning |
|:---|:---:|:---|:---|
| `TOKEN_ARRAY_SIZE` | 100 | `tokenizer.h` | Maximum number of tokens |
| `MAX_INSTRUCTION_SIZE` | 1024 | `instruction.h` | Maximum IR instructions in a program |

---

## Source File Map

| File | Role |
|:---|:---|
| `assembler/src/main.c` | Entry point — reads file, orchestrates pipeline |
| `assembler/src/tokenizer.c` | Whitespace-splitting tokenizer |
| `assembler/src/opcode_table.c` | Mnemonic → opcode lookup table |
| `assembler/src/parser.c` | Token array → IR instruction array |
| `assembler/src/serializer.c` | IR → binary bytecode file |
| `assembler/include/token.h` | Token type enum and struct |
| `assembler/include/tokenizer.h` | Tokenizer function prototypes |
| `assembler/include/opcode_table.h` | Opcode enum and table struct |
| `assembler/include/parser.h` | Parser function prototype |
| `assembler/include/serializer.h` | Serializer function prototype |
| `common/instruction.h` | Shared — VM opcodes, IR struct, Program struct |
