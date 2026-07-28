CC = clang
CFLAGS = -Wall -Wextra -g -std=c11 -Iinclude

SRC = src/main.c \
      src/vm.c \
      src/stack.c \
	  src/instruction_handlers.c

TARGET = mylangvm

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)