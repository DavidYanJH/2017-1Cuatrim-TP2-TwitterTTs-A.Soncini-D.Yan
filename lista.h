#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

// La Lista Enlazada está planteada como una lista de punteros genéricos

struct lista;
typedef struct lista lista_t;
struct lista_iter;
typedef struct lista_iter lista_iter_t;


/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista vacía.
// Post: Devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Obtiene la longitud de la lista o su cantidad de elementos.
// Pre: La lista fue creada.
// Post: Devuelve la longitud de la lista.
size_t lista_largo(const lista_t *lista);

// Pre: La lista fue creada.
// Post: Devuelve verdadero o falso, según si la lista tiene o no elementos
bool lista_esta_vacia(const lista_t *lista);

// Inserta un elemento en la primera posicion de la lista.
// En caso de error devuelve false.
// Pre: La lista fue creada.
// Post: Se ha insertado un elemento en la primera posicion de la lista,
// y por consiguiente dato se encuentra al principio de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un elemento en la ultima posicion de la lista.
// En caso de error devuelve false.
// Pre: La lista fue creada.
// Post Se ha insertado un elemento en la ultima posicion de la lista, 
// y por consiguiente dato se encuentra en el final de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Si la lista tiene elementos se obtiene el valor del primer elemento de la lista.
// En caso de una lista vacía devuelve NULL.
// Pre: La lista fue creada.
// Post: Se ha obtenido el primer elemento de la lista, cuando la misma no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Si la lista tiene elementos se obtiene el valor del ultimo elemento de la lista.
// En caso de una lista vacia devuelve NULL.
// Pre: La lista fue creada.
// Se ha obtenido el ultimo elemento de la lista, cuando no la misma no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Si la lista tiene elementos se elimina el primer elemento de la lista.
// En caso de una lista vacía devuelve NULL.
// Pre: La lista fue creada
// Post: Se ha eliminado el primer elemento de la lista.
void *lista_borrar_primero(lista_t *lista);

// Destruye la lista. Si la funcion recibe una funcion destruir_dato por
// parametro para cada elemento de la lista llama a destruir_dato; 
// en cambio se recibe como parametro NULL en caso de no utilizarse
// Pre: La lista fue creada. Ademas destruir_dato es una función capaz de 
// destruir cada uno de los elementos de la lista.
// Post: Se ha eliminado todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

// Iterador Iterno. Recibe una función callback "visitar" junto con el dato y un puntero extra,
// la misma devuelve true si se debe seguir iterando, false en caso contrario. El resultado de
// la funcion es entregado al usuario mediante el puntero extra. Si el usuario no necesita
// el extra debe pasarlo como un parametro NULL.
// Pre: La lista fue creada.
// Pos: la funcion lista_iterar itera internamente y aplica la funcion visitar a cada uno de 
// los elementos de la misma lista.
void lista_iterar(lista_t* lista, bool (*visitar) (void* dato, void* extra), void* extra);


// Pre: Tener una lista creada.
// Post: Crea un iterador. Si la lista está vacía el iterador apunta al final.
// En caso contrario, si la lista no está vacía el iterador apunta al principio.
// Si la operacion falla devuelve NULL.
lista_iter_t* lista_iter_crear(lista_t* lista);

// Pre: El iterador fue creado anteriormente.
// Post: Verifica si el iterador se encuentra al final de la lista.
// Devuelve true en caso verdadero, false en caso contrario.
bool lista_iter_al_final(const lista_iter_t* iter);

// Pre: El iterador fue creado anteriormente.
// Post: Si el iterador no se encuentra al final de la lista, 
// el mismo avanza una posicion devolviendo true.
// En cambio, si se encuentra al final de la lista, devuelve false y no avanza. 
bool lista_iter_avanzar(lista_iter_t* iter);

// Pre: El iterador fue creado anteriormente.
// Post: Obtiene el dato actual al que está apuntando si no está al final de la lista.
// Si se encuentra apuntando al final de la lista devuelve NULL.
void* lista_iter_ver_actual(const lista_iter_t* iter);

// Pre: El iterador fue creado anteriormente.
// Post: Inserta el dato en la posicion de la lista a la cual está apuntado.
// Si la operacion de insercion fue exitosa devuelve true, false en caso contrario.
bool lista_iter_insertar(lista_iter_t* iter, void* dato);

// Pre: El iterador fue creado anteriormente.
// Post: Elimina el dato en la posicion de la lista a la cual está apuntando.
// Finalmente devuelve el dato borrado de la lista al usuario.
void* lista_iter_borrar(lista_iter_t* iter);

// Pre: El iterador fue creado anteriormente.
// Post: Destruye el iterador creado una vez que su uso se haya terminado.
void lista_iter_destruir(lista_iter_t* iter);




/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y solamente
// pueden emplear la interfaz pública tal y como aparece en lista.h
// (las pruebas no pueden acceder a los miembros del struct lista).

// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);

#endif // LISTA_H