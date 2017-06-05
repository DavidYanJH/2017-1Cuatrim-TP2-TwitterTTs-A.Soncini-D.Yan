#ifndef CBF_H
#define CBF_H

#include <stdbool.h>
#include <stddef.h>
#include "lista.h"

// Los structs deben llamarse "cbf" y "cbf_iter".
typedef struct cbf cbf_t;
typedef struct cbf_iter cbf_iter_t;

/* Crea el hash
 */
cbf_t* cbf_crear();

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool cbf_guardar(cbf_t *cbf, const char *clave);

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *cbf_borrar(cbf_t *cbf, const char *clave);


/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t cbf_cantidad(const cbf_t *cbf);

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void cbf_destruir(cbf_t *cbf);

/* Iterador del hash */

// Crea iterador
cbf_iter_t *cbf_iter_crear(const cbf_t *cbf);

// Avanza iterador
bool cbf_iter_avanzar(cbf_iter_t *iter);

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
void* cbf_iter_ver_actual(const cbf_iter_t *iter);

// Comprueba si terminó la iteración
bool cbf_iter_al_final(const cbf_iter_t *iter);

// Destruye iterador
void cbf_iter_destruir(cbf_iter_t* iter);

void imprimirTrendingTopics(cbf_t* hash, size_t k);

#endif // CBF_H
