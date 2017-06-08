CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror
EXE1 = procesar_tweets
EXE2 = procesar_usuarios


all: p1 p2

p1: cbf procesar_tweets.o lista.o heap.o pila.o
	$(CC) $(CFLAGS) -o $(EXE1) procesar_tweets.o cbf.o lista.o heap.o pila.o

p2: hash procesar_usuarios.o lista.o heap.o
	$(CC) $(CFLAGS) -o $(EXE2) procesar_usuarios.o hash.o lista.o heap.o

hash:
	$(CC) $(CFLAGS) hash.c -c

cbf:
	$(CC) $(CFLAGS) cbf.c -c

procesar_usuarios.o: 
	$(CC) $(CFLAGS) procesar_usuarios.c -c

procesar_tweets.o: 
	$(CC) $(CFLAGS) procesar_tweets.c -c

lista.o:
	$(CC) $(CFLAGS) lista.c -c

heap.o: 
	$(CC) $(CFLAGS) heap.c -c

clean:
	rm $(EXE2) $(EXE1) *.o

run: all
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./procesar_usuarios tweets
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes cat tweets | ./procesar_tweets 50 5
