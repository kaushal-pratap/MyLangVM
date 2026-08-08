#include "loader.h"

static bool read_int32_le(FILE *file, int32_t *value) {
    uint8_t bytes[4];

    if (fread(bytes, sizeof(uint8_t), 4, file) != 4) {
        return false;
    }
    
    uint32_t bits =
        ((uint32_t)bytes[0]) |
        ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) |
        ((uint32_t)bytes[3] << 24);

    *value = (int32_t)bits;

    return true;
}

void load_program(ProgramMemory *program_memory, const char *filename){
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open bytecode.mylangvm\n");
        exit(EXIT_FAILURE);
    }

    uint8_t opcode_byte;
    int32_t operand;

    while(fread(&opcode_byte, sizeof(uint8_t), 1, file) == 1){
        VmInstruction opcode = (VmInstruction)opcode_byte;
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
            if (!read_int32_le(file, &operand)) {
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