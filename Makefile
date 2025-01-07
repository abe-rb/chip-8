CC = gcc
CFLAGS = -Wall -std=c99 -g
LDLFLAGS = -lSDL2

SRC = main.c chip8.c
OBJ = $(SRC:.c=.o)
EXEC = chip8

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDLFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
