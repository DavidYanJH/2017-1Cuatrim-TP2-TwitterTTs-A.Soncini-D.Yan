#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "hash.h"
#include "lista.h"

#define NUM_ARGUM 2

/**************************************************************************************
 *                   FUNCIONES AUXILIARES DEL PROGRAMA PRINCIPAL                        
 *************************************************************************************/

void procesar_entradas(FILE* arch_lectura, hash_t* hash, size_t* max_tags)
{
	ssize_t leidos = 0;
	char* line = NULL;
	size_t aux;
	char* usuario = NULL;
	char* delimiter = ",";
	size_t* dato = NULL;

	while ((leidos = getline(&line, &aux, arch_lectura)) > 0) {
		usuario = strtok(line, delimiter);
		if (!hash_pertenece(hash, usuario)) {
			dato = calloc(1, sizeof(size_t));
			if (!dato) {
				hash_destruir(hash);
				fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
				exit(EXIT_FAILURE);
			}
			if (!hash_guardar(hash, usuario, dato)) {
				free(dato);
				hash_destruir(hash);
				fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
				exit(EXIT_FAILURE);
			}
		}
		dato = hash_obtener(hash, usuario);
		while (strtok(NULL, delimiter))
			++*dato;
		if (*dato > *max_tags) 
			*max_tags = *dato;
	}
	free(line);
}


void array_counting_destruir(lista_t** array, size_t capacidad)
{
	for (size_t index = 0; index < capacidad; ++index)
		lista_destruir(array[index], NULL);
	free(array);
}


lista_t** array_counting_crear(size_t capacidad)
{
	lista_t** array = malloc(sizeof(lista_t*)*capacidad);
	if (!array) return NULL;
	for (size_t index = 0; index < capacidad; ++index) {
		array[index] = lista_crear();
		if (!array[index]) {
			array_counting_destruir(array, index);
			return NULL;
		}
	}
	return array;
}	


void generar_salidas(hash_t* hash, size_t max_tags)	
{		
	hash_iter_t* hash_iter = hash_iter_crear(hash);
	if (!hash_iter) {
		hash_destruir(hash);
		fprintf(stderr, "Memoria insuficiente. Abortando...\n");
		exit(EXIT_FAILURE);
	}

	size_t capacidad = max_tags + 1;
	lista_t** array_counting = array_counting_crear(capacidad);
	if (!array_counting) {
		hash_iter_destruir(hash_iter);
		hash_destruir(hash);
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		exit(EXIT_FAILURE);
	}
	char* usuario = NULL;
	size_t* num_tags = NULL;
	while (!hash_iter_al_final(hash_iter)) {
		usuario = hash_iter_ver_actual(hash_iter);
		num_tags = hash_obtener(hash, usuario);
		if (!lista_insertar_ultimo(array_counting[*num_tags], usuario)) {
			hash_iter_destruir(hash_iter);
			hash_destruir(hash);
			array_counting_destruir(array_counting, capacidad);
			fprintf(stderr, "Memoria insuficiente. Abortando...\n");
			exit(EXIT_FAILURE);
		}
		hash_iter_avanzar(hash_iter);
	}
	hash_iter_destruir(hash_iter);

	printf("***********ESTADISTICAS DE USUARIOS (CANTIDAD DE TAGS)***********\n");
	for (size_t index = 0; index < capacidad; ++index) {
		if (!lista_esta_vacia(array_counting[index])){
			printf("%zd: ", index);
			while (!lista_esta_vacia(array_counting[index])){
				usuario = lista_borrar_primero(array_counting[index]);
				printf("%s", usuario);
				if (!lista_esta_vacia(array_counting[index])) printf(", ");
			}
			printf("\n");
		}
	}
	array_counting_destruir(array_counting, capacidad);
}


/* ************************************************************************************
 *                                PROGRAMA PRINCIPAL
 * ************************************************************************************/

int main(int argc, char* argv[])
{
	if (argc != NUM_ARGUM) {
		fprintf(stderr, "Error en Cantidad de Argumentos al invocar el Programa\n");
		exit(EXIT_FAILURE);
	}

	FILE* arch_lectura = fopen(argv[1], "r");
	if (!arch_lectura) {
		fprintf(stderr, "Error al Abrir el Archivo de Lectura\n");
		exit(EXIT_FAILURE);
	}

	hash_t* hash = hash_crear(free);
	if (!hash) {
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		exit(EXIT_FAILURE); 
	}

	size_t max_tags = 0;
	procesar_entradas(arch_lectura, hash, &max_tags);
	fclose(arch_lectura);
	if (max_tags > 0) generar_salidas(hash, max_tags);
	hash_destruir(hash);
	return EXIT_SUCCESS;
}