#include "instruction_handlers.h"

void execute_psh(VM *vm){
    stack_push(&vm->operandStack,vm->program[++vm->pc]);
}

void execute_add(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    stack_push(&vm->operandStack,lhs+rhs);
}

void execute_sub(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    stack_push(&vm->operandStack,lhs-rhs);
}

void execute_mul(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    stack_push(&vm->operandStack,lhs*rhs);
}

void execute_div(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(rhs == 0){
        stack_push(&vm->operandStack,lhs);
        stack_push(&vm->operandStack,rhs);
        fprintf(stderr,"Division by zero !! ");
        exit(EXIT_FAILURE);
    }
    stack_push(&vm->operandStack,lhs/rhs);
}

void execute_dup(VM *vm){
    int duplicate = stack_peek(&vm->operandStack);
    stack_push(&vm->operandStack,duplicate);
}

void execute_swp(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    stack_push(&vm->operandStack,rhs);
    stack_push(&vm->operandStack,lhs);
}

void execute_mod(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(rhs == 0){
        stack_push(&vm->operandStack,lhs);
        stack_push(&vm->operandStack,rhs);
        fprintf(stderr,"Modulo by zero !! ");
        exit(EXIT_FAILURE);
    }
    stack_push(&vm->operandStack,lhs%rhs);
}

void execute_neg(VM *vm){
    if(stack_peek(&vm->operandStack) > 0){
        int num = stack_pop(&vm->operandStack);
        stack_push(&vm->operandStack,-num);
    }
}
void execute_pos(VM *vm){
    if(stack_peek(&vm->operandStack) < 0){
        int num = stack_pop(&vm->operandStack);
        stack_push(&vm->operandStack,-num);
    }
}

void execute_gt(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(lhs > rhs){
        stack_push(&vm->operandStack,1);
    }else{
        stack_push(&vm->operandStack,0);
    }
}

void execute_ge(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(lhs >= rhs){
        stack_push(&vm->operandStack,1);
    }else{
        stack_push(&vm->operandStack,0);
    }
}

void execute_eq(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(lhs == rhs){
        stack_push(&vm->operandStack,1);
    }else{
        stack_push(&vm->operandStack,0);
    }
}

void execute_ne(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(lhs != rhs){
        stack_push(&vm->operandStack,1);
    }else{
        stack_push(&vm->operandStack,0);
    }
}

void execute_lt(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(lhs < rhs){
        stack_push(&vm->operandStack,1);
    }else{
        stack_push(&vm->operandStack,0);
    }
}

void execute_le(VM *vm){
    int rhs = stack_pop(&vm->operandStack);
    int lhs = stack_pop(&vm->operandStack);
    if(lhs <= rhs){
        stack_push(&vm->operandStack,1);
    }else{
        stack_push(&vm->operandStack,0);
    }
}

void execute_jmp(VM *vm){
    ++vm->pc;
    size_t target = vm->program[vm->pc];
    vm->pc = vm->instructionMapArray[target];
}

void execute_jz(VM *vm){
    if(stack_peek(&vm->operandStack) == 0){
        ++vm->pc;
        size_t target = vm->program[vm->pc];
        vm->pc = vm->instructionMapArray[target];
    }else{
        vm->pc+=2;
    }
    stack_pop(&vm->operandStack);
    
}

void execute_jnz(VM *vm){
    if(stack_peek(&vm->operandStack) != 0){
        ++vm->pc;
        size_t target = vm->program[vm->pc];
        vm->pc = vm->instructionMapArray[target];
    }else{
        vm->pc+=2;
    }
    stack_pop(&vm->operandStack);
}

void execute_pop(VM *vm){
    stack_pop(&vm->operandStack);
}

void execute_hlt(VM *vm){
    vm->running = false;
}