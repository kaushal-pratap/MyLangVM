#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "parser.h"
#include "instruction.h"
#include "serializer.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.mylangvm>\n", argv[0]);
        return 1;
    }

    FILE *file_pointer = fopen(argv[1], "r");

    if (file_pointer == NULL) {
        fprintf(stderr, "Error: Could not open program.asm\n");
        return 1;
    }

    if (fseek(file_pointer, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: Could not determine file size\n");
        fclose(file_pointer);
        return EXIT_FAILURE;
    }

    long file_size = ftell(file_pointer);

    if (file_size < 0) {
        fprintf(stderr, "Error: Could not determine file size\n");
        fclose(file_pointer);
        return EXIT_FAILURE;
    }

    rewind(file_pointer);

    char *buffer = malloc((size_t)file_size + 1);

    if (buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for source file\n");
        fclose(file_pointer);
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread(
        buffer,
        sizeof(char),
        (size_t)file_size,
        file_pointer
    );

    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: Could not read source file\n");
        free(buffer);
        fclose(file_pointer);
        return EXIT_FAILURE;
    }

    buffer[bytes_read] = '\0';

    fclose(file_pointer);

    Token token_array[TOKEN_ARRAY_SIZE];
    size_t token_array_count = 0;

    tokenize(token_array, &token_array_count, buffer);

    Program program = {0};

    parse(token_array, token_array_count, &program);

    if (!serialize(&program, argv[2])) {
        free(buffer);
        return EXIT_FAILURE;
    }

    free(buffer);

    return 0;
}