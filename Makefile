CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror
TDA = cbf
EXE = procesar_tweets


all: tda main.o lista.o heap.o
	$(CC) $(CFLAGS) -o $(EXE) main.o $(TDA).o lista.o heap.o

tda:
	$(CC) $(CFLAGS) $(TDA).c -c

main.o: 
	$(CC) $(CFLAGS) main.c -c

lista.o:
	$(CC) $(CFLAGS) lista.c -c

heap.o: 
	$(CC) $(CFLAGS) heap.c -c

clean:
	rm $(EXE) *.o

run: all
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes cat tweets.txt | ./pruebas 900000 10
