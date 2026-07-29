#include "instruction_handlers.h"

// Function to fetch the current instruction
Instruction vm_fetch_instruction(VM *vm){
    return vm->program[vm->pc];
}

// Function to execute the instructions
void vm_execute_instruction(VM *vm, Instruction instr){
    switch(instr){
        case HLT: {
            execute_hlt(vm);
            break;
        }
        case PSH: {
            execute_psh(vm);
            vm->pc++;
            break;
        }
        case ADD: {
            execute_add(vm);
            vm->pc++;
            break;
        }
        case SUB: {
            execute_sub(vm);
            vm->pc++;
            break;
        }
        case MUL: {
            execute_mul(vm);
            vm->pc++;
            break;
        }
        case DIV: {
            execute_div(vm);
            vm->pc++;
            break;
        }
        case DUP: {
            execute_dup(vm);
            vm->pc++;
            break;
        }
        case SWP: {
            execute_swp(vm);
            vm->pc++;
            break;
        }
        case MOD: {
            execute_mod(vm);
            vm->pc++;
            break;
        }
        case NEG: {
            execute_neg(vm);
            vm->pc++;
            break;
        }
        case POS: {
            execute_pos(vm);
            vm->pc++;
            break;
        }
        case GT: {
            execute_gt(vm);
            vm->pc++;
            break;
        }
        case GE: {
            execute_ge(vm);
            vm->pc++;
            break;
        }
        case EQ: {
            execute_eq(vm);
            vm->pc++;
            break;
        }
        case NE: {
            execute_ne(vm);
            vm->pc++;
            break;
        }
        case LT: {
            execute_lt(vm);
            vm->pc++;
            break;
        }
        case LE: {
            execute_le(vm);
            vm->pc++;
            break;
        }
        case JMP: {
            execute_jmp(vm);
            break;
        }
        case JZ: {
            execute_jz(vm);
            break;
        }
        case JNZ: {
            execute_jnz(vm);
            break;
        }
        case POP: {
            execute_pop(vm);
            vm->pc++;
            break;
        }
        default:
        fprintf(stderr,"Unknown instruction: %d\n", instr);
        vm->running = false;
        break;
    }
    
}

void instructionMap(VM *vm, const int *program, size_t programLength){
    vm->instructionCount = 0;
   
    for(size_t i = 0; i < programLength; i++){
        if(program[i] == PSH){
            i += 1;
        }else if(program[i] == JMP){
            i += 1;
        }else if(program[i] == JZ){
            i += 1;
        }else if(program[i] == JNZ){
            i += 1;
        }
        vm->instructionCount += 1;
    }
    vm->instructionMapArray = malloc(vm->instructionCount*sizeof(size_t));
    size_t i = 0;
    for(size_t j = 0; j < vm->instructionCount; j++){
        vm->instructionMapArray[j] = i;
        if(program[i] == PSH){
            i+=2;
        }else if(program[i] == JMP){
            i+=2;
        }else if(program[i] == JZ){
            i+=2;
        }else if(program[i] == JNZ){
            i+=2;
        }else{
            i++;
        }
    }

}

// Function to initialize our VM
void vm_init(VM *vm, const int *program, size_t programLength){
    vm->pc = 0;
    vm->running = true;
    vm->programLength = programLength;
    vm->program = program;
    stack_init(&vm->stack);
    instructionMap(vm, program, programLength);
}


// Function to call each VM step individually 
void vm_step(VM *vm){
    if(vm->pc >= vm->programLength){
        fprintf(stderr,"Program counter out of bounds !! ");
        exit(EXIT_FAILURE);
    }
    vm_execute_instruction(vm,vm_fetch_instruction(vm));
}

// Function to run the VM
void vm_run(VM *vm){
    while(vm->running){
        vm_step(vm);
    };
}