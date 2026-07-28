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
void stack_init(Stack *stack){
    stack->top = -1;
}

// Fucntion to check if stack is empty
bool stack_is_empty(Stack *stack){
    return stack->top == -1;
}

// Function to check if stack is full
bool stack_is_full(Stack *stack){
    return stack->top >= MAX_STACK_SIZE - 1;
}

// Function to push some value to the stack
void stack_push(Stack *stack, int value){
    if(stack_is_full(stack)){
        fprintf(stderr,"Stack Overflow ! ");
        exit(EXIT_FAILURE);
    }
    printf("The element is pushed : %d\n",value);
    stack->arr[++stack->top] = value;
}

// Function to pop some value form the stack and it also prints the value for debugging 
int stack_pop(Stack *stack){
    if(stack_is_empty(stack)){
        fprintf(stderr,"Stack Underflow ! ");
        exit(EXIT_FAILURE);
    }
    int value = stack->arr[stack->top];
    printf("The element is popped : %d\n",value);
    stack->top--;
    return value;
}

// Function to look at the top most element
int stack_peek(Stack *stack){
    if(stack_is_empty(stack)){
        fprintf(stderr,"Stack Underflow ! ");
        exit(EXIT_FAILURE);
    }
    return stack->arr[stack->top];
}


// Now begins the implementation of my own language virtual machine



// Defining a structure for VM separately, which will allow us to run multiple VM instances
typedef struct{
    Stack stack;
    size_t pc;
    bool running;
    size_t programLength;
    const int *program;
}VM;


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


void vm_init(VM *vm, const int *program, size_t programLength){
    vm->pc = 0;
    vm->running = true;
    vm->programLength = programLength;
    vm->program = program;
    stack_init(&vm->stack);
}

void vm_step(VM *vm){
    if(vm->pc >= vm->programLength){
        fprintf(stderr,"Program counter out of bounds !! ");
        exit(EXIT_FAILURE);
    }
    vm_execute_instruction(vm,vm_fetch_instruction(vm));
    vm->pc++;
}
void vm_run(VM *vm){
    while(vm->running){
        vm_step(vm);
    };
}


//Our main function where we initialize our stack and run the vm
int main(){
    VM vm1;
    vm_init(&vm1, program, sizeof(program)/sizeof(program[0]));
    vm_run(&vm1);
    return 0;
}