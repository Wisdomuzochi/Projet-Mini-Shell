CC = gcc
CFLAGS = -Wall -Wextra -g

OBJ = mishell.o parser.o execute.o

all: mishell

mishell: $(OBJ)
	$(CC) $(CFLAGS) -o mishell $(OBJ)

mishell.o: mishell.c parser.h execute.h
	$(CC) $(CFLAGS) -c mishell.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

execute.o: execute.c execute.h
	$(CC) $(CFLAGS) -c execute.c

clean:
	rm -f mishell $(OBJ)