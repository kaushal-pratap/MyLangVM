#include<stdio.h> 
#include<stdbool.h>
#include<stdlib.h>

// Defining the maximum size of stack
#define MAX_STACK_SIZE 100 

// Creating a structure of stack data structure with aliases Stack
typedef struct{
    int arr[MAX_STACK_SIZE];
    int top;
}Stack; 

// Function to initialize the stack, it takes pointer of Stack structure data type as input parameter
void initialize(Stack *stack){
    stack->top = -1;
}

// Fucntion to check if stack is empty
bool isEmpty(Stack *stack){
    return stack->top == -1;
}

// Function to check if stack is full
bool isFull(Stack *stack){
    return stack->top >= MAX_STACK_SIZE - 1;
}

// Function to push some value to the stack
void push(Stack *stack, int value){
    if(isFull(stack)){
        fprintf(stderr,"Stack Overflow ! ");
        exit(EXIT_FAILURE);
    }
    printf("The element is pushed : %d\n",value);
    stack->arr[++stack->top] = value;
}

// Function to pop some value form the stack and it also prints the value for debugging 
int pop(Stack *stack){
    if(isEmpty(stack)){
        fprintf(stderr,"Stack Underflow ! ");
        exit(EXIT_FAILURE);
    }
    int value = stack->arr[stack->top];
    printf("The element is popped : %d\n",value);
    stack->top--;
    return value;
}

// Function to look at the top most element
int peek(Stack *stack){
    if(isEmpty(stack)){
        fprintf(stderr,"Stack Underflow ! ");
        exit(EXIT_FAILURE);
    }
    return stack->arr[stack->top];
}


// Now begins the implementation of my own language virtual machine

// Defining program counter
int pc = 0;

// Defining if program is running or not for [HLT]
bool running = true;

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

// Function to fetch the current instruction
Instruction fetch(){
    return program[pc];
}

// Function to execute the instructions
void eval(Instruction instr, Stack *stack){
    switch(instr){
        case HLT: {
            running = false;
            break;
        }
        case PSH: {
            push(stack,program[++pc]);
            break;
        }
        case ADD: {
            int rhs = pop(stack);
            int lhs = pop(stack);
            push(stack,lhs+rhs);
            break;
        }
        case SUB: {
            int rhs = pop(stack);
            int lhs = pop(stack);
            push(stack,lhs-rhs);
            break;
        }
        case MUL: {
            int rhs = pop(stack);
            int lhs = pop(stack);
            push(stack,lhs*rhs);
            break;
        }
        case DIV: {
            int rhs = pop(stack);
            int lhs = pop(stack);
            if(rhs == 0){
                push(stack,lhs);
                push(stack,rhs);
                fprintf(stderr,"Division by zero !! ");
                running = false;
                break;
            }
            push(stack,lhs/rhs);
            break;
        }
        case DUP: {
            int duplicate = peek(stack);
            push(stack,duplicate);
            break;
        }
        case SWP: {
            int rhs = pop(stack);
            int lhs = pop(stack);
            push(stack,rhs);
            push(stack,lhs);
            break;
        }
        case MOD: {
            int rhs = pop(stack);
            int lhs = pop(stack);
            if(rhs == 0){
                push(stack,lhs);
                push(stack,rhs);
                fprintf(stderr,"Modulo by zero !! ");
                running = false;
                break;
            }
            push(stack,lhs%rhs);
            break;
        }
        case NEG: {
            if(peek(stack) > 0){
                int num = pop(stack);
                push(stack,(-1)*num);
                break;
            }
            break;
        }
        case POS: {
            if(peek(stack) < 0){
                int num = pop(stack);
                push(stack,(-1)*num);
                break;
            }
            break;
        }
        case POP: {
            pop(stack);
            break;
        }
        default:
        fprintf(stderr,"Unknown instruction: %d\n", instr);
        running = false;
        break;
    }
    
}

//Our main function where we initialize our stack and run the vm
int main(){
    Stack stack;
    initialize(&stack);
    int programLength = sizeof(program)/sizeof(int);
    
    while(running){
        if(pc >= programLength){
        fprintf(stderr,"Program counter out of bounds !! ");
        exit(EXIT_FAILURE);
    }
        eval(fetch(),&stack);
        pc++;
    };
    return 0;
}