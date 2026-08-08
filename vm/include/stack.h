#ifndef STACK_H
#define STACK_H

#include<stddef.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include <stdint.h>

// Defining the maximum size of stack
#define MAX_STACK_SIZE 100

// Creating a structure of stack data structure with aliases Stack
typedef struct{
    int32_t arr[MAX_STACK_SIZE];
    int top;
}Stack; 

// Function to initialize the stack, it takes pointer of Stack structure data type as input parameter
void stack_init(Stack *stack);

// Fucntion to check if stack is empty
bool stack_is_empty(const Stack *stack);

// Function to check if stack is full
bool stack_is_full(const Stack *stack);

// Function to push some value to the stack
void stack_push(Stack *stack, int32_t value);

// Function to pop some value form the stack and it also prints the value for debugging 
int32_t stack_pop(Stack *stack);

// Function to look at the top most element
int32_t stack_peek(const Stack *stack);

#endif