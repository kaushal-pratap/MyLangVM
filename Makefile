CC = clang
CFLAGS = -Wall -Wextra -g -std=c11

SRC = src/main.c

TARGET = mylangvm

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)