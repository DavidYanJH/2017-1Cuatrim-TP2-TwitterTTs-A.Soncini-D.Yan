#define _GNU_SOURCE

#include "testing.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/
int main(int argc, char* argv[])
{
    printf("~~~ INICIO MAIN ~~~\n");

	size_t buffersize = 0;
	char* strline = NULL;
	size_t code = 0;
	char* aux;
	size_t linecount = atoi(argv[1]);
	size_t k = atoi(argv[2]);
	size_t i;

	code = getline(&strline, &buffersize, stdin);
	while (code != -1) {
		hash_t* hash = hash_crear();
		i = 0;
		while ((code != -1) && (i < linecount)) {
			i++;
			if (strline[code - 1] == '\n')
				strline[code - 1] = '\0';
			aux = strtok(strline, ","); // Obviamos el nombre de usuario.
			aux = strtok(NULL, ",");
			while  (aux) {
				hash_guardar(hash, aux);
				aux = strtok(NULL, ",");
			}
			code = getline(&strline, &buffersize, stdin);
		}
		imprimirTrendingTopics(hash, k);
		hash_destruir(hash);
	}
    return 0;
}
