#ifndef MYLANGVM_ASSEMBLER_TOKEN_H
#define MYLANGVM_ASSEMBLER_TOKEN_H
#include <stddef.h>

// Here we define the type of tokens we will parse.
typedef enum TokenType {
    TOKEN_IDENTIFIER, // An identifier such as "PSH" or "loop".
    TOKEN_INTEGER, // A numeric literal
    TOKEN_NEWLINE, // The tokenizer encountered a newline character.
    TOKEN_END_OF_FILE, // It means we've completed reading the file 
    TOKEN_INVALID, // An unrecognized or invalid character sequence.
}TokenType;

// Here we define the properties of a token, what info it should carry.
typedef struct Token {
    TokenType type;
    const char *start;
    size_t length;
    size_t line;
    size_t column;
}Token;

#endif