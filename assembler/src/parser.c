#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include <ctype.h>
#include "opcode_table.h"
#include "instruction.h"
#include <limits.h>

static const Token *consume_integer(const Token *token_array, size_t token_array_count, size_t *current){
    if(*current+1 >= token_array_count){
        fprintf(stderr,"Error: Integer missing \nLine %zu, Column %zu\n",token_array[*current].line, token_array[*current].column);
        exit(EXIT_FAILURE);
    }
    if(token_array[*current+1].type != TOKEN_INTEGER){
        fprintf(stderr,"Error: Expected integer \nLine %zu, Column %zu\n",token_array[*current].line, token_array[*current].column);
        exit(EXIT_FAILURE);
    }
    (*current)++;
    return &token_array[*current];
}

static int token_to_int(const Token *token){
    long long value = 0;
    size_t index = 0;
    const char *current = token->start;
    int sign = 1;

    if(*current == '-'){
        sign = -1;
        index = 1;
        current++;
    }
    for(; index < token->length; index++){
        int digit = *current - '0';
        value = value * 10 + digit;
        current++;
    }
    value *= sign;
    if (value > INT_MAX || value < INT_MIN) {
        fprintf(stderr,"Error: Maximum instruction limit (%d) exceeded.\n",MAX_INSTRUCTION_SIZE);
        exit(EXIT_FAILURE);
    }
    return (int)value;

}

static void push_instruction(Program *program, Opcode opcode, int operand, bool has_operand){
    if(program->instruction_count >= MAX_INSTRUCTION_SIZE){
        fprintf(stderr,"Instructions overflow !!");
        exit(EXIT_FAILURE);
    }
    program->instructions[program->instruction_count].opcode = opcode;
    program->instructions[program->instruction_count].operand = operand;
    program->instructions[program->instruction_count].has_operand = has_operand;
    program->instruction_count++;
}

static void push_instruction_with_operand(Program *program, Opcode opcode, const Token *token_array, size_t token_count, size_t *current){
    const Token *token = consume_integer(token_array, token_count, current);
    int operand = token_to_int(token);
    push_instruction(program,opcode, operand, true);
}

void parse(const Token* token_array, size_t token_count, Program *program){

    size_t current = 0;
    while(current < token_count){
        Opcode opcode = lookup_opcode(&token_array[current]);
        switch (opcode)
        {
            case OP_PUSH: 
            case OP_JUMP:
            case OP_JUMP_IF_ZERO:
            case OP_JUMP_IF_NOT_ZERO: 
            case OP_CALL:
            case OP_STORE:
            case OP_LOAD: 
            {
                push_instruction_with_operand(program,opcode, token_array, token_count, &current);
                break;
            }


            case OP_ADD:
            case OP_SUBTRACT:
            case OP_MULTIPLY:
            case OP_DIVIDE: 
            case OP_DUPLICATE:
            case OP_SWAP: 
            case OP_MODULO: 
            case OP_NEGATIVE: 
            case OP_POSITIVE:
            case OP_GREATER_THAN: 
            case OP_GREATER_EQUAL: 
            case OP_EQUAL: 
            case OP_NOT_EQUAL:
            case OP_LESS_THAN:
            case OP_LESS_EQUAL: 
            case OP_RETURN: 
            case OP_INPUT: 
            case OP_PRINT: 
            case OP_POP: 
            case OP_HALT:
            {
                push_instruction(program,opcode, 0, false);
                break;
            }

            case (OP_INVALID): {
                fprintf(stderr,"Unknown instruction\n"
                "Line %zu, Column %zu\n",
                token_array[current].line,
                token_array[current].column);
                exit(EXIT_FAILURE);
            }
            default: {
                fprintf(stderr, "Internal parser error\n");
                exit(EXIT_FAILURE);
            }
            
        }
        current++;
    }
}