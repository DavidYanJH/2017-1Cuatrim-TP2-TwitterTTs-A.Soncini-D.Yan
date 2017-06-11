#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "countingfilter.h"
#include "heap.h"
#include "pila.h"
#include "auxlib.h"

size_t* obtenermin(const nodo_counting_t* nodo) {
 	size_t* minimo;
 	if (*nodo->pt1 < *nodo->pt2)
 		minimo = nodo->pt1;
 	else
 		minimo = nodo->pt2;
 	if (*nodo->pt3 < *minimo)
 		minimo = nodo->pt3;
 	return minimo;
 }

int cmprts(const void* a, const void* b) {
 	const nodo_heap_t* nodoa = a;
 	const nodo_heap_t* nodob = b;

 	if (*nodoa->tts < *nodob->tts)
 		return 1;
 	else if (*nodoa->tts > *nodob->tts)
 		return -1;
 	else return 0;
 }

void imprimirTrendingTopics(hash_t* hash, counting_filter_t* counting_filter, size_t maxtts) {
	pila_t* pila = pila_crear();
	if (!pila) {
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		hash_destruir(hash);
		counting_filter_destruir(counting_filter);
		exit(EXIT_FAILURE);
	}

	heap_t* heap = heap_crear(cmprts);
	if (!heap) {
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		pila_destruir(pila);
		hash_destruir(hash);
		counting_filter_destruir(counting_filter);
		exit(EXIT_FAILURE);
	}

	hash_iter_t* iter = hash_iter_crear(hash);
	if (!iter) {
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		pila_destruir(pila);
		heap_destruir(heap, NULL);
		hash_destruir(hash);
		counting_filter_destruir(counting_filter);
		exit(EXIT_FAILURE);
	}

	while (!hash_iter_al_final(iter)) {
		nodo_heap_t* nodo_heap = malloc(sizeof(nodo_heap_t));
		if (!nodo_heap) {
			fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
			pila_destruir(pila);
			heap_destruir(heap, NULL);
			hash_iter_destruir(iter);
			hash_destruir(hash);
			counting_filter_destruir(counting_filter);
			exit(EXIT_FAILURE);
		}

		char* clavetmp = hash_iter_ver_actual(iter);
		nodo_heap->clave = clavetmp;
		nodo_counting_t* nodo_hash = hash_obtener(hash, nodo_heap->clave);
		nodo_heap->tts = obtenermin(nodo_hash);
		if (heap_cantidad(heap) < maxtts)
			heap_encolar(heap, nodo_heap);
		else {
			nodo_heap_t* nodo_min = heap_ver_max(heap);
			if (cmprts(nodo_min, nodo_heap) > 0) {
				nodo_min = heap_desencolar(heap);
				heap_encolar(heap, nodo_heap);
			}
		}
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);

	printf("************TWEETS CON TENDENCIA*************\n");
	while (!heap_esta_vacio(heap)) {
		pila_apilar(pila, heap_desencolar(heap));
	}
	while (!pila_esta_vacia(pila)) {
		nodo_heap_t* nodo = pila_desapilar(pila);
		heap_encolar(heap, nodo);
		//printf("TWEET #%s fue Trending Topic con %zu RTs.\n", nodo->clave, *nodo->tts);
		printf("#%s fue Trending Topic.\n", nodo->clave);
	}
	printf("\n");
	pila_destruir(pila);
	heap_destruir(heap, free);
}

void procesarEntrada(counting_filter_t* counting_filter, size_t maxlines, size_t maxtts) {

	size_t buffersize = 0;					// Bytes leidos en getline
	char* strline = NULL;					// Almacena puntero a nueva linea leida
	size_t linebuffer = 0;					// Tamano de buffer actual para lectura de nueva linea
	char* token;							// Almacena puntero a cada token de una linea
	size_t linecount;						// Contador de lineas leidas
	char delimiter[2] = ",";				// Delimitador de archivo

	linebuffer = getline(&strline, &buffersize, stdin);
	while (linebuffer != -1) {
		hash_t* hash = hash_crear(free);
		
		if (!hash) {
			fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
			counting_filter_destruir(counting_filter);
			exit(EXIT_FAILURE);	
		}

		linecount = 0;
		while ((linebuffer != -1) && (linecount < maxlines)) {
			linecount++;

			if (strline[linebuffer - 1] == '\n')	// Quito caracter de salto de linea en ultimo tweet
				strline[linebuffer - 1] = '\0';

			token = strtok(strline, delimiter); // Excluyo el nombre de usuario.
			token = strtok(NULL, delimiter);	// Obtengo primer tweet

			while  (token) {
				nodo_counting_t* nodo = counting_filter_generar_nodo(counting_filter, token);	// Obtengo indices para el tweet actual

				if (!nodo) {
					fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
					counting_filter_destruir(counting_filter);
					hash_destruir(hash);
					exit(EXIT_FAILURE);	
				}

				// Incremento contadores de tweet con los indices obtenidos
				*nodo->pt1+=1; 
				*nodo->pt2+=1;
				*nodo->pt3+=1;

				if (!hash_guardar(hash, token, nodo)) {
					fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
					counting_filter_destruir(counting_filter);
					hash_destruir(hash);
					exit(EXIT_FAILURE);
				}

				token = strtok(NULL, delimiter);
			}

			linebuffer = getline(&strline, &buffersize, stdin);		// Reinicio ciclo de lectura
		}

		imprimirTrendingTopics(hash, counting_filter, maxtts);
		hash_destruir(hash);										// Destruyo hash temporal
	}
	if (linebuffer == -1 && !strline) {								// No fue posible allocar buffer para lectura
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		exit(EXIT_FAILURE);
	}
	counting_filter_destruir(counting_filter);
}