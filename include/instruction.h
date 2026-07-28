#ifndef INSTRUCTION_H
#define INSTRUCTION_H

// Creating enumration, same as [#define] but [enum] auto-indexes the elements and replaces the words same as like [#define] do
typedef enum{
    PSH, // Pushes a number to the stack
    ADD, // Takes the last two numbers from stack, pops them one by one, add them and push the result back in stack
    SUB, // Takes the last two numbers from stack, pop them one by one, subtracts them and push the result back in stack
    MUL, // Takes the last two numbers from stack, pop them one by one, multiplies them and push the result back in stack
    DIV, // Takes the last two numbers from stack, pop them one by one, divide them and push the result back in stack
    DUP, // Duplicates the top element and pushes it on top
    SWP, // Swap the top two values
    MOD, // Finds the modulo of two numbers and pushes the result back to the stack
    NEG, // Negate -> Makes the top element negative
    POS, // Makes the top element positive
    POP, // Pops the last element of the stack
    HLT // Ends the program
}Instruction;

#endif