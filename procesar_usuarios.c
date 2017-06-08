#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "heap.h"
#include "lista.h"

#define MAX_ARGUMENTS 2

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/
typedef struct nodo_tmp {
	char* usuario;
	size_t* tweets;
} nodo_tmp_t;


int cmptweets(const void* a, const void* b) {
 	const nodo_tmp_t* aux1 = a;
 	const nodo_tmp_t* aux2 = b;

 	if (aux1->tweets > aux2->tweets)
 		return -1;
 	else if (aux1->tweets < aux2->tweets)
 		return 1;
 	else return 0;
}

void generarOutput(hash_t* hash, size_t max_dato) {
	lista_t** arraytmp = malloc(sizeof(lista_t*) * (max_dato + 1));
	if (!arraytmp) {
		printf("Memoria insuficiente. Abortando...\n");
		return;
	}
	for (size_t i = 0; i < max_dato + 1; i++) {
		arraytmp[i] = lista_crear();
		if (!arraytmp[i]) 
			printf("Error al crear lista %zu\n", i);
	}

	hash_iter_t* iter = hash_iter_crear(hash);
	if (!iter) {
		printf("Memoria insuficiente. Abortando...\n");
		free(arraytmp);
		return;
	}

	nodo_tmp_t* nodo;
	while (!hash_iter_al_final(iter)) {
		nodo = malloc(sizeof(nodo_tmp_t));
		nodo->usuario = hash_iter_ver_actual(iter);
		nodo->tweets = hash_obtener(hash, nodo->usuario);
		lista_insertar_ultimo(arraytmp[*nodo->tweets], nodo);
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);


	printf("************ESTADISTICA DE USUARIOS POR TWEETs*************");

	size_t tweets_ant = 0;
	for (size_t i = 0; i < max_dato + 1; i++) {
		while (!lista_esta_vacia(arraytmp[i])) {
			nodo = (nodo_tmp_t*) lista_borrar_primero(arraytmp[i]);
				if (*(nodo->tweets) != tweets_ant) {
					printf("\n%zu: ", *(nodo->tweets));
					tweets_ant = *nodo->tweets;
				}
			printf("%s ",nodo->usuario);
			free(nodo);
		}
		lista_destruir(arraytmp[i], NULL);
	}
	printf("\n");
	free(arraytmp);
}

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/
int main(int argc, char* argv[]) {
	
	if (argc != MAX_ARGUMENTS) {
		printf("Error! Debe pasar solo un argumento...\n");
		return 1;
	}

	FILE *file_input = fopen(argv[1], "r");
	if (!file_input) {
		printf("Error al abrir el archivo de lectura! \n");
		return 1;
	}

	size_t buffersize = 0;					// Bytes leidos en getline
	char* strline = NULL;					// Almacena puntero a nueva linea leida
	size_t linebuffer = 0;					// Tamano de buffer actual para lectura de nueva linea
	char* token;							// Almacena puntero a cada token de una linea
	char delimiter[2] = ",";				// Caracter delimitador
	char* usuario;							// Puntero al nombre de usuario
	size_t max_dato = 0;					// Maxima cantidad de tweets por usuario
	size_t* dato;							// Puntero al dato del hash
	bool existe;							// Variable para controlar existencia de clave en hash

	hash_t* hash = hash_crear(free);
	if (!hash) {
		printf("Memoria insuficiente. Abortando...\n");
		return 1;
	}

	linebuffer = getline(&strline, &buffersize, file_input);
	while (linebuffer != -1) {
		if (strline[linebuffer - 1] == '\n')
			strline[linebuffer - 1] = '\0';
		usuario = strtok(strline, delimiter);			// Leo nombre de usuario
		dato = hash_obtener(hash, usuario);				// Obtengo cantidad de tweets segun nombre de usuario
		existe = (dato != NULL);
		if (!dato) {
			dato = malloc(sizeof(size_t));
			if (!dato) {
				printf("Memoria insuficiente. Abortando...\n");
				hash_destruir(hash);
				return 1;
			}
			*dato = 0;
		}
		token = strtok(NULL, delimiter);			// Leo primer tweet
		while  (token) {
			*dato +=1;
			token = strtok(NULL, delimiter);
		}
		if (*dato > max_dato)
			max_dato = *dato;
		if (!existe)
			hash_guardar(hash, usuario, dato);
		linebuffer = getline(&strline, &buffersize, file_input);
	}
	generarOutput(hash, max_dato);
	hash_destruir(hash);
	fclose(file_input);
	free(strline);
    return 0;
}
