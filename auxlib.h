#ifndef AUXLIB_H
#define AUXLIB_H

#include "hash.h"

/* Struct para nodo de Heap de minimo.
 * Clave: Contiene el nombre de un hashtag.
 * tts: Contiene el minimo contador para ese hashtag, obtenido con "obtenermin", de esta misma libbreria.
 */
typedef struct nodo_heap {
	char* clave;
	size_t* tts;
} nodo_heap_t;

/* Obtiene y retorna el puntero al menor contador de un hashtag del TDA Counting Filter.
 * Pre: El nodo fue creado mediante la funcion counting_filter_generar_nodo.
 * Post: Se obtuvo el minimo contador para ese nodo.
 */
size_t* obtenermin(const nodo_counting_t* nodo);

/* Funcion de comparacion para nodo de Heap de minimo.
 * Pre: La funcion debe recibir punteros a nodo_heap_t*, validos.
 * Post: Se retorna un valor < 0, si a es mayor a b; > 0, si a es menor a b; 0 en caso de igualdad.
 */
int cmprts(const void* a, const void* b);

/* Imrpime los maxtts Trending Topics.
 * Pre: Todos los parametros recibidos fueron creados con exito por la funcion procesarEntrada.
 * Post: Se imprimen los maxtts Trending Topics hasta el momento.
 */
void imprimirTrendingTopics(hash_t* hash, counting_filter_t* counting_filter, size_t maxtts);

/* Procesa el archivo de entrada y muestra las estadisticas de para los maxtts Trending Topics.
 * cada maxlines.
 * Pre: .
 * Post: .
 */
void procesarEntrada(counting_filter_t* counting_filter, size_t maxlines, size_t maxtts);

#endif // AUXLIB_H