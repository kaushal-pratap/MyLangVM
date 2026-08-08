CC ?= cc

CFLAGS = -Wall -Wextra -g -std=c11 \
	-Iinclude \
	-I../common \
	-I../assembler/include

SRC = src/main.c \
	src/vm.c \
	src/stack.c \
	src/instruction_handlers.c \
	src/instruction.c \
	src/loader.c

TARGET = ../build/vm/mylangvm

all: directories

directories:
	mkdir -p ../build/vm
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)