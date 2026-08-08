#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "parser.h"
#include "instruction.h"
#include "serializer.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr,"Usage: %s <input.asm> <output.mylangvm>\n",argv[0]);
        return 1;
    }
    
    FILE *file_pointer = fopen(argv[1], "r");

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
    Program program = {0};
    parse(token_array, token_array_count, &program);
    serialize(&program, argv[2]);
    if (!serialize(&program, argv[2])) {
        return EXIT_FAILURE;
    }
    free(buffer);
    buffer = NULL;
	return 0;
}
