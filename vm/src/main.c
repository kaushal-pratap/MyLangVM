#include "vm.h"
#include "loader.h"

int main(int argc, char *argv[]){
    ProgramMemory program_memory = {0};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bytecode>\n", argv[0]);
        return 1;
    }
    load_program(&program_memory, argv[1]);
    
    VM vm1;
    vm_init(&vm1, program_memory.program , program_memory.program_size);
    vm_run(&vm1);
    vm_destroy(&vm1);
    return 0;
}