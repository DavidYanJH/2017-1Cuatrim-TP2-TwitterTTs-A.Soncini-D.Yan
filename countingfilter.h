#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct counting_filter {
	size_t* tabla1;
	size_t* tabla2;
	size_t* tabla3;
	size_t capacidad;
} counting_filter_t;

typedef struct nodo_counter {
	size_t* pt1;
	size_t* pt2;
	size_t* pt3;
} nodo_counting_t;

counting_filter_t* counting_filter_crear(size_t counting_size);

nodo_counting_t* counting_filter_generar_nodo(counting_filter_t* counting_filter, char* clave);

void counting_filter_destruir(counting_filter_t* counting_filter);