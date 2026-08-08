#include "instruction_handlers.h"

// Function to fetch the current instruction
VmInstruction vm_fetch_instruction(VM *vm){
    return (VmInstruction)vm->program[vm->pc];
}

// Function to execute the instructions
void vm_execute_instruction(VM *vm, VmInstruction instr){
    switch(instr){
        case HLT: {
            execute_hlt(vm);
            break;
        }
        case PSH: {
            execute_psh(vm);
            break;
        }
        case ADD: {
            execute_add(vm);
            break;
        }
        case SUB: {
            execute_sub(vm);
            break;
        }
        case MUL: {
            execute_mul(vm);
            break;
        }
        case DIV: {
            execute_div(vm);
            break;
        }
        case DUP: {
            execute_dup(vm);
            break;
        }
        case SWP: {
            execute_swp(vm);
            break;
        }
        case MOD: {
            execute_mod(vm);
            break;
        }
        case NEG: {
            execute_neg(vm);
            break;
        }
        case POS: {
            execute_pos(vm);
            break;
        }
        case GT: {
            execute_gt(vm);
            break;
        }
        case GE: {
            execute_ge(vm);
            break;
        }
        case EQ: {
            execute_eq(vm);
            break;
        }
        case NE: {
            execute_ne(vm);
            break;
        }
        case LT: {
            execute_lt(vm);
            break;
        }
        case LE: {
            execute_le(vm);
            break;
        }
        case JMP: {
            execute_jmp(vm);
            break;
        }
        case JZ: {
            execute_jz(vm);
            break;
        }
        case JNZ: {
            execute_jnz(vm);
            break;
        }
        case CALL: {
            execute_call(vm);
            break;
        }
        case RET: {
            execute_ret(vm);
            break;
        }
        case STORE: {
            execute_store(vm);
            break;
        }
        case LOAD: {
            execute_load(vm);
            break;
        }
        case INPT: {
            execute_inpt(vm);
            break;
        }
        case PRNT: {
            execute_prnt(vm);
            break;
        }
        case POP: {
            execute_pop(vm);
            break;
        }
        default:
        fprintf(stderr,"Unknown instruction: %d\n", instr);
        vm->running = false;
        break;
    }
    
}
static void build_instruction_map(VM *vm, const int32_t *program, size_t programLength){
    vm->instructionCount = 0;
    size_t i = 0;
    while(i < programLength){
        vm->instructionCount++;
        i += instruction_size(program[i]);
    }
    vm->instructionMapArray = malloc(vm->instructionCount*sizeof(size_t));
    if (vm->instructionMapArray == NULL) {
        fprintf(stderr, "Failed to allocate instruction map\n");
        exit(EXIT_FAILURE);
    }
    size_t instruction_index = 0;
    size_t program_index = 0;

    while(program_index < programLength){
        vm->instructionMapArray[instruction_index] = program_index;
        program_index += instruction_size(program[program_index]);
        instruction_index++;
    }
}

// Function to initialize our VM
void vm_init(VM *vm, const int32_t *program, size_t programLength){
    vm->pc = 0;
    vm->running = true;
    vm->programLength = programLength;
    vm->program = program;
    stack_init(&vm->operandStack);
    stack_init(&vm->callStack);
    build_instruction_map(vm, program, programLength);
}


// Function to call each VM step individually 
void vm_step(VM *vm){
    if(vm->pc >= vm->programLength){
        fprintf(stderr,"Program terminated unexpectedly: missing HLT instruction?\n");
        vm->running = false;
        return;
    }
    VmInstruction instr = vm_fetch_instruction(vm);
    vm_execute_instruction(vm,instr);
    if(!is_pc_modified(instr)){
        vm->pc++;
    }
}

// Function to run the VM
void vm_run(VM *vm){
    while(vm->running){
        vm_step(vm);
    };
}

void vm_destroy(VM *vm) {
    free(vm->instructionMapArray);
    vm->instructionMapArray = NULL;
    vm->instructionCount = 0;
}