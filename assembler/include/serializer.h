#ifndef MYLANGVM_ASSEMBLER_SERIALIZER_H
#define MYLANGVM_ASSEMBLER_SERIALIZER_H
#include "instruction.h"
#include "opcode_table.h"
bool serialize(const Program *program, const char *filename);


#endif