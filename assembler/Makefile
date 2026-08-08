CC = clang

CFLAGS = -Wall -Wextra -g -std=c11 \
	-Iinclude \
	-I../common

SRC = src/main.c \
	src/tokenizer.c \
	src/opcode_table.c \
	src/parser.c \
	src/serializer.c

TARGET = ../build/assembler/mylangasm

all: directories

directories:
	mkdir -p ../build/assembler
	mkdir -p ../build/bytecode
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f ../build/bytecode/*