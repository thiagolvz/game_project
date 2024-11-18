CC = gcc
CFLAGS = -Wall -lncurses
OBJ = main.o game.o snake.o board.o food.o

snake_game: $(OBJ)
	$(CC) -o snake_game $(OBJ) $(CFLAGS)

main.o: main.c game.h snake.h board.h food.h
	$(CC) -c main.c $(CFLAGS)

game.o: game.c game.h snake.h board.h food.h
	$(CC) -c game.c $(CFLAGS)

snake.o: snake.c snake.h
	$(CC) -c snake.c $(CFLAGS)

board.o: board.c board.h
	$(CC) -
