#include <stdio.h>
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>

void push_token(Token *token_array, size_t *token_array_count, char *start, size_t line, size_t column,size_t length){
    if(*token_array_count >= TOKEN_ARRAY_SIZE){
        fprintf(stderr,"Tokenizer Overflow !!");
        return;
    }
    token_array[*token_array_count] = (Token){
        .type = TOKEN_IDENTIFIER,
        .length = length,
        .start = start,
        .line = line,
        .column = column
    };
    (*token_array_count)++;
    
}

void complete_token(Token *token_array, size_t *token_array_count, char **start, char *current, size_t line, char *line_start){
    if(*start == current){
        *start = current + 1;
        return;
    }
    size_t column = (*start - line_start) + 1;
    size_t length = current - *start;
    push_token(token_array, token_array_count, *start,line,column,length);
    *start = current+1;    
}

void tokenize(char* buffer){
    Token token_array[TOKEN_ARRAY_SIZE];
    size_t token_array_count = 0;
    char *current = buffer;
    char *start = buffer;
    char *line_start = buffer;
    size_t line = 1;

    while(*current != '\0'){

        if(*current == ' '){
            complete_token(token_array, &token_array_count, &start, current, line, line_start);
        }else if(*current == '\n'){
            complete_token(token_array, &token_array_count, &start, current, line, line_start);
            line++;
            line_start = current+1;
        }

        current++;
    }
    // 1. Push any trailing token if file doesn't end with space or newline
    complete_token(token_array, &token_array_count, &start,current, line, line_start
);

    // Loop to iterate in every token array and to print line and column of each
    for(size_t i = 0; i < token_array_count; i++){
        printf("Length : %zu\n",token_array[i].length);
        printf("line and column : %zu,%zu\n",token_array[i].line,token_array[i].column);
        printf("\n");
    }
}
