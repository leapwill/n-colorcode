CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c11 -I ./include
LDFLAGS =  -fsanitize=address -lncurses 

SRC = src/n-colorcode.c
DEPS = n-colorcode.h
OBJ = $(SRC:.c=.o)
EXEC = n-colorcode

bin/%.o: %.c $(DEPS)
	$(CC) -c -o obj/$@ $< $(CFLAGS)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o bin/$@ $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ) $(EXEC)
