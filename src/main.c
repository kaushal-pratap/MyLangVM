#include "vm.h"

// Defining our actual program array, the instructions to execute
const int program[] = {
        PSH, 5,
        PSH, 6,
        ADD,
        PSH, 9,
        ADD,
        PSH, 30,
        SUB,
        PSH, 10,
        MUL,
        PSH, 2,
        DIV,
        PSH, 4,
        DUP,
        PSH, 8,
        SWP,
        MOD,
        NEG,
        POP,
        NEG,
        POS,
        POP,
        HLT
    };

//Our main function where we initialize our stack and run the vm
int main(){
    VM vm1;
    vm_init(&vm1, program, sizeof(program)/sizeof(program[0]));
    vm_run(&vm1);
    return 0;
}