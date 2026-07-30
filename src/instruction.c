#include "instruction.h"
size_t instruction_size(Instruction instr){
    switch(instr){
        case PSH:
        case JMP:
        case JZ:
        case JNZ: 
        case CALL:
        case STORE:
        case LOAD:
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
        case CALL:
        case RET:
        return true;

        default:
        return false;
    }
}