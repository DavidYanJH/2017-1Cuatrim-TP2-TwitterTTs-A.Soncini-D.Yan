#ifndef AUXLIB_H
#define AUXLIB_H

#include "hash.h"

/* Struct para nodo de Heap de minimo.
 * Pre: .
 * Post: .
 */
typedef struct nodo_heap {
	char* clave;
	size_t* tts;
} nodo_heap_t;

/* Obtiene y retorna el puntero al menor contador de un elemento del TDA Counting Filter.
 * Pre: .
 * Post: .
 */
size_t* obtenermin(const nodo_counting_t* nodo);

/* Funcion de comparacion para nodo de Heap de minimo.
 * Pre: .
 * Post: .
 */
int cmprts(const void* a, const void* b);

/* Imrpime los maxtts Trending Topics.
 * Pre: .
 * Post: .
 */
void imprimirTrendingTopics(hash_t* hash, counting_filter_t* counting_filter, size_t maxtts);

/* Procesa el archivo de entrada y muestra las estadisticas de para los maxtts Trending Topics.
 * cada maxlines.
 * Pre: .
 * Post: .
 */
void procesarEntrada(counting_filter_t* counting_filter, size_t maxlines, size_t maxtts);

#endif // AUXLIB_H