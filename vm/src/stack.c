#include "stack.h"

// Function to initialize the stack, it takes pointer of Stack structure data type as input parameter
void stack_init(Stack *stack){
    stack->top = -1;
}

// Fucntion to check if stack is empty
bool stack_is_empty(const Stack *stack){
    return stack->top == -1;
}

// Function to check if stack is full
bool stack_is_full(const Stack *stack){
    return stack->top >= MAX_STACK_SIZE - 1;
}

// Function to push some value to the stack
void stack_push(Stack *stack, int32_t value){
    if(stack_is_full(stack)){
        fprintf(stderr,"Stack Overflow ! ");
        exit(EXIT_FAILURE);
    }
    // printf("The element is pushed : %d\n",value);
    stack->arr[++stack->top] = value;
}

// Function to pop some value form the stack and it also prints the value for debugging 
int32_t stack_pop(Stack *stack){
    if(stack_is_empty(stack)){
        fprintf(stderr,"Stack Underflow ! ");
        exit(EXIT_FAILURE);
    }
    int32_t value = stack->arr[stack->top];
    // printf("The element is popped : %d\n",value);
    stack->top--;
    return value;
}

// Function to look at the top most element
int32_t stack_peek(const Stack *stack){
    if(stack_is_empty(stack)){
        fprintf(stderr,"Stack Underflow ! ");
        exit(EXIT_FAILURE);
    }
    return stack->arr[stack->top];
}
