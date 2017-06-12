CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror
EXE1 = procesar_tweets
EXE2 = procesar_usuarios


all: p1 p2

p1: auxlib.o countingfilter.o procesar_tweets.o heap.o pila.o hash.o lista.o
	$(CC) $(CFLAGS) -o $(EXE1) procesar_tweets.o countingfilter.o pila.o heap.o hash.o lista.o auxlib.o

p2: hash.o procesar_usuarios.o lista.o heap.o
	$(CC) $(CFLAGS) -o $(EXE2) procesar_usuarios.o hash.o lista.o heap.o

pila.o:
	$(CC) $(CFLAGS) pila.c -c

hash.o:
	$(CC) $(CFLAGS) hash.c -c

countingfilter.o:
	$(CC) $(CFLAGS) countingfilter.c -c

procesar_usuarios.o: 
	$(CC) $(CFLAGS) procesar_usuarios.c -c

auxlib.o:
	$(CC) $(CFLAGS) auxlib.c -c

procesar_tweets.o: 
	$(CC) $(CFLAGS) procesar_tweets.c -c

lista.o:
	$(CC) $(CFLAGS) lista.c -c

heap.o: 
	$(CC) $(CFLAGS) heap.c -c

clean:
	rm -f $(EXE2) $(EXE1) *.o