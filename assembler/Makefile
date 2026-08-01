CC = clang
CFLAGS = -Wall -Wextra -g -std=c11 \
		-Iinclude \
		-I../common

SRC = src/main.c \
      src/tokenizer.c \
	  src/opcode_table.c

TARGET = mylangasm

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)