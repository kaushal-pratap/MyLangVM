#ifndef INSTRUCTION_HANDLERS_H
#define INSTRUCTION_HANDLERS_H

#include "vm.h"

void execute_psh(VM *vm);
void execute_add(VM *vm);
void execute_sub(VM *vm);
void execute_mul(VM *vm);
void execute_div(VM *vm);
void execute_dup(VM *vm);
void execute_swp(VM *vm);
void execute_mod(VM *vm);
void execute_neg(VM *vm);
void execute_pos(VM *vm);
void execute_gt(VM *vm);
void execute_ge(VM *vm);
void execute_eq(VM *vm);
void execute_ne(VM *vm);
void execute_lt(VM *vm);
void execute_le(VM *vm);
void execute_pop(VM *vm);
void execute_hlt(VM *vm);

#endif