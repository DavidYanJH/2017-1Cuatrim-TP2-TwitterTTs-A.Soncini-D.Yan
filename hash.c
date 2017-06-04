#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "heap.h"

#define HASHMIN 997 // Capacidad Inicial y Minima de la Tabla de Hash
#define NUMPRIMO 5381  // Numero Primo en la funcion de hashing
#define PERCENT 100 // Constante Numerico en Calculo de Factor de Carga
#define COEFICIENTE 2 // Constante de Variacion en Redimension de la Capacidad de Hash
#define MAXCARGA 200 // Maximo Factor de Carga
#define MINCARGA 20 // Minimo Factor de Carga
#define BITSHIFT 5 // Salto d bits a la izquierda en funcion de Hash

struct hash {
	lista_t** tabla;
	size_t* tablacont1;
	size_t* tablacont2;
	size_t* tablacont3;
	size_t capacidad;
	size_t existentes;
	//hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
	const hash_t* hash;
	lista_iter_t* lista_iter;
	size_t index_actual;
	size_t iteradas;
}; 

typedef struct nodo_hash {
	char* clave;
	size_t hash1;
	size_t hash2;
	size_t hash3;
	size_t rts;
}celda_t;

/* ******************************************************************
 *            DECLARACION DE LAS FUNCIONES AUXILIARES
 * *****************************************************************/

lista_t** crear_hash_tabla(size_t size_hash) {
	size_t i = 0;
	lista_t** hash_tabla = malloc(sizeof(lista_t*) * size_hash);
	if (!hash_tabla) return NULL;
	while ((i < size_hash) && (hash_tabla[i] = lista_crear()))
		i++;
	if (i != size_hash) {
		for (size_t j = 0; j < i; j++)
			free(hash_tabla[j]);
		free(hash_tabla);
	}
	return hash_tabla;
}

size_t* crear_hash_tablacont(size_t size_hash) {
	size_t* tablacont = malloc(sizeof(size_t) * size_hash);
	if (tablacont) {
		for (size_t i = 0; i < size_hash; i++) {
			tablacont[i] = 0;
		}
	}
	return tablacont;
}

size_t djb2hash(const char *str, size_t length) {
	unsigned long hash = NUMPRIMO;
    int c;
    while ((c = *str++)) {
    	hash = ((hash << BITSHIFT) + hash) + c; /*hash * 33 + c*/
    }
    	return (hash % length);
}

size_t elfhash ( const char *s, size_t length)
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
    return h % length;
}

lista_iter_t* busqueda_hash_cell(hash_t* hash, const char* clave, size_t* index) {
	*index = djb2hash(clave, hash->capacidad);
	
	lista_iter_t* iter = lista_iter_crear(hash->tabla[*index]);
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

celda_t* cargar_hash_cell(const char* clave, size_t hash_capacidad, size_t index) {
	celda_t* celda = malloc(sizeof(celda_t));
	if (!celda) return NULL;
	celda->clave = malloc(sizeof(char)* (strlen(clave) + 1));
	if (!celda->clave) {
		free(celda);
		return NULL;
	}
	strcpy(celda->clave, clave);
	celda->hash1 = index;
	celda->hash2 = elfhash(clave, hash_capacidad);
	celda->hash3 = celda->hash1 * celda->hash2 % hash_capacidad;
	return celda;
}

void cell_destruir(celda_t* cell) {
	//if (destruir_dato) destruir_dato(cell->dato);
	free(cell->clave);
    free(cell);
}

float calculo_carga(size_t hash_existentes, size_t hash_capacidad) {
	float factor_carga = (float)hash_existentes / (float)hash_capacidad * PERCENT;
	return factor_carga;
}

void tabla_destruir(lista_t** tabla, size_t capacidad) {
	for(size_t index = 0; index < capacidad; ++index)
		if (tabla[index]) lista_destruir(tabla[index], NULL);
	free(tabla);
}

bool hash_rehashing(hash_t* hash, lista_t** new_tabla, size_t new_capacidad, size_t* new_tablacont1, size_t* new_tablacont2, size_t* new_tablacont3) {
	bool OK = true;
	size_t i = 0;
	lista_iter_t* iter = NULL;
	celda_t* cell = NULL;
	size_t new_index;

	lista_t** old_tabla = hash->tabla;
	size_t* old_tablacont1 = hash->tablacont1;
	size_t* old_tablacont2 = hash->tablacont2;
	size_t* old_tablacont3 = hash->tablacont3;
	hash->tabla = new_tabla;
	hash->tablacont1 = new_tablacont1;
	hash->tablacont2 = new_tablacont2;
	hash->tablacont3 = new_tablacont3;

	size_t old_capacidad = hash->capacidad;
	hash->capacidad = new_capacidad;

	while ((OK) && (i < old_capacidad)) {
		iter = lista_iter_crear(old_tabla[i]);
		if (!iter) OK = false;
		while ((OK) && (!lista_iter_al_final(iter))) {
			cell = lista_iter_ver_actual(iter);
			new_index = djb2hash(cell->clave, hash->capacidad);
			OK = lista_insertar_ultimo(hash->tabla[new_index], cell);
			if (!OK) break;
			hash->tablacont1[new_index] = old_tablacont1[cell->hash1];
			cell->hash1 = new_index;

			size_t new_index2 = elfhash(cell->clave, hash->capacidad);
			hash->tablacont2[new_index2] = old_tablacont2[cell->hash2];
			cell->hash2 = new_index2;

			size_t new_index3 = new_index * new_index2 % hash->capacidad;
			hash->tablacont3[new_index3] = old_tablacont3[cell->hash3];
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
		hash->tabla = old_tabla;
		hash->capacidad = old_capacidad;
		hash->tablacont1 = old_tablacont1;
		hash->tablacont2 = old_tablacont2;
		hash->tablacont3 = old_tablacont3;
	}
	return OK;
}

bool hash_redimensionar(hash_t* hash) {
	float factor_carga = calculo_carga(hash->existentes, hash->capacidad);
    size_t new_capacidad = 0;
	if (factor_carga > MAXCARGA) new_capacidad = hash->capacidad * COEFICIENTE;
	if (factor_carga < MINCARGA) new_capacidad = hash->capacidad / COEFICIENTE;
	if (new_capacidad < HASHMIN) return false;
	lista_t** new_tabla = crear_hash_tabla(new_capacidad);
	size_t* new_tablacont1 = crear_hash_tablacont(new_capacidad);
	size_t* new_tablacont2 = crear_hash_tablacont(new_capacidad);
	size_t* new_tablacont3 = crear_hash_tablacont(new_capacidad);
	if (!new_tabla || !new_tablacont1 || !new_tablacont2 || !new_tablacont3){
		free(new_tabla);
		free(new_tablacont1);
		free(new_tablacont2);
		free(new_tablacont3);
		return false;
	}
	if (!hash_rehashing(hash, new_tabla, new_capacidad, new_tablacont1, new_tablacont2, new_tablacont3)) {
        tabla_destruir(new_tabla, new_capacidad);
        free(new_tablacont1);
		free(new_tablacont2);
		free(new_tablacont3);
		return false;
	}
	return true;
}

size_t obtenermin(const hash_t* hash, const celda_t* celda) {
 	size_t min = hash->tablacont1[celda->hash1];
 	if (min > hash->tablacont2[celda->hash2])
 		min = hash->tablacont2[celda->hash2];
 	if (min > hash->tablacont3[celda->hash3])
 		min = hash->tablacont3[celda->hash3];
 	return min;
 }

 int cmprts(const void* a, const void* b) {
 	const celda_t* aux1 = a;
 	const celda_t* aux2 = b;
 	if (aux1->rts > aux2->rts)
 		return 1;
 	else if (aux1->rts < aux2->rts)
 		return -1;
 	else return 0;
 }

/* ******************************************************************
 *                   PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t* hash_crear() {
 	hash_t* hash = malloc(sizeof(hash_t));
 	if (!hash) return NULL;
 	hash->tabla = crear_hash_tabla(HASHMIN);
 	hash->tablacont1 = crear_hash_tablacont(HASHMIN);
 	hash->tablacont2 = crear_hash_tablacont(HASHMIN);
 	hash->tablacont3 = crear_hash_tablacont(HASHMIN);
 	if (!hash->tabla || !hash->tablacont1 || !hash->tablacont2 || !hash->tablacont3) {
 		free(hash->tabla);
 		free(hash->tablacont1);
 		free(hash->tablacont2);
 		free(hash->tablacont3);
 		free(hash);
 		return NULL;
 	}
 	hash->capacidad = HASHMIN;
 	hash->existentes = 0;
 	//hash->destruir_dato = destruir_dato;
 	return hash;
 }

bool hash_guardar(hash_t *hash, const char *clave) {
	hash_redimensionar(hash);
 	size_t index;
 	celda_t* celda = NULL;
 	lista_iter_t* iter = busqueda_hash_cell(hash, clave, &index);
 	if (!iter) return false;
 	celda = lista_iter_ver_actual(iter);
 	if (celda) {
 		hash->tablacont1[celda->hash1]++;
 		hash->tablacont2[celda->hash2]++;
 		hash->tablacont3[celda->hash3]++;
 		celda->rts = obtenermin(hash, celda);
		lista_iter_destruir(iter);
		return true;
 	}
 	lista_iter_destruir(iter);
 	celda = cargar_hash_cell(clave, hash->capacidad, index);
 	if (!celda)	return false;
 	lista_insertar_ultimo(hash->tabla[index], celda);
 	hash->tablacont1[celda->hash1]++;
 	hash->tablacont2[celda->hash2]++;
 	hash->tablacont3[celda->hash3]++;
 	celda->rts = obtenermin(hash, celda);
 	hash->existentes++;
 	return true;
 }

/*void *hash_borrar(hash_t *hash, const char *clave) {
	hash_redimensionar(hash);
	size_t index;
	lista_iter_t* iter = busqueda_hash_cell(hash, clave, &index);
	void* aux_dato;
	if (!iter) return NULL;
	celda_t* celda = lista_iter_borrar(iter);
	lista_iter_destruir(iter);
	if (!celda) return NULL;
	aux_dato = celda->dato;
	free(celda->clave);
	free(celda);
	hash->existentes--;
	return aux_dato;
}*/

/*void* hash_obtener(const hash_t *hash, const char *clave) {
	size_t index;
 	lista_iter_t* iter = busqueda_hash_cell((hash_t*) hash, (char*) clave, &index);
 	if (!iter) return NULL;
 	celda_t* celda = lista_iter_ver_actual(iter);
 	lista_iter_destruir(iter);
 	return celda;
}// Devuelvo la celda porque necesito el tweet y su minima aparicion*/


/*bool hash_pertenece(const hash_t* hash, const char* clave) {
	size_t index;
	lista_iter_t* iter = busqueda_hash_cell((hash_t*) hash, (char*) clave, &index);
	if (!iter) return false;
	celda_t* celda = lista_iter_ver_actual(iter);
	lista_iter_destruir(iter);
	if (!celda) return false;
	else return true;
}*/

size_t hash_cantidad(const hash_t* hash) {
	return hash->existentes;
}

void hash_destruir(hash_t *hash) {
	celda_t* cell = NULL;
	size_t index = 0;
	for (; index < hash->capacidad; index++) {
		while (!lista_esta_vacia(hash->tabla[index])){
			cell = lista_borrar_primero(hash->tabla[index]);
			cell_destruir(cell);
		}
		lista_destruir(hash->tabla[index], NULL);
	}
	free(hash->tabla);
	free(hash->tablacont1);
	free(hash->tablacont2);
	free(hash->tablacont3);
	free(hash);
}

/* ******************************************************************
 *                   PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash) {
 	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
 	if (!iter) return NULL;
 	iter->hash = hash;
 	iter->index_actual = 0;
 	iter->iteradas = 0;
 	if (hash->existentes == 0) {
 		iter->lista_iter = NULL;
 		return iter;
 	}
 	while (lista_esta_vacia(iter->hash->tabla[iter->index_actual]))
 		iter->index_actual++;
 	iter->lista_iter = lista_iter_crear(hash->tabla[iter->index_actual]);
 	if (!iter->lista_iter) {
 		free(iter);
 		return NULL;
 	}
 	return iter;
 }

void *hash_iter_ver_actual (const hash_iter_t *iter) {
	if (hash_iter_al_final(iter)) return NULL;
	celda_t* celda = lista_iter_ver_actual(iter->lista_iter);
	return celda;
}

bool hash_iter_avanzar(hash_iter_t* hash_iter) {
	if (hash_iter_al_final(hash_iter)) return false;
	lista_iter_avanzar(hash_iter->lista_iter);
	hash_iter->iteradas++;
	if (!lista_iter_al_final(hash_iter->lista_iter)) return true;
	lista_iter_destruir(hash_iter->lista_iter);
	while (hash_iter->iteradas < hash_iter->hash->existentes) {
		hash_iter->index_actual++;
		while (lista_esta_vacia(hash_iter->hash->tabla[hash_iter->index_actual]))
 			hash_iter->index_actual++;
		hash_iter->lista_iter = lista_iter_crear(hash_iter->hash->tabla[hash_iter->index_actual]);
		if (!hash_iter->lista_iter) return false;
		else return true;
	}
	hash_iter->lista_iter = NULL;
	return true;
}

bool hash_iter_al_final(const hash_iter_t *hash_iter) {
	return hash_iter->lista_iter == NULL;
}
	
void hash_iter_destruir(hash_iter_t* hash_iter) {
	if(!hash_iter_al_final(hash_iter)) free(hash_iter->lista_iter);
	free(hash_iter);	
}

void imprimirTrendingTopics(hash_t* hash, size_t k) {
	heap_t* heap = heap_crear(cmprts);
	hash_iter_t* iter = hash_iter_crear(hash);
	while (!hash_iter_al_final(iter)) {
		celda_t* celda = hash_iter_ver_actual(iter);
		if (heap_cantidad(heap) < k)
			heap_encolar(heap, celda);
		else {
			celda_t* celda_min = heap_ver_max(heap);
			if (cmprts(celda_min, celda) < 0) {
				celda_min = (celda_t*) heap_desencolar(heap);
				//cell_destruir(celda_min);
				heap_encolar(heap, celda);
			}
		}
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	printf("************TWEETS CON TENDENCIA*************\n");
	while (!heap_esta_vacio(heap)) {
		celda_t* celda = heap_desencolar(heap);
		printf("TWEET #%s fue Trending Topic con %zu RTs.\n", celda->clave, celda->rts);
		//printf("APARICIONES: %zu\n\n", celda->rts);
		//cell_destruir(celda);
	}
	printf("\n\n\n");
	heap_destruir(heap, NULL);
	//printf("OK Destruir HEAP\n");
}