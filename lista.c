#include "lista.h"
#include <stdlib.h>

/**********************************************************
*    DECLARACION DE LAS ESTRUCTURAS Y FUNCIONES AUX       *
**********************************************************/

typedef struct nodo_lista {
	void** dato;
	struct nodo_lista* siguiente;
} nodo_t;

struct lista {
	nodo_t* primero;
	nodo_t* ultimo;
	size_t largo;
};

struct lista_iter {
	nodo_t* actual;
	nodo_t* anterior;
	lista_t* lista;
};


nodo_t* nodo_crear(void* dato)
{
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->dato = dato;
    nodo->siguiente = NULL;
	return nodo;
}

/**********************************************************
*            PRIMITIVAS DE LA LISTA ENLAZADA              *
**********************************************************/

lista_t* lista_crear(void)
{
	lista_t* lista = malloc(sizeof(lista_t));
	if (!lista) return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;
	return lista;
}


size_t lista_largo(const lista_t* lista)
{
	if (!lista) return 0;
	return lista->largo;
}


bool lista_esta_vacia(const lista_t* lista)
{
	if (!lista) return true;
	return lista->largo == 0;
}


bool lista_insertar_primero(lista_t* lista, void* dato)
{
	if (!lista) return false;
	nodo_t* nodo = nodo_crear(dato);
	if (!nodo) return false;
	nodo->siguiente = lista->primero;
	if (lista_esta_vacia(lista)) lista->ultimo = nodo;
	lista->primero = nodo;
	lista->largo++;
	return true;
}


bool lista_insertar_ultimo(lista_t* lista, void* dato)
{
	if (!lista) return false;
	nodo_t* nodo = nodo_crear(dato);
	if (!nodo) return false;
	if (lista_esta_vacia(lista)) lista->primero = nodo;
	else lista->ultimo->siguiente = nodo;
	lista->ultimo = nodo;
	lista->largo++;
	return true;
}


void* lista_ver_primero(const lista_t* lista)
{
	if (!lista || lista_esta_vacia(lista)) return NULL;
	return lista->primero->dato;
}


void* lista_ver_ultimo(const lista_t* lista)
{
	if (!lista || lista_esta_vacia(lista)) return NULL;
	return lista->ultimo->dato;
}


void* lista_borrar_primero(lista_t* lista)
{
	if (!lista || lista_esta_vacia(lista)) return NULL;
	nodo_t* aux_nodo = lista->primero;
	void* aux_dato = aux_nodo->dato;
	lista->primero = aux_nodo->siguiente;
	free(aux_nodo);
	lista->largo--;
	return aux_dato;
}


void lista_destruir(lista_t* lista, void destruir_dato(void *))
{
	if (!lista) return;
	while (!lista_esta_vacia(lista))
	{
		void* aux = lista_borrar_primero(lista);
		if (destruir_dato != NULL) destruir_dato(aux);
	}
    free(lista);
}

/**********************************************************
*   PRIMITIVAS DEL ITERADOR EXTERNO DE LA LISTA ENLAZADA  *
**********************************************************/

lista_iter_t* lista_iter_crear(lista_t* lista)
{
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter == NULL) return NULL;
	iter->anterior = NULL;
	iter->actual = lista->primero;
	iter->lista = lista;
	return iter;
}


bool lista_iter_al_final(const lista_iter_t* iter)
{
	return iter->actual == NULL;
}


bool lista_iter_avanzar(lista_iter_t* iter)
{
	if (!lista_iter_al_final(iter))
	{
		iter->anterior = iter->actual;
		iter->actual = iter->actual->siguiente;
		return true;
	}
	return false;
}


void* lista_iter_ver_actual(const lista_iter_t* iter)
{
	if (lista_iter_al_final(iter)) return NULL;
	return iter->actual->dato;
}


bool lista_iter_insertar(lista_iter_t* iter, void* dato)
{
	nodo_t* nodo = nodo_crear(dato);
	if (!nodo) return false;
	nodo->siguiente = iter->actual;
	iter->lista->largo++;
	if (iter->actual == iter->lista->primero)
	{
		iter->lista->primero = nodo;
		iter->actual = nodo;
		return true;
	}	
	if (iter->actual == NULL) iter->lista->ultimo = nodo;
	iter->actual = nodo;
	iter->anterior->siguiente = iter->actual;
	return true;
}


void* lista_iter_borrar(lista_iter_t* iter)
{
	if (lista_iter_al_final(iter)) return NULL;
	nodo_t* aux_nodo = iter->actual;
	void* aux_dato = iter->actual->dato;
	if (iter->actual == iter->lista->primero) 
	{
		iter->lista->primero = iter->actual->siguiente;
	}
	else if (iter->actual == iter->lista->ultimo)
	{
        iter->anterior->siguiente = NULL; 
		iter->lista->ultimo = iter->anterior;
	}
	else iter->anterior->siguiente = iter->actual->siguiente;
	iter->actual = iter->actual->siguiente;
	free(aux_nodo);
	iter->lista->largo--;
	return aux_dato;
}

void lista_iter_destruir(lista_iter_t* iter)
{
	if (iter != NULL) free(iter);
}

/**********************************************************
*   PRIMITIVAS DEL ITERADOR INTERNO DE LA LISTA ENLAZADA  *
**********************************************************/

void lista_iterar(lista_t* lista, bool (*visitar)(void* dato, void* extra), void* extra)
{
	
	nodo_t* nodo = lista->primero;
	while (nodo != NULL && visitar(nodo->dato, extra)) 
		nodo = nodo->siguiente;
}