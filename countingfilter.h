#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ******************************************************************
 *                    COUNTING BLOOM FILTER TDA
 * *****************************************************************/

 /* Esta version del TDA se basa en 3 funciones de hashing y 3 arreglos.
  * El TDA puede generar un struct en base a una clave con los punteros
  * a cada arreglo para la clave dada.
 */


 /* Struct general del TDA
 */
typedef struct counting_filter {
	size_t* tabla1;
	size_t* tabla2;
	size_t* tabla3;
	size_t capacidad;
} counting_filter_t;


/* Struct de nodo para una clave
 */
typedef struct nodo_counter {
	size_t* pt1;
	size_t* pt2;
	size_t* pt3;
} nodo_counting_t;

/* Crea la estructura del TDA.
 * PRE: Ninguna, aunque se espera que el valor counting_size sea mayor a cero.
 * POST: Se creo la estructura y se inicializo todos los arreglos en cero.
 */
counting_filter_t* counting_filter_crear(size_t counting_size);

/* Crea la estructura de NODO para una clave.
 * PRE: TDA principal creado correcctamente.
 * POST: Se almaceno en cada campo de la estructura un puntero a cada posicion de los arreglos
 * relacionados con la clave otorgada.
 */
nodo_counting_t* counting_filter_generar_nodo(counting_filter_t* counting_filter, char* clave);

/* Destruye la estructura del TDA.
 */
void counting_filter_destruir(counting_filter_t* counting_filter);