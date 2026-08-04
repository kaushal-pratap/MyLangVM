#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "parser.h"

int main(void) {
    FILE *file_pointer;
    file_pointer = fopen("src/program.asm","r");
    if (file_pointer == NULL) {
        fprintf(stderr, "Error: Could not open program.asm\n");
        return 1;
    }
    fseek(file_pointer,0,SEEK_END);
    long file_size = ftell(file_pointer);
    rewind(file_pointer);
    char *buffer = malloc(file_size + 1);
    fread(buffer,sizeof(char),file_size,file_pointer);
    buffer[file_size] = '\0';
    fclose(file_pointer);
    Token token_array[TOKEN_ARRAY_SIZE];
    size_t token_array_count = 0;
    tokenize(token_array, &token_array_count, buffer);
    parse(token_array, &token_array_count);

	return 0;
}
