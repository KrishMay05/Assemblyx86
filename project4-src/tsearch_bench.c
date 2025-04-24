
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tsearch.h"

int
main()
{
	TreeSearch  tsearch;

	tsearch_init(&tsearch);

	// Generate random keys
	int number_keys = 5 * 1000;
	int number_lookups = 2 * 1000 * 1000;

	char keys[number_keys][MAXKEY];

	for (long i = 0; i < number_keys; i++) {
		char key[MAXKEY];
		for (int j = 0; j < MAXKEY-1; j++) {
			key[j]= 'A' + (rand() % 25) ;
		}
		key[MAXKEY-1]='\0';
		//printf("Insert k=%s\n", key);
		tsearch_insert( &tsearch, key, (void*) i);
		strcpy(keys[i],key);
	}

	for (long i = 0; i < number_lookups; i++) {
		long ii = i%number_keys;
		char * key = keys[ii];
		long data = (long) tsearch_find(&tsearch, key);
		//printf("(%s,%ld)=%ld\n", key, ii, data);
		assert(data==ii);
	}
}
