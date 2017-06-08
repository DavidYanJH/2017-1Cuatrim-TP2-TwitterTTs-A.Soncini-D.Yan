#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cbf.h"
#include "lista.h"
#include "heap.h"
#include "pila.h"

#define CBFMIN 997 // Capacidad Inicial y Minima de la Tabla de Hash
#define NUMPRIMO 5381  // Numero Primo en la funcion de hashing
#define PERCENT 100 // Constante Numerico en Calculo de Factor de Carga
#define COEFICIENTE 2 // Constante de Variacion en Redimension de la Capacidad de Hash
#define MAXCARGA 200 // Maximo Factor de Carga
#define MINCARGA 20 // Minimo Factor de Carga
#define BITSHIFT 5 // Salto d bits a la izquierda en funcion de Hash

struct cbf {
	lista_t** tabla;
	size_t* tablacont1;
	size_t* tablacont2;
	size_t* tablacont3;
	size_t capacidad;
	size_t cantidad;
};

struct cbf_iter {
	const cbf_t* cbf;
	lista_iter_t* lista_iter;
	size_t index_actual;
	size_t iteradas;
}; 

typedef struct nodo_cbf {
	char* clave;
	size_t hash1;
	size_t hash2;
	size_t hash3;
	size_t rts;
}celda_t;

/* ******************************************************************
 *            DECLARACION DE LAS FUNCIONES AUXILIARES
 * *****************************************************************/

lista_t** crear_cbf_tabla(size_t size_cbf) {
	size_t i = 0;
	lista_t** cbf_tabla = malloc(sizeof(lista_t*) * size_cbf);
	if (!cbf_tabla) return NULL;
	while ((i < size_cbf) && (cbf_tabla[i] = lista_crear()))
		i++;
	if (i != size_cbf) {
		for (size_t j = 0; j < i; j++)
			free(cbf_tabla[j]);
		free(cbf_tabla);
	}
	return cbf_tabla;
}

size_t* crear_cbf_tablacont(size_t size_cbf) {
	size_t* tablacont = malloc(sizeof(size_t) * size_cbf);
	if (tablacont) {
		for (size_t i = 0; i < size_cbf; i++) {
			tablacont[i] = 0;
		}
	}
	return tablacont;
}

size_t djb2hash(const char *str, size_t size_cbf) {
	unsigned long cbf = NUMPRIMO;
    int c;
    while ((c = *str++)) {
    	cbf = ((cbf << BITSHIFT) + cbf) + c; /*hash * 33 + c*/
    }
    	return (cbf % size_cbf);
}

size_t elfhash ( const char *s, size_t size_cbf)
{
    size_t   h = 0;
    size_t   high = 0;
    while ( *s )
    {
        h = ( h << 4 ) + *s++;
        if ( high == h && 0xF0000000 )
            h ^= high >> 24;
        h &= ~high;
    }
    return h % size_cbf;
}

lista_iter_t* busqueda_cbf_cell(cbf_t* cbf, const char* clave, size_t* index) {
	*index = djb2hash(clave, cbf->capacidad);
	
	lista_iter_t* iter = lista_iter_crear(cbf->tabla[*index]);
	if (!iter) return NULL;

	celda_t* celda = NULL;
	while (!lista_iter_al_final(iter)) {
		celda = lista_iter_ver_actual(iter);
		if (strcmp(clave, celda->clave) == 0)
	    	break;
		lista_iter_avanzar(iter);
	}
	return iter;
}

celda_t* cargar_cbf_cell(const char* clave, size_t cbf_capacidad, size_t index) {
	celda_t* celda = malloc(sizeof(celda_t));
	if (!celda) return NULL;
	celda->clave = malloc(sizeof(char)* (strlen(clave) + 1));
	if (!celda->clave) {
		free(celda);
		return NULL;
	}
	strcpy(celda->clave, clave);
	celda->hash1 = index;
	celda->hash2 = elfhash(clave, cbf_capacidad);
	celda->hash3 = celda->hash1 * celda->hash2 % cbf_capacidad;
	return celda;
}

void cell_destruir(celda_t* cell) {
	//if (destruir_dato) destruir_dato(cell->dato);
	free(cell->clave);
    free(cell);
}

float calculo_carga(size_t cbf_cantidad, size_t cbf_capacidad) {
	float factor_carga = (float)cbf_cantidad / (float)cbf_capacidad * PERCENT;
	return factor_carga;
}

void tabla_destruir(lista_t** tabla, size_t capacidad) {
	for(size_t index = 0; index < capacidad; ++index)
		if (tabla[index]) lista_destruir(tabla[index], NULL);
	free(tabla);
}

bool cbf_rehashing(cbf_t* cbf, lista_t** new_tabla, size_t new_capacidad, size_t* new_tablacont1, size_t* new_tablacont2, size_t* new_tablacont3) {
	bool OK = true;
	size_t i = 0;
	lista_iter_t* iter = NULL;
	celda_t* cell = NULL;
	size_t new_index;

	lista_t** old_tabla = cbf->tabla;
	size_t* old_tablacont1 = cbf->tablacont1;
	size_t* old_tablacont2 = cbf->tablacont2;
	size_t* old_tablacont3 = cbf->tablacont3;
	cbf->tabla = new_tabla;
	cbf->tablacont1 = new_tablacont1;
	cbf->tablacont2 = new_tablacont2;
	cbf->tablacont3 = new_tablacont3;

	size_t old_capacidad = cbf->capacidad;
	cbf->capacidad = new_capacidad;

	while ((OK) && (i < old_capacidad)) {
		iter = lista_iter_crear(old_tabla[i]);
		if (!iter) OK = false;
		while ((OK) && (!lista_iter_al_final(iter))) {
			cell = lista_iter_ver_actual(iter);
			new_index = djb2hash(cell->clave, cbf->capacidad);
			OK = lista_insertar_ultimo(cbf->tabla[new_index], cell);
			if (!OK) break;
			cbf->tablacont1[new_index] = old_tablacont1[cell->hash1];
			cell->hash1 = new_index;

			size_t new_index2 = elfhash(cell->clave, cbf->capacidad);
			cbf->tablacont2[new_index2] = old_tablacont2[cell->hash2];
			cell->hash2 = new_index2;

			size_t new_index3 = new_index * new_index2 % cbf->capacidad;
			cbf->tablacont3[new_index3] = old_tablacont3[cell->hash3];
			cell->hash3 = new_index3;
			lista_iter_avanzar(iter);
		}
		lista_iter_destruir(iter);
		i++;
	}
	if (OK) {
		tabla_destruir(old_tabla, old_capacidad);
		free(old_tablacont1);
		free(old_tablacont2);
		free(old_tablacont3);
	}
	else {
		cbf->tabla = old_tabla;
		cbf->capacidad = old_capacidad;
		cbf->tablacont1 = old_tablacont1;
		cbf->tablacont2 = old_tablacont2;
		cbf->tablacont3 = old_tablacont3;
	}
	return OK;
}

bool cbf_redimensionar(cbf_t* cbf) {
	float factor_carga = calculo_carga(cbf->cantidad, cbf->capacidad);
    size_t new_capacidad = 0;
	if (factor_carga > MAXCARGA) new_capacidad = cbf->capacidad * COEFICIENTE;
	if (factor_carga < MINCARGA) new_capacidad = cbf->capacidad / COEFICIENTE;
	if (new_capacidad < CBFMIN) return false;
	lista_t** new_tabla = crear_cbf_tabla(new_capacidad);
	size_t* new_tablacont1 = crear_cbf_tablacont(new_capacidad);
	size_t* new_tablacont2 = crear_cbf_tablacont(new_capacidad);
	size_t* new_tablacont3 = crear_cbf_tablacont(new_capacidad);
	if (!new_tabla || !new_tablacont1 || !new_tablacont2 || !new_tablacont3){
		free(new_tabla);
		free(new_tablacont1);
		free(new_tablacont2);
		free(new_tablacont3);
		return false;
	}
	if (!cbf_rehashing(cbf, new_tabla, new_capacidad, new_tablacont1, new_tablacont2, new_tablacont3)) {
        tabla_destruir(new_tabla, new_capacidad);
        free(new_tablacont1);
		free(new_tablacont2);
		free(new_tablacont3);
		return false;
	}
	return true;
}

size_t obtenermin(const cbf_t* cbf, const celda_t* celda) {
 	size_t min = cbf->tablacont1[celda->hash1];
 	if (min > cbf->tablacont2[celda->hash2])
 		min = cbf->tablacont2[celda->hash2];
 	if (min > cbf->tablacont3[celda->hash3])
 		min = cbf->tablacont3[celda->hash3];
 	return min;
 }

 int cmprts(const void* a, const void* b) {
 	const celda_t* aux1 = a;
 	const celda_t* aux2 = b;
 	if (aux1->rts > aux2->rts)
 		return -1;
 	else if (aux1->rts < aux2->rts)
 		return 1;
 	else return 0;
 }

/* ******************************************************************
 *                   PRIMITIVAS DEL CBF
 * *****************************************************************/

cbf_t* cbf_crear() {
 	cbf_t* cbf = malloc(sizeof(cbf_t));
 	if (!cbf) return NULL;
 	cbf->tabla = crear_cbf_tabla(CBFMIN);
 	cbf->tablacont1 = crear_cbf_tablacont(CBFMIN);
 	cbf->tablacont2 = crear_cbf_tablacont(CBFMIN);
 	cbf->tablacont3 = crear_cbf_tablacont(CBFMIN);
 	if (!cbf->tabla || !cbf->tablacont1 || !cbf->tablacont2 || !cbf->tablacont3) {
 		free(cbf->tabla);
 		free(cbf->tablacont1);
 		free(cbf->tablacont2);
 		free(cbf->tablacont3);
 		free(cbf);
 		return NULL;
 	}
 	cbf->capacidad = CBFMIN;
 	cbf->cantidad = 0;
 	return cbf;
 }

bool cbf_guardar(cbf_t* cbf, const char *clave) {
	cbf_redimensionar(cbf);
 	size_t index;
 	celda_t* celda = NULL;
 	lista_iter_t* iter = busqueda_cbf_cell(cbf, clave, &index);
 	if (!iter) return false;
 	celda = lista_iter_ver_actual(iter);
 	if (celda) {
 		cbf->tablacont1[celda->hash1]++;
 		cbf->tablacont2[celda->hash2]++;
 		cbf->tablacont3[celda->hash3]++;
 		celda->rts = obtenermin(cbf, celda);
		lista_iter_destruir(iter);
		return true;
 	}
 	lista_iter_destruir(iter);
 	celda = cargar_cbf_cell(clave, cbf->capacidad, index);
 	if (!celda)	return false;
 	lista_insertar_ultimo(cbf->tabla[index], celda);
 	cbf->tablacont1[celda->hash1]++;
 	cbf->tablacont2[celda->hash2]++;
 	cbf->tablacont3[celda->hash3]++;
 	celda->rts = obtenermin(cbf, celda);
 	cbf->cantidad++;
 	return true;
 }

size_t cbf_cantidad(const cbf_t* cbf) {
	return cbf->cantidad;
}

void cbf_destruir(cbf_t* cbf) {
	celda_t* cell = NULL;
	size_t index = 0;
	for (; index < cbf->capacidad; index++) {
		while (!lista_esta_vacia(cbf->tabla[index])){
			cell = lista_borrar_primero(cbf->tabla[index]);
			cell_destruir(cell);
		}
		lista_destruir(cbf->tabla[index], NULL);
	}
	free(cbf->tabla);
	free(cbf->tablacont1);
	free(cbf->tablacont2);
	free(cbf->tablacont3);
	free(cbf);
}

/* ******************************************************************
 *                   PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

cbf_iter_t *cbf_iter_crear(const cbf_t *cbf) {
 	cbf_iter_t* iter = malloc(sizeof(cbf_iter_t));
 	if (!iter) return NULL;
 	iter->cbf = cbf;
 	iter->index_actual = 0;
 	iter->iteradas = 0;
 	if (cbf->cantidad == 0) {
 		iter->lista_iter = NULL;
 		return iter;
 	}
 	while (lista_esta_vacia(iter->cbf->tabla[iter->index_actual]))
 		iter->index_actual++;
 	iter->lista_iter = lista_iter_crear(cbf->tabla[iter->index_actual]);
 	if (!iter->lista_iter) {
 		free(iter);
 		return NULL;
 	}
 	return iter;
 }

void *cbf_iter_ver_actual (const cbf_iter_t *iter) {
	if (cbf_iter_al_final(iter)) return NULL;
	celda_t* celda = lista_iter_ver_actual(iter->lista_iter);
	return celda;
}

bool cbf_iter_avanzar(cbf_iter_t* cbf_iter) {
	if (cbf_iter_al_final(cbf_iter)) return false;
	lista_iter_avanzar(cbf_iter->lista_iter);
	cbf_iter->iteradas++;
	if (!lista_iter_al_final(cbf_iter->lista_iter)) return true;
	lista_iter_destruir(cbf_iter->lista_iter);
	while (cbf_iter->iteradas < cbf_iter->cbf->cantidad) {
		cbf_iter->index_actual++;
		while (lista_esta_vacia(cbf_iter->cbf->tabla[cbf_iter->index_actual]))
 			cbf_iter->index_actual++;
		cbf_iter->lista_iter = lista_iter_crear(cbf_iter->cbf->tabla[cbf_iter->index_actual]);
		if (!cbf_iter->lista_iter) return false;
		else return true;
	}
	cbf_iter->lista_iter = NULL;
	return true;
}

bool cbf_iter_al_final(const cbf_iter_t *cbf_iter) {
	return cbf_iter->lista_iter == NULL;
}
	
void cbf_iter_destruir(cbf_iter_t* cbf_iter) {
	if(!cbf_iter_al_final(cbf_iter)) free(cbf_iter->lista_iter);
	free(cbf_iter);	
}

void imprimirTrendingTopics(cbf_t* cbf, size_t k) {
	heap_t* heap = heap_crear(cmprts);
	pila_t* pila = pila_crear();
	cbf_iter_t* iter = cbf_iter_crear(cbf);
	while (!cbf_iter_al_final(iter)) {
		celda_t* celda = cbf_iter_ver_actual(iter);
		if (heap_cantidad(heap) < k)
			heap_encolar(heap, celda);
		else {
			celda_t* celda_min = heap_ver_max(heap);
			if (cmprts(celda_min, celda) > 0) {
				celda_min = heap_desencolar(heap);
				heap_encolar(heap, celda);
			}
		}
		cbf_iter_avanzar(iter);
	}
	cbf_iter_destruir(iter);
	printf("************TWEETS CON TENDENCIA*************\n");
	while (!heap_esta_vacio(heap)) {
		pila_apilar(pila, heap_desencolar(heap));
	}
	heap_destruir(heap, NULL);
	while (!pila_esta_vacia(pila)) {
		celda_t* celda = pila_desapilar(pila);
		printf("TWEET #%s fue Trending Topic con %zu RTs.\n", celda->clave, celda->rts);
	}
	printf("\n\n\n");
	pila_destruir(pila);
}