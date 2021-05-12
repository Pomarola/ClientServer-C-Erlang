ARG = -Wall -Wextra -Werror -g -std=c99 -pthread
BARRIER = ./Barrier/barrier
BOARD = ./Board/board
GAME = ./Game/game
TEMP = game.o barrier.o board.o simulador

all: simulador

simulador: simulador.c game.o board.o barrier.o
	gcc $(ARG) -o simulador simulador.c game.o board.o barrier.o -lm

game.o: $(GAME).c $(GAME).h
	gcc $(ARG) -c $(GAME).c 

barrier.o: $(BARRIER).c $(BARRIER).h
	gcc $(ARG) -c $(BARRIER).c 

board.o: $(BOARD).c $(BOARD).h
	gcc $(ARG) -c $(BOARD).c 

clean:
	-rm $(TEMP)

.PHONY : all
.PHONY : clean