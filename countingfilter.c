#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "countingfilter.h"

#define NUMPRIMO 5381
#define BITSHIFT 5


size_t djb2hash(const char *str, size_t counting_size) {
	unsigned long hash = NUMPRIMO;
    int c;
    while ((c = *str++)) {
    	hash = ((hash << BITSHIFT) + hash) + c; /*hash * 33 + c*/
    }
    	return (hash % counting_size);
}

size_t elfhash ( const char *s, size_t counting_size)
{
    size_t   h = 0;
    size_t   high = 0;
    while ( *s ) {
        h = ( h << 4 ) + *s++;
        if ( high == h && 0xF0000000 )
            h ^= high >> 24;
        h &= ~high;
    }
    return h % counting_size;
}

counting_filter_t* counting_filter_crear(size_t counting_size) {
	if (counting_size == 0) return NULL;
	counting_filter_t* counting_filter = malloc(sizeof(counting_filter_t));
	if (!counting_filter) return NULL;
	counting_filter->tabla1 = calloc(counting_size + 1, sizeof(size_t));
	counting_filter->tabla2 = calloc(counting_size + 1, sizeof(size_t));
	counting_filter->tabla3 = calloc(counting_size + 1, sizeof(size_t));
	if (!counting_filter->tabla1 || !counting_filter->tabla2 || !counting_filter->tabla3) {
		free(counting_filter->tabla1);
		free(counting_filter->tabla2);
		free(counting_filter->tabla3);
		free(counting_filter);
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		return NULL;
	}
	counting_filter->capacidad = counting_size;
	return counting_filter;
}

nodo_counting_t* counting_filter_generar_nodo(counting_filter_t* counting_filter, char* clave) {
	nodo_counting_t* nodo = malloc(sizeof(nodo_counting_t));
	if (!nodo) return NULL;

	size_t index1 = djb2hash(clave, counting_filter->capacidad);
	size_t index2 = elfhash(clave, counting_filter->capacidad);
	size_t index3 = index1 * index2 % counting_filter->capacidad;

	nodo->pt1 = &(counting_filter->tabla1[index1]);
	nodo->pt2 = &(counting_filter->tabla2[index2]);
	nodo->pt3 = &(counting_filter->tabla3[index3]);
	return nodo;
}

void counting_filter_destruir(counting_filter_t* counting_filter) {
	free(counting_filter->tabla1);
	free(counting_filter->tabla2);
	free(counting_filter->tabla3);
	free(counting_filter);
}