// Alternativas de Funciones de Hashing

size_t hashing(const char* s, size_t hash_capacidad)
{	
	size_t hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + NUMPRIMO * hashval;
	return hashval % hash_capacidad;
}


size_t hashing(const char *str, size_t hash_capacidad) {
	unsigned long hash = NUMPRIMO;
    int c;
    while ((c = *str++)) {
    	hash = ((hash << BITSHIFT) + hash) + c; /*hash * 33 + c*/
    }
    	return (hash % hash_capacidad);
}