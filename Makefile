CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror
TDA = hash
PRUEBAS = pruebas_alumno
EXE = pruebas


all: main.o lista.o heap.o testing.o tda
	$(CC) $(CFLAGS) -o $(EXE) $(TDA).o main.o lista.o heap.o testing.o

tda: main.o testing.o lista.o heap.o
	$(CC) $(CFLAGS) $(TDA).c -c

testing.o:
	$(CC) $(CFLAGS) testing.c -c

main.o:
	$(CC) $(CFLAGS) main.c -c

lista.o:
	$(CC) $(CFLAGS) lista.c -c

heap.o: 
	$(CC) $(CFLAGS) heap.c -c

clean:
	rm $(EXE) *.o

run: all
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./$(EXE) tweetsv2
