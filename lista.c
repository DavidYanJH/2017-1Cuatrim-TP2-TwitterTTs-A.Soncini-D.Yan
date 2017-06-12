#include "lista.h"
#include <stdlib.h>

/* ******************************************************************
 *                   ANDRES ELISEO SONCINI
 *                   PADRON: 100265
 *                   CORRECTOR: Martín Buchwald
 * *****************************************************************/

typedef struct nodo_lista nodo_lista_t;

struct nodo_lista {
	void** data;
	nodo_lista_t* next;
};

struct lista {
	nodo_lista_t* first;
	nodo_lista_t* last;
	size_t lenght;
};

struct lista_iter {
	lista_t* lista;
	nodo_lista_t* actual;
	nodo_lista_t* anterior;	
};

lista_t* lista_crear(void) {
	lista_t* lista = malloc(sizeof(lista_t));
	if (lista != NULL) {
		lista->first = NULL;
		lista->last = NULL;
		lista->lenght = 0;
	}
	return lista;
}

// Devuelve un nodo con su apuntador al dato por el cual se creó y
// el aputardor a next en NULL. Si falló malloc, devuelve null.
nodo_lista_t* nodo_crear(void* dato) {
	nodo_lista_t* nodo = malloc(sizeof(nodo_lista_t));
	if (nodo != NULL) {
		nodo->next = NULL;
		nodo->data = dato;
	}	
	return nodo;
}

bool lista_esta_vacia(const lista_t *lista) {
	return (lista->lenght == 0);
}

bool lista_insertar_primero(lista_t *lista, void *dato) {
	nodo_lista_t* nodo = nodo_crear(dato);
	if (nodo == NULL) {
		return false;
	}
	nodo->next = lista->first;
	lista->first = nodo;
	if (lista_esta_vacia(lista)) {
		lista->last = nodo;
	}
	(lista->lenght)++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato) {
	if (lista_esta_vacia(lista)) {
		return lista_insertar_primero(lista, dato);
	}
	else {
		nodo_lista_t* nodo = nodo_crear(dato);
		if (nodo == NULL) {
			return false;
		}
		(lista->last)->next = nodo;
		lista->last = nodo;
		(lista->lenght)++;
		return true;
	}
}

void *lista_borrar_primero(lista_t *lista) {
	void* dato = lista_ver_primero(lista);
	if (!lista_esta_vacia(lista)) {
		nodo_lista_t* nodo = lista->first;
		lista->first = nodo->next;
		free(nodo);
		(lista->lenght)--;	
	}
	return dato;
}

void* lista_ver_primero(const lista_t *lista) {
	if (lista_esta_vacia(lista)) {
		return NULL;
	}
	return (lista->first)->data;
}

void *lista_ver_ultimo(const lista_t* lista) {
	if (lista_esta_vacia(lista)) {
		return NULL;
	}
	return (lista->last)->data;
}

size_t lista_largo(const lista_t *lista) {
	return lista->lenght;
}

void lista_destruir(lista_t* lista, void destruir_dato(void*)) {
	while (!lista_esta_vacia(lista)) {
		void* prim = lista_borrar_primero(lista);
		if (destruir_dato != NULL) {
			destruir_dato(prim);
		}
	}
	free(lista);
}

/* *****************************************************************
 *                     PRIMITIVAS DE ITERACION
 * *****************************************************************/

lista_iter_t* lista_iter_crear(lista_t *lista) {
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter != NULL) {
		iter->lista = lista;
		iter->actual = lista->first;
		iter->anterior = NULL;
	}
	return iter;
}

bool lista_iter_al_final(const lista_iter_t *iter) {
	return (iter->actual == NULL);
}

void* lista_iter_ver_actual(const lista_iter_t *iter) {
	if (lista_iter_al_final(iter)) {
		return NULL;
	}
	return (iter->actual)->data;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
	if (lista_iter_al_final(iter)) {
		return false;
	}
	iter->anterior = iter->actual;
	iter->actual = (iter->actual)->next;
	return true;
}

void lista_iter_destruir(lista_iter_t *iter) {
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
	nodo_lista_t* nodo = nodo_crear(dato);
	if (nodo == NULL) {
		return false;
	}
	if (iter->actual == NULL) {
		iter->lista->last = nodo;
	}
	if (iter->anterior == NULL) {
		iter->lista->first = nodo;
	}
	else {
		iter->anterior->next = nodo;	
	}
	nodo->next = iter->actual;
	iter->actual = nodo;
	(iter->lista)->lenght++;
	return true;
}

void* lista_iter_borrar(lista_iter_t *iter) {
	if (lista_iter_al_final(iter)) {
		return NULL;
	}
	nodo_lista_t* nodo = iter->actual;
	void* dato = iter->actual->data;
	if (iter->anterior == NULL) {
		iter->lista->first = nodo->next;
	}
	else {
		iter->anterior->next = nodo->next;
	}
	if (nodo->next == NULL) {
		iter->lista->last = iter->anterior;
	}
	iter->actual = nodo->next;
	free(nodo);
	(iter->lista)->lenght--;
	return dato;
}

void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra) {
	nodo_lista_t* nodo = lista->first;
	while ((nodo != NULL) && visitar(nodo->data, extra)) {
		nodo = nodo->next;
	}
}