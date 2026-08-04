#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include <ctype.h>

bool token_equal(const Token *token, const char *word){
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
void consume_integer(Token *token_array, size_t token_array_count, size_t *current){
    if(*current+1 >= token_array_count){
        fprintf(stderr,"Error: Integer missing \nLine %zu, Column %zu\n",token_array[*current].line, token_array[*current].column);
        exit(EXIT_FAILURE);
    }
    (*current)++;
    if(token_array[*current].type != TOKEN_INTEGER){
        fprintf(stderr,"Error: Expected integer \nLine %zu, Column %zu\n",token_array[*current].line, token_array[*current].column);
        exit(EXIT_FAILURE);
    }
}

void parse(Token* token_array, size_t* token_array_count){
    size_t current = 0;
    while(current < *token_array_count){
        if(token_equal(&token_array[current],"PUSH")){
            consume_integer(token_array, *token_array_count, &current);
            printf("PUSH Found\n");    
    }
    else if(token_equal(&token_array[current],"ADD")){
        printf("ADD Found\n");
    }
    else if(token_equal(&token_array[current],"MUL")){
        printf("Multiply Found\n");
    }
    else if(token_equal(&token_array[current],"SUB")){
        printf("Subtraction Found\n");
    }
    else if(token_equal(&token_array[current],"JUMP")){
        consume_integer(token_array, *token_array_count, &current);
        printf("JUMP Found\n");  
    }
    else {
        fprintf(stderr,"Unknown instruction\n"
            "Line %zu, Column %zu\n",
            token_array[current].line,
            token_array[current].column);

    exit(EXIT_FAILURE);
}
        current++;
    }
    
}