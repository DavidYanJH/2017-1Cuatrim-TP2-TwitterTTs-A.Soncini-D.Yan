#include <stdio.h>
#include <stdlib.h>
#include "countingfilter.h"
#include "auxlib.h"

#define MAX_ARGUMENTS 3
#define COUNTING_SIZE 9887


/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/
int main(int argc, char* argv[]) {
	
	if (argc != MAX_ARGUMENTS) {
		fprintf(stderr, "Error! Debe pasar solo dos argumento...\n");
		exit(EXIT_FAILURE);
	}

	size_t maxlines = atoi(argv[1]);		// Cantidad de lineas maximas consideradas para analisis
	size_t maxtts = atoi(argv[2]);			// Cantidad de Trending Topics a considerar


	if (maxlines == 0 || maxtts == 0) {
		fprintf(stderr, "Error! Los argumentos deben ser numericos y mayores a cero...\n");
		exit(EXIT_FAILURE);
	}
	
	counting_filter_t* counting_filter = counting_filter_crear(COUNTING_SIZE);

	if (!counting_filter) {
		fprintf(stderr, "Memoria Insuficiente. Abortando...\n");
		exit(EXIT_FAILURE);	
	}

	procesarEntrada(counting_filter, maxlines, maxtts);

	counting_filter_destruir(counting_filter);

    exit(EXIT_SUCCESS);
}
