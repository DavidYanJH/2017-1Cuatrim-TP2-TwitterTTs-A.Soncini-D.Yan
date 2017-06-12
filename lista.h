#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

struct lista;
typedef struct lista lista_t;

struct lista_iter;
typedef struct lista_iter lista_iter_t;


/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al inicio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista. El elemento se encuentra al inicio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista. El elemento se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void* lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
// No se modificó la lista.
void* lista_ver_primero(const lista_t *lista);

// Obtiene el valor del último elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del último, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el último elemento de la lista, cuando no está vacía.
// No se modificó la lista.
void* lista_ver_ultimo(const lista_t* lista);

// Obtiene el largo de la lista.
// Pre: la lista fue creada.
// Post: se devolvió el largo actual de la lista. Si la lista está vacía
// se devuelve cero. No se modificó la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void*));

/* *****************************************************************
 *                      PRIMITIVAS DE ITERACION
 * *****************************************************************/

// Crea un iterador.
// Pre: La lista fue creada.
// Post: Devuelve un iterador nuevo para la lista pasada por referencia.
lista_iter_t* lista_iter_crear(lista_t *lista);

// Evalua si se está al final de la lista.
// Pre: El iterador fue creado.
// Post: true o false, según se esté o no en el fin de la lista.
bool lista_iter_al_final(const lista_iter_t *iter);

// Devuelve un puntero al dato actual sobre el que se halla el iterador.
// Pre: El iterador fue creado.
// Post: devuelve un puntero al dato, o NULL si la lista está vacía o si
// el iterador esta al final de la lista
void* lista_iter_ver_actual(const lista_iter_t *iter);

// Avanza hacia el siguiete dato de la lista.
// Pre: El iterador fue creado.
// Post: Devuelve true si se pudo avanzar. Devuelve false si ya se 
// encontraba en fin de lista.
bool lista_iter_avanzar(lista_iter_t *iter);

// Destruye el iterador.
// Pre: Iterador fue creado.
// Post: Se eliminó el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un dato entre el dato actual y el anterior lista.
// Pre: El iterador fue creado.
// Post: La lista contiene un elemento más, que se halla entre 
// el anterior y el actual. Ahora, el elemento actual es el insertado.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra el dato actual de la lista.
// Pre: El iterador fueron creados.
// Post: Elimina el dato actual de la lista y devuelve el valor 
// del dato que contenía.
// Si la lista está vacía, devuelve NULL.
void* lista_iter_borrar(lista_iter_t *iter);

// Realiza las acciones que indique la función visitar sobre la lista, siempre
// que la misma devuelva true.
// El parámetro extra, le provee memoria de distintas iteraciones.
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);

#endif // LISTA_H