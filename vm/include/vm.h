#ifndef VM_H
#define VM_H
#include "stack.h"
#include "instruction.h"

#define MEMORY_SIZE 1024
// Defining a structure for VM separately, which will allow us to run multiple VM instances
typedef struct{
    Stack operandStack;
    Stack callStack;
    int memory[MEMORY_SIZE];
    size_t pc;
    bool running;
    size_t programLength;
    const int *program;
    size_t instructionCount;
    size_t *instructionMapArray;
}VM;

// Function to fetch the current instruction
VmInstruction vm_fetch_instruction(VM *vm);

// Function to execute the instructions
void vm_execute_instruction(VM *vm, VmInstruction instr);

// Function to initialize our VM
void vm_init(VM *vm, const int *program, size_t programLength);

// Function to call each VM step individually 
void vm_step(VM *vm);

// Function to run the VM
void vm_run(VM *vm);

#endif