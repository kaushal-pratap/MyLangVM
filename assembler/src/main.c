#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"

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

    tokenize(buffer);

	return 0;
}
