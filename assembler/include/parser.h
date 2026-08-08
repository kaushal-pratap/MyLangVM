#ifndef MYLANGVM_ASSEMBLER_PARSER_H
#define MYLANGVM_ASSEMBLER_PARSER_H
#include "tokenizer.h"
#include "instruction.h"
void parse(const Token* token_array, size_t token_count, Program *program);
#endif