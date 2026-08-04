#include <stdio.h>
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void push_token(Token *token_array, size_t *token_array_count, char *start, size_t line, size_t column,size_t length){
    if(*token_array_count >= TOKEN_ARRAY_SIZE){
        fprintf(stderr,"Tokenizer Overflow !!");
        return;
    }
    token_array[*token_array_count] = (Token){
        .type = token_type(start,length),
        .length = length,
        .start = start,
        .line = line,
        .column = column
    };
    (*token_array_count)++;
    
}

void emit_token(Token *token_array, size_t *token_array_count, char **start, char *current, size_t line, char *line_start){
    if(*start == current){
        *start = current + 1;
        return;
    }
    size_t column = (*start - line_start) + 1;
    size_t length = current - *start;
    push_token(token_array, token_array_count, *start,line,column,length);
    *start = current+1;    
}

void tokenize(Token* token_array, size_t *token_array_count, char* buffer){
    
    char *current = buffer;
    char *start = buffer;
    char *line_start = buffer;
    size_t line = 1;

    while(*current != '\0'){

        if(is_whitespace(*current)){
            emit_token(token_array, token_array_count, &start, current, line, line_start);
            if(*current == '\n'){
                line++;
                line_start = current+1;
            }
        }
        current++;
    }
    // 1. Push any trailing token if file doesn't end with space or newline
    emit_token(token_array, token_array_count, &start,current, line, line_start);

    // Loop to iterate in every token array and to print line and column of each
    // for(size_t i = 0; i < *token_array_count; i++){
    //     printf("%.*s\n",(int)token_array[i].length,token_array[i].start);
    //     if(token_array[i].type == TOKEN_INTEGER){
    //         printf("Token type : Integer\n");
    //     }else{
    //         printf("Token type : Identifier\n");
    //     }
    //     printf("Length : %zu\n",token_array[i].length);
    //     printf("line and column : %zu,%zu\n",token_array[i].line,token_array[i].column);
    //     printf("khikhiki, i moved token array count to main.c");
    //     printf("\n");
    // }
}

bool is_whitespace(char c){
    return c == ' ' ||
           c == '\n' || 
           c == '\t' ||
           c == '\r';
}

TokenType token_type(char *start, size_t length) {

    if(is_integer(start, length)){
        return TOKEN_INTEGER;
    }else{
        return TOKEN_IDENTIFIER;
    }

}

bool is_integer(char *start, size_t length){
    if(length == 0){
        return false;
    }
    if(*start == '-'){
        length--;
        start++;
    }
    if(length == 0){
        return false;
    }
    while(length > 0){
        if(*start < '0' || *start > '9'){
            return false;
        }
        start++;
        length--;
    }
    return true;
}