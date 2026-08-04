#ifndef MYLANGVM_ASSEMBLER_PARSER_H
#define MYLANGVM_ASSEMBLER_PARSER_H
#include "tokenizer.h"
void parse(Token* token_array, size_t* token_array_count);
bool token_equal(const Token *token, const char *word);
void consume_integer(Token *token_array, size_t token_array_count, size_t *current);
#endif