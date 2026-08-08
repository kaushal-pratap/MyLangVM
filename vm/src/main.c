#include "vm.h"
#include "loader.h"

int main(){
    ProgramMemory program_memory = {0};
    load_program(&program_memory, "../assembler/bytecode.mylangvm");
    VM vm1;
    vm_init(&vm1, program_memory.program , program_memory.program_size);
    vm_run(&vm1);
    return 0;
}