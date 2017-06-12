#include "pila.h"
#include <stdlib.h>

/* ******************************************************************
 *                   ANDRES ELISEO SONCINI
 *                   PADRON: 100265
 *                   CORRECTOR: Martín Buchwald
 * *****************************************************************/

#define TAM_INICIAL 10
#define FACTOR_CRECIMIENTO 2
#define UMBRAL_DECRECIMIENTO (FACTOR_CRECIMIENTO * 2)
#define FACTOR_DECRECIMIENTO FACTOR_CRECIMIENTO

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...
pila_t* pila_crear(void) {
    pila_t* pila = malloc(sizeof(pila_t));
    if (pila == NULL) {
        return NULL;
    }

    pila->datos = malloc(TAM_INICIAL * sizeof(void*));
    if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
    pila->capacidad = TAM_INICIAL;
    pila->cantidad = 0;
    return pila;
}

void pila_destruir(pila_t *pila) {
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
    return (pila->cantidad == 0);
}

bool pila_redimensionar(pila_t *pila, size_t capacidad_nueva) {
    void** datos_nuevo = realloc(pila->datos,capacidad_nueva * sizeof(void*));
    if (datos_nuevo == NULL) {
        return false;
    }

    pila->datos = datos_nuevo;
    pila->capacidad = capacidad_nueva;
    return true;
}

bool pila_apilar(pila_t *pila, void* valor) {
    if (pila->cantidad == pila->capacidad) {
        if (!pila_redimensionar(pila, pila->capacidad * FACTOR_CRECIMIENTO)) {
            return false;
        }
    }
    pila->datos[pila->cantidad] = valor;
    ++(pila->cantidad);
    return true;
}

void* pila_ver_tope(const pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }
    return pila->datos[pila->cantidad - 1];
}

void* pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }
    pila->cantidad = (pila->cantidad - 1);
    void* tope = pila->datos[pila->cantidad];

    if ((pila->cantidad < pila->capacidad / UMBRAL_DECRECIMIENTO) && (pila->capacidad != TAM_INICIAL)) {
        pila_redimensionar(pila, pila->capacidad / FACTOR_DECRECIMIENTO);
    }
    return tope;
}
