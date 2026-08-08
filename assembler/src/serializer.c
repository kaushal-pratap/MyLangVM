#include "serializer.h"
#include <stdio.h>

#define OPCODE_MAP_COUNT (sizeof(opcode_map) / sizeof(opcode_map[0]))

typedef struct OpcodeMap{
    Opcode assembler_opcode;
    VmInstruction vm_opcode;

} OpcodeMap;

static const OpcodeMap opcode_map[] = {
    {OP_PUSH,               PSH},
    {OP_ADD,                ADD},
    {OP_SUBTRACT,           SUB},
    {OP_MULTIPLY,           MUL},
    {OP_DIVIDE,             DIV},
    {OP_DUPLICATE,          DUP},
    {OP_SWAP,               SWP},
    {OP_MODULO,             MOD},
    {OP_NEGATIVE,           NEG},
    {OP_POSITIVE,           POS},
    {OP_GREATER_THAN,       GT},
    {OP_GREATER_EQUAL,      GE},
    {OP_EQUAL,              EQ},
    {OP_NOT_EQUAL,          NE},
    {OP_LESS_THAN,          LT},
    {OP_LESS_EQUAL,         LE},
    {OP_JUMP,               JMP},
    {OP_JUMP_IF_ZERO,       JZ},
    {OP_JUMP_IF_NOT_ZERO,   JNZ},
    {OP_CALL,               CALL},
    {OP_RETURN,             RET},
    {OP_STORE,              STORE},
    {OP_LOAD,               LOAD},
    {OP_INPUT,              INPT},
    {OP_PRINT,              PRNT},
    {OP_POP,                POP},
    {OP_HALT,               HLT}
};
static VmInstruction assembler_opcode_to_vm_opcode(Opcode assembler_opcode){
    for(size_t index = 0; index < OPCODE_MAP_COUNT; index++){
        if(opcode_map[index].assembler_opcode == assembler_opcode){
            return opcode_map[index].vm_opcode;
        }
    }
    fprintf(stderr, "Internal error: unmapped opcode %d\n", assembler_opcode);
    exit(EXIT_FAILURE);
}

// Code to convert our bytecode into little endian compatible bytecode

static bool write_int32_le(FILE *file, int32_t value){
    uint32_t bits = (uint32_t)value;
    uint8_t bytes[4];
    bytes[0] = (uint8_t)(bits & 0xFF);
    bytes[1] = (uint8_t)((bits >> 8) & 0xFF);
    bytes[2] = (uint8_t)((bits >> 16) & 0xFF);
    bytes[3] = (uint8_t)((bits >> 24) & 0xFF);
    return fwrite(bytes, sizeof(uint8_t), 4, file) == 4;
}


bool serialize(const Program *program, const char *filename){
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Could not create bytecode.mylangvm\n");
        exit(EXIT_FAILURE);
    }
    for(size_t index = 0; index < program->instruction_count; index++){
        uint8_t vm_opcode = (uint8_t)assembler_opcode_to_vm_opcode(program->instructions[index].opcode);
 
        if (fwrite(&vm_opcode,sizeof(uint8_t),1,file) != 1) {
            fprintf(stderr, "Failed to write opcode !!\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        if(program->instructions[index].has_operand){
            if (!write_int32_le(file, program->instructions[index].operand)) {
                fprintf(stderr, "Failed to write operand !!\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }
    if (fclose(file) == EOF) {
        fprintf(stderr, "Failed to close output file\n");
        return false;
    }

    return true;
}