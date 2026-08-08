#include "loader.h"

void load_program(ProgramMemory *program_memory, const char *filename){
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open bytecode.mylangvm\n");
        exit(EXIT_FAILURE);
    }

    VmInstruction opcode;
    int operand;

    while(fread(&opcode, sizeof(opcode), 1, file) == 1){
        if (program_memory->program_size >= MAX_PROGRAM_SIZE) {
            fprintf(stderr, "Program too large\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        program_memory->program[program_memory->program_size] = opcode;
        program_memory->program_size++;
        if(instruction_size(opcode) == 2){
            if (program_memory->program_size >= MAX_PROGRAM_SIZE) {
                fprintf(stderr, "Program too large\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            if (fread(&operand, sizeof(operand), 1, file) != 1) {
                fprintf(stderr, "Read failed : Operand missing !! \n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            program_memory->program[program_memory->program_size] = operand;
            program_memory->program_size++;
        }
        // printf("Opcode = %d\n", opcode);
        // if (instruction_size(opcode) == 2){
        //     printf("Operand = %d\n", operand);
        // }
    }
    if (ferror(file)) {
        fprintf(stderr, "Error reading bytecode file\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
   
    
    if (fclose(file) == EOF) {
        fprintf(stderr, "Failed to close bytecode file\n");
        exit(EXIT_FAILURE);
    }
}