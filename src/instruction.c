#include "instruction.h"
size_t instruction_size(Instruction instr){
    switch(instr){
        case PSH:
        case JMP:
        case JZ:
        case JNZ: 
        return 2;

        default:
        return 1;
    }
}

bool is_pc_modified(Instruction instr){
    switch(instr){
        case JMP:
        case JZ:
        case JNZ:
        return true;

        default:
        return false;
    }
}