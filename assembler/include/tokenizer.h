#ifndef MYLANGVM_ASSEMBLER_TOKENIZER_H
#define MYLANGVM_ASSEMBLER_TOKENIZER_H
#include "token.h"

#define TOKEN_ARRAY_SIZE 100

void tokenize(char* buffer);
void push_token(Token *token_array, size_t *token_array_count, char *start, size_t line, size_t column,size_t length);
void complete_token(Token *token_array, size_t *token_array_count, char **start, char *current, size_t line, char *line_start);

#endif