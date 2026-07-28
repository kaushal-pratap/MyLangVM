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
            break;
        }
        case ADD: {
            execute_add(vm);
            break;
        }
        case SUB: {
            execute_sub(vm);
            break;
        }
        case MUL: {
            execute_mul(vm);
            break;
        }
        case DIV: {
            execute_div(vm);
            break;
        }
        case DUP: {
            execute_dup(vm);
            break;
        }
        case SWP: {
            execute_swp(vm);
            break;
        }
        case MOD: {
            execute_mod(vm);
            break;
        }
        case NEG: {
            execute_neg(vm);
            break;
        }
        case POS: {
            execute_pos(vm);
            break;
        }
        case GT: {
            execute_gt(vm);
            break;
        }
        case GE: {
            execute_ge(vm);
            break;
        }
        case EQ: {
            execute_eq(vm);
            break;
        }
        case NE: {
            execute_ne(vm);
            break;
        }
        case LT: {
            execute_lt(vm);
            break;
        }
        case LE: {
            execute_le(vm);
            break;
        }
        case POP: {
            execute_pop(vm);
            break;
        }
        default:
        fprintf(stderr,"Unknown instruction: %d\n", instr);
        vm->running = false;
        break;
    }
    
}

// Function to initialize our VM
void vm_init(VM *vm, const int *program, size_t programLength){
    vm->pc = 0;
    vm->running = true;
    vm->programLength = programLength;
    vm->program = program;
    stack_init(&vm->stack);
}


// Function to call each VM step individually 
void vm_step(VM *vm){
    if(vm->pc >= vm->programLength){
        fprintf(stderr,"Program counter out of bounds !! ");
        exit(EXIT_FAILURE);
    }
    vm_execute_instruction(vm,vm_fetch_instruction(vm));
    vm->pc++;
}

// Function to run the VM
void vm_run(VM *vm){
    while(vm->running){
        vm_step(vm);
    };
}