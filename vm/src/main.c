#include "vm.h"

// Defining our actual program array, the instructions to execute
const int program[] = {
        // PSH, 5,
        // PSH, 6,
        // ADD,
        // PSH, 9,
        // ADD,
        // PSH, 30,
        // SUB,
        // PSH, 10,
        // MUL,
        // PSH, 2,
        // DIV,
        // PSH, 4,
        // DUP,
        // PSH, 8,
        // SWP,
        // MOD,
        // NEG,
        // POP,
        // NEG,
        // POS,
        // POP,
        // HLT

        // PSH, 5,
        // PSH, 10,
        // GT,
        // PSH, 3,
        // PSH, 2,
        // GE,
        // PSH, 4,
        // PSH, 4,
        // EQ,
        // PSH, 9,
        // PSH, 100,
        // NE,
        // PSH, 2,
        // PSH, 10,
        // LT,
        // PSH, 50,
        // PSH, 80,
        // LE,
        // HLT

        // PSH, 0,
        // PSH,5,
        // PSH,6,
        // PSH,7,
        // PSH,8,
        // HLT

        // PSH,1000,
        // PSH,2000,
        // CALL,6,
        // CALL,10,
        // CALL,14,
        // HLT,

        // PSH,3000,
        // PSH,4000,
        // PSH,5000,
        // RET,

        // PSH,6000,      
        // PSH,7000,
        // PSH,8000,
        // RET,

        // PSH,9000,
        // PSH,10000,
        // RET
        // PSH,5,
        // CALL,3,
        // HLT,
        // CALL,5,
        // RET,
        // CALL,7,
        // RET,
        // PSH,10000,
        // RET
        // PSH,5,
        // STORE,0,
        // PSH,1,
        // STORE,1,
        // LOAD,0,
        // LOAD,1,
        // ADD,
        // STORE,2,
        INPT,
        INPT,
        ADD,
        PRNT,
        HLT
    };

//Our main function where we initialize our stack and run the vm
int main(){
    VM vm1;
    vm_init(&vm1, program, sizeof(program)/sizeof(program[0]));
    vm_run(&vm1);
    return 0;
}