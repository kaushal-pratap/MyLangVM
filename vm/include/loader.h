#ifndef LOADER_H
#define LOADER_H
#include "vm.h"

#define MAX_PROGRAM_SIZE 1024

typedef struct ProgramMemory{
    int program[MAX_PROGRAM_SIZE];
    size_t program_size;
} ProgramMemory;

void load_program(ProgramMemory *program,
                  const char *filename);

#endif