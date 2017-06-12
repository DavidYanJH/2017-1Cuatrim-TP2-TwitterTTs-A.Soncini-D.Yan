#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HASHMIN 997 // Capacidad Inicial y Minima de la Tabla de Hash
#define NUMPRIMO 5381  // Numero Primo en la funcion de hashing
#define PERCENT 100 // Constante Numerico en Calculo de Factor de Carga
#define COEFICIENTE 2 // Constante de Variacion en Redimension de la Capacidad de Hash
#define MAXCARGA 200 // Maximo Factor de Carga
#define MINCARGA 20 // Minimo Factor de Carga
#define BITSHIFT 5 // Salto d bits a la izquierda en funcion de Hash

struct hash {
	lista_t** tabla;
	size_t capacidad;
	size_t existentes;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
	const hash_t* hash;
	lista_iter_t* lista_iter;
	size_t index_actual;
	size_t iteradas;
}; 

typedef struct nodo_hash {
	char* clave;
	void* dato;
} celda_t;

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


size_t hashing(const char *str, size_t hash_capacidad) {
	unsigned long hash = NUMPRIMO;
    int c;
    while ((c = *str++)) {
    	hash = ((hash << BITSHIFT) + hash) + c; /*hash * 33 + c*/
    }
    	return (hash % hash_capacidad);
}

lista_iter_t* busqueda_hash_cell(hash_t* hash, const char* clave, size_t* index) {
	*index = hashing(clave, hash->capacidad);
	
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

celda_t* cargar_hash_cell(const char* clave, void* dato) {
	celda_t* celda = malloc(sizeof(celda_t));
	if (!celda) return NULL;
	celda->clave = malloc(sizeof(char)* (strlen(clave) + 1));
	if (!celda->clave) {
		free(celda);
		return NULL;
	}
	strcpy(celda->clave, clave);
	celda->dato = dato;
	return celda;
}

void cell_destruir(celda_t* cell, hash_destruir_dato_t destruir_dato) {
	if (destruir_dato) destruir_dato(cell->dato);
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

bool hash_rehashing(hash_t* hash, lista_t** new_tabla, size_t new_capacidad) {
	bool OK = true;
	size_t i = 0;
	lista_iter_t* iter = NULL;
	celda_t* cell = NULL;
	size_t new_index;

	lista_t** old_tabla = hash->tabla;
	hash->tabla = new_tabla;

	size_t old_capacidad = hash->capacidad;
	hash->capacidad = new_capacidad;

	while ((OK) && (i < old_capacidad)) {
		iter = lista_iter_crear(old_tabla[i]);
		if (!iter) OK = false;
		while ((OK) && (!lista_iter_al_final(iter))) {
			cell = lista_iter_ver_actual(iter);
			new_index = hashing(cell->clave, hash->capacidad);
			OK = lista_insertar_ultimo(hash->tabla[new_index], cell);
			if (!OK) break;
			lista_iter_avanzar(iter);
		}
		lista_iter_destruir(iter);
		i++;
	}
	if (OK) tabla_destruir(old_tabla, old_capacidad);
	else {
		hash->tabla = old_tabla;
		hash->capacidad = old_capacidad;
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
	if (!new_tabla) return false;
	if (!hash_rehashing(hash, new_tabla, new_capacidad)) {
        tabla_destruir(new_tabla, new_capacidad);
		return false;
	}
	return true;
}

/* ******************************************************************
 *                   PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato) {
 	hash_t* hash = malloc(sizeof(hash_t));
 	if (!hash) return NULL;
 	hash->tabla = crear_hash_tabla(HASHMIN);
 	if (!hash->tabla) {
 		free(hash);
 		return NULL;
 	}
 	hash->capacidad = HASHMIN;
 	hash->existentes = 0;
 	hash->destruir_dato = destruir_dato;
 	return hash;
 }

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	hash_redimensionar(hash);
 	size_t index;
 	celda_t* celda = NULL;
 	lista_iter_t* iter = busqueda_hash_cell(hash, clave, &index);
 	if (!iter) return false;
 	celda = lista_iter_ver_actual(iter);
 	if (celda) {
 		if (hash->destruir_dato) 
			hash->destruir_dato(celda->dato);
		celda->dato = dato;
		lista_iter_destruir(iter);
		return true;
 	}
 	lista_iter_destruir(iter);
 	celda = cargar_hash_cell(clave, dato);
 	if (!celda)	return false;
 	lista_insertar_ultimo(hash->tabla[index], celda);
 	hash->existentes++;
 	return true;
 }

void *hash_borrar(hash_t *hash, const char *clave) {
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
}

void *hash_obtener(const hash_t *hash, const char *clave) {
	size_t index;
 	lista_iter_t* iter = busqueda_hash_cell((hash_t*) hash, (char*) clave, &index);
 	if (!iter) return NULL;
 	celda_t* celda = lista_iter_ver_actual(iter);
 	lista_iter_destruir(iter);
 	if (!celda) return NULL;
 	return celda->dato;
}

bool hash_pertenece(const hash_t* hash, const char* clave) {
	size_t index;
	lista_iter_t* iter = busqueda_hash_cell((hash_t*) hash, (char*) clave, &index);
	if (!iter) return false;
	celda_t* celda = lista_iter_ver_actual(iter);
	lista_iter_destruir(iter);
	if (!celda) return false;
	else return true;
}

size_t hash_cantidad(const hash_t* hash) {
	return hash->existentes;
}

void hash_destruir(hash_t *hash) {
	celda_t* cell = NULL;
	size_t index = 0;
	for (; index < hash->capacidad; index++) {
		while (!lista_esta_vacia(hash->tabla[index])){
			cell = lista_borrar_primero(hash->tabla[index]);
			cell_destruir(cell, hash->destruir_dato);
		}
		lista_destruir(hash->tabla[index], NULL);
	}
	free(hash->tabla);
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

char *hash_iter_ver_actual (const hash_iter_t *iter) {
	if (hash_iter_al_final(iter)) return NULL;
	const celda_t* celda = lista_iter_ver_actual(iter->lista_iter);
	if (!celda) return NULL;
	return celda->clave;
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