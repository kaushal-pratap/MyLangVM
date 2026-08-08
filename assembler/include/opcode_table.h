#ifndef MYLANGVM_ASSEMBLER_OPCODE_TABLE_H
#define MYLANGVM_ASSEMBLER_OPCODE_TABLE_H
#include "token.h"

typedef enum Opcode{
    OP_INVALID, 
    OP_PUSH,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_DUPLICATE,
    OP_SWAP,
    OP_MODULO,
    OP_NEGATIVE,
    OP_POSITIVE,
    OP_GREATER_THAN,
    OP_GREATER_EQUAL,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS_THAN,
    OP_LESS_EQUAL,
    OP_JUMP,
    OP_JUMP_IF_ZERO,
    OP_JUMP_IF_NOT_ZERO,
    OP_CALL,
    OP_RETURN,
    OP_STORE,
    OP_LOAD,
    OP_INPUT,
    OP_PRINT,
    OP_POP,
    OP_HALT
} Opcode;

typedef struct OpcodeEntry{
    const char *mnemonic;
    Opcode opcode;

} OpcodeEntry;

Opcode lookup_opcode(const Token *token);
#endif