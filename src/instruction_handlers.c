#include "instruction_handlers.h"

void execute_psh(VM *vm){
    stack_push(&vm->stack,vm->program[++vm->pc]);
}

void execute_add(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    stack_push(&vm->stack,lhs+rhs);
}

void execute_sub(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    stack_push(&vm->stack,lhs-rhs);
}

void execute_mul(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    stack_push(&vm->stack,lhs*rhs);
}

void execute_div(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(rhs == 0){
        stack_push(&vm->stack,lhs);
        stack_push(&vm->stack,rhs);
        fprintf(stderr,"Division by zero !! ");
        exit(EXIT_FAILURE);
    }
    stack_push(&vm->stack,lhs/rhs);
}

void execute_dup(VM *vm){
    int duplicate = stack_peek(&vm->stack);
    stack_push(&vm->stack,duplicate);
}

void execute_swp(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    stack_push(&vm->stack,rhs);
    stack_push(&vm->stack,lhs);
}

void execute_mod(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(rhs == 0){
        stack_push(&vm->stack,lhs);
        stack_push(&vm->stack,rhs);
        fprintf(stderr,"Modulo by zero !! ");
        exit(EXIT_FAILURE);
    }
    stack_push(&vm->stack,lhs%rhs);
}

void execute_neg(VM *vm){
    if(stack_peek(&vm->stack) > 0){
        int num = stack_pop(&vm->stack);
        stack_push(&vm->stack,-num);
    }
}
void execute_pos(VM *vm){
    if(stack_peek(&vm->stack) < 0){
        int num = stack_pop(&vm->stack);
        stack_push(&vm->stack,-num);
    }
}

void execute_gt(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(lhs > rhs){
        stack_push(&vm->stack,1);
    }else{
        stack_push(&vm->stack,0);
    }
}

void execute_ge(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(lhs >= rhs){
        stack_push(&vm->stack,1);
    }else{
        stack_push(&vm->stack,0);
    }
}

void execute_eq(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(lhs == rhs){
        stack_push(&vm->stack,1);
    }else{
        stack_push(&vm->stack,0);
    }
}

void execute_ne(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(lhs != rhs){
        stack_push(&vm->stack,1);
    }else{
        stack_push(&vm->stack,0);
    }
}

void execute_lt(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(lhs < rhs){
        stack_push(&vm->stack,1);
    }else{
        stack_push(&vm->stack,0);
    }
}

void execute_le(VM *vm){
    int rhs = stack_pop(&vm->stack);
    int lhs = stack_pop(&vm->stack);
    if(lhs <= rhs){
        stack_push(&vm->stack,1);
    }else{
        stack_push(&vm->stack,0);
    }
}

void execute_pop(VM *vm){
    stack_pop(&vm->stack);
}

void execute_hlt(VM *vm){
    vm->running = false;
}