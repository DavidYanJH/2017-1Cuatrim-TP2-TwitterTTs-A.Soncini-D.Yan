#define _GNU_SOURCE

#include "testing.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cbf.h"

#define MAX_ARGUMENTS 3

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/
int main(int argc, char* argv[]) {
	
	if (argc != MAX_ARGUMENTS) {
		printf("Error! Debe pasar solo dos argumento...\n");
		return 1;
	}

	size_t maxlines = atoi(argv[1]);		// Cantidad de lineas maximas consideradas para analisis
	size_t maxtts = atoi(argv[2]);			// Cantidad de Trending Topics a considerar
	size_t buffersize = 0;					// Bytes leidos en getline
	char* strline = NULL;					// Almacena puntero a nueva linea leida
	size_t linebuffer = 0;					// Tamano de buffer actual para lectura de nueva linea
	char* token;							// Almacena puntero a cada token de una linea
	size_t linecount;						// Contador de lineas leidas
	char delimiter[2] = ",";

	if (maxlines == 0 || maxtts == 0) {
		printf("Error! Los argumentos deben ser numericos y mayores a cero...\n");
		return 1;
	}

	linebuffer = getline(&strline, &buffersize, stdin);
	while (linebuffer != -1) {
		cbf_t* cbf = cbf_crear();
		if (!cbf) {
			printf("Memoria insuficiente. Abortando...\n");
			return 1;
		}
		linecount = 0;
		while ((linebuffer != -1) && (linecount < maxlines)) {
			linecount++;
			if (strline[linebuffer - 1] == '\n')
				strline[linebuffer - 1] = '\0';
			token = strtok(strline, delimiter); // Obviamos el nombre de usuario.
			token = strtok(NULL, delimiter);
			while  (token) {
				if (!cbf_guardar(cbf, token)) {
					printf("Memoria insuficiente. Abortando...\n");
					cbf_destruir(cbf);
					return 1;
				}
				token = strtok(NULL, delimiter);
			}
			linebuffer = getline(&strline, &buffersize, stdin);
		}
		imprimirTrendingTopics(cbf, maxtts);
		cbf_destruir(cbf);
	}
    return 0;
}
