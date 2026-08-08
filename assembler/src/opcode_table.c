#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "opcode_table.h"
#include "token.h"

#define OPCODE_COUNT (sizeof(opcode_table) / sizeof(opcode_table[0]))

const OpcodeEntry opcode_table[] = {
    {"PUSH", OP_PUSH},
    {"ADD", OP_ADD},
    {"SUBTRACT", OP_SUBTRACT},
    {"MULTIPLY", OP_MULTIPLY},
    {"DIVIDE", OP_DIVIDE},
    {"DUPLICATE", OP_DUPLICATE},
    {"SWAP", OP_SWAP},
    {"MODULO", OP_MODULO},
    {"NEGATIVE", OP_NEGATIVE},
    {"POSITIVE", OP_POSITIVE},
    {"GREATER_THAN",OP_GREATER_THAN},
    {"GREATER_EQUAL", OP_GREATER_EQUAL},
    {"EQUAL", OP_EQUAL},
    {"NOT_EQUAL", OP_NOT_EQUAL},
    {"LESS_THAN", OP_LESS_THAN},
    {"LESS_EQUAL", OP_LESS_EQUAL},
    {"JUMP", OP_JUMP},
    {"JUMP_IF_ZERO", OP_JUMP_IF_ZERO},
    {"JUMP_IF_NOT_ZERO", OP_JUMP_IF_NOT_ZERO},
    {"CALL", OP_CALL},
    {"RETURN", OP_RETURN},
    {"STORE", OP_STORE},
    {"LOAD", OP_LOAD},
    {"INPUT", OP_INPUT},
    {"PRINT", OP_PRINT},
    {"POP", OP_POP},
    {"HALT", OP_HALT}
};

static bool token_equal(const Token *token, const char *word){
    if(token->length != strlen(word)){
        return false;
    }
    for(size_t i = 0; i < token->length; i++){
        if(toupper((unsigned char)token->start[i]) != word[i]){
            return false;
        }
    }
    return true;
}

Opcode lookup_opcode(const Token *token)
{
    for (size_t i = 0; i < OPCODE_COUNT; i++) {
        if (token_equal(token, opcode_table[i].mnemonic)) {
            return opcode_table[i].opcode;
        }
    }

    return OP_INVALID;
}