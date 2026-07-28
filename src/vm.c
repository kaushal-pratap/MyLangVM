#include "vm.h"

// Function to fetch the current instruction
Instruction vm_fetch_instruction(VM *vm){
    return vm->program[vm->pc];
}

// Function to execute the instructions
void vm_execute_instruction(VM *vm, Instruction instr){
    switch(instr){
        case HLT: {
            vm->running = false;
            break;
        }
        case PSH: {
            stack_push(&vm->stack,vm->program[++vm->pc]);
            break;
        }
        case ADD: {
            int rhs = stack_pop(&vm->stack);
            int lhs = stack_pop(&vm->stack);
            stack_push(&vm->stack,lhs+rhs);
            break;
        }
        case SUB: {
            int rhs = stack_pop(&vm->stack);
            int lhs = stack_pop(&vm->stack);
            stack_push(&vm->stack,lhs-rhs);
            break;
        }
        case MUL: {
            int rhs = stack_pop(&vm->stack);
            int lhs = stack_pop(&vm->stack);
            stack_push(&vm->stack,lhs*rhs);
            break;
        }
        case DIV: {
            int rhs = stack_pop(&vm->stack);
            int lhs = stack_pop(&vm->stack);
            if(rhs == 0){
                stack_push(&vm->stack,lhs);
                stack_push(&vm->stack,rhs);
                fprintf(stderr,"Division by zero !! ");
                vm->running = false;
                break;
            }
            stack_push(&vm->stack,lhs/rhs);
            break;
        }
        case DUP: {
            int duplicate = stack_peek(&vm->stack);
            stack_push(&vm->stack,duplicate);
            break;
        }
        case SWP: {
            int rhs = stack_pop(&vm->stack);
            int lhs = stack_pop(&vm->stack);
            stack_push(&vm->stack,rhs);
            stack_push(&vm->stack,lhs);
            break;
        }
        case MOD: {
            int rhs = stack_pop(&vm->stack);
            int lhs = stack_pop(&vm->stack);
            if(rhs == 0){
                stack_push(&vm->stack,lhs);
                stack_push(&vm->stack,rhs);
                fprintf(stderr,"Modulo by zero !! ");
                vm->running = false;
                break;
            }
            stack_push(&vm->stack,lhs%rhs);
            break;
        }
        case NEG: {
            if(stack_peek(&vm->stack) > 0){
                int num = stack_pop(&vm->stack);
                stack_push(&vm->stack,-num);
                break;
            }
            break;
        }
        case POS: {
            if(stack_peek(&vm->stack) < 0){
                int num = stack_pop(&vm->stack);
                stack_push(&vm->stack,-num);
                break;
            }
            break;
        }
        case POP: {
            stack_pop(&vm->stack);
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