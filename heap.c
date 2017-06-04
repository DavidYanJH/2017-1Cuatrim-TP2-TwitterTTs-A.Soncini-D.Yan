#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "hash.h"

#define CAPAC_INICIAL 36
#define COEF_REDIM 2
#define BASE_REDUC 4

struct heap {
	void** datos;
	cmp_func_t cmp;
	size_t cantidad;
	size_t capacidad;
};

/* ******************************************************************
 *                  FUNCIONES AUXILIARES DEL HEAP
 * *****************************************************************/
size_t obtener_padre(size_t index) {
	return (index - 1) / 2;
}

size_t obtener_hijo_izquierdo(size_t index) {	
	return index * 2 + 1;
}

size_t obtener_hijo_derecho(size_t index) {	
	return index * 2 + 2;
}

void swap_vectorial(void** vector, size_t x,  size_t y) {
	void* aux = vector[x];
	vector[x] = vector[y];
	vector[y] = aux;
}

bool heap_redimensionar(heap_t* heap, size_t new_capacidad) 
{
	void** new_datos = realloc(heap->datos, sizeof(void*) * new_capacidad);
	if (!new_datos) return false;
	heap->datos = new_datos;
	heap->capacidad = new_capacidad;
	return true;
}

void upheap(heap_t* heap, size_t actual) 
{
	if (actual == 0) return;
	size_t padre = obtener_padre(actual);
	if (heap->cmp(heap->datos[actual], heap->datos[padre]) < 0) {
		swap_vectorial(heap->datos, actual, padre);
		upheap(heap, padre);
	}
}

void downheap(void* array[], size_t actual, size_t lenght, cmp_func_t cmp)
{
	if (actual == lenght) return;
	size_t maximo = actual;
	size_t hijo_izq = obtener_hijo_izquierdo(actual);
	size_t hijo_der = obtener_hijo_derecho(actual);
	if (hijo_izq < lenght)
		if (cmp(array[hijo_izq], array[maximo]) < 0)
			maximo = hijo_izq;
	if (hijo_der < lenght)
		if (cmp(array[hijo_der], array[maximo]) < 0)
			maximo = hijo_der;
	if (maximo != actual) {
		swap_vectorial(array, maximo, actual);
		downheap(array, maximo, lenght, cmp);
	}
}

void max_heapify(void* array[], size_t lenght, cmp_func_t cmp)
{
	size_t nodo_inicial = lenght/2-1;
	for (size_t index = nodo_inicial; index > 0; index--)
		downheap(array, index, lenght, cmp);
	downheap(array, 0, lenght, cmp);
}


void heap_sort(void* array[], size_t lenght, cmp_func_t cmp)
{
	if (lenght <= 1) return;
	max_heapify(array, lenght, cmp);
	for (size_t index = lenght-1; index > 0; --index)
	{
		swap_vectorial(array, 0, index);
		downheap(array, 0, index, cmp);
	}
}



/* ******************************************************************
 *                        PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t* heap_crear(cmp_func_t cmp) 
{
	if (!cmp) return NULL;
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;
	heap->datos = malloc(sizeof(void*) * CAPAC_INICIAL);
	if (!heap->datos) {
		free(heap);
		return NULL;
	}
	heap->cmp = cmp;
	heap->cantidad = 0;
	heap->capacidad = CAPAC_INICIAL;
	return heap;
}


size_t heap_cantidad(const heap_t* heap) 
{
	if (!heap) return 0;
	return heap->cantidad;
}


bool heap_esta_vacio(const heap_t* heap)
{
	if (!heap) return true;
	return (heap->cantidad == 0);
}


void* heap_ver_max(const heap_t* heap)
{
	if (heap_esta_vacio(heap)) return NULL;
	return heap->datos[0];
}


void heap_destruir(heap_t* heap, void destruir_elem(void* e))
{
	if (!heap_esta_vacio(heap) && destruir_elem) {
		for (size_t index = 0; index < heap->cantidad; ++index) 
			destruir_elem(heap->datos[index]);
	}
	if (heap) {
		free(heap->datos);
		free(heap);
	}
}


bool heap_encolar(heap_t* heap, void* elem) 
{
	if (!heap || !elem) return false;
	if (heap->cantidad == heap->capacidad) {
		size_t new_capacidad = heap->capacidad * COEF_REDIM;
		if (!heap_redimensionar(heap, new_capacidad)) return false;
	}
	heap->datos[heap->cantidad] = elem;
	upheap(heap, heap->cantidad);
	++heap->cantidad;
	return true;
}


void* heap_desencolar(heap_t* heap)
{
	if (heap_esta_vacio(heap)) return NULL;
	--heap->cantidad;
	if (heap->cantidad == 0) return heap->datos[0];
	swap_vectorial(heap->datos, 0, heap->cantidad);
	downheap(heap->datos, 0, heap->cantidad, heap->cmp);
	// Evaluación de la Necesidad de Redimensionar - Reducir la Capacidad del Heap cuando 
	// la cantidad de elementos sea menor o igual a un cuarto (25%) de la capacidad actual...
	if (heap->capacidad > CAPAC_INICIAL && (heap->cantidad * BASE_REDUC) <= heap->capacidad) {
		size_t new_capacidad = heap->capacidad / COEF_REDIM;
		heap_redimensionar(heap, new_capacidad);
	}
	return heap->datos[heap->cantidad];
}


heap_t* heap_crear_arr(void* arreglo[], size_t n, cmp_func_t cmp)
{
	heap_t* heap = heap_crear(cmp);
	if (!heap) return NULL;
	for (size_t index = 0; index < n; ++index) 
	{
		if (heap->cantidad == heap->capacidad) {
			size_t new_capacidad = heap->capacidad * COEF_REDIM;
			if (!heap_redimensionar(heap, new_capacidad)) break;
		}
		heap->datos[index] = arreglo[index];
		++heap->cantidad;
	}
	// En caso de haber insertado correctamente la totalidad de elementos del arreglo
	// se realiza un heapify del array heap->datos con el fin de adquirir la forma de 
	// un heap, luego finalmente se regresa el puntero del heap creado...
	if (heap->cantidad == n) {
		max_heapify(heap->datos, heap->cantidad, heap->cmp);
		return heap;
	}
	// En caso de sin inicializar el heap con la totalidad de elementos del arreglo 
	// debido a la falta de memoria en ejecución deberia liberar las memorias utilizadas
	// debidamente, finalmente retorna un NULL...
	free(heap->datos);
	free(heap);
	return NULL;
}