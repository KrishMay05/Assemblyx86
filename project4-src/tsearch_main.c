
#include <stdio.h>
#include <assert.h>
#include "tsearch.h"

int
main()
{
	TreeSearch  tsearch;

	tsearch_init(&tsearch);
	tsearch_insert( &tsearch, "Peter", (void*) 25);
	tsearch_insert( &tsearch, "Mary", (void*) 29);
	tsearch_insert( &tsearch, "George", (void*) 27);
	tsearch_insert( &tsearch, "Paula", (void*) 26);
	tsearch_insert( &tsearch, "Jennifer", (void*) 21);
	tsearch_insert( &tsearch, "Isabel", (void*) 22);
	tsearch_insert( &tsearch, "Paul", (void*) 19);

	tsearch_print(&tsearch);

	long age;
	age = (long ) tsearch_find(&tsearch, "Paula");
	printf("Paula: %ld\n", age);

	age = (long ) tsearch_find(&tsearch, "Paul");
	printf("Paul: %ld\n", age);

	age = (long ) tsearch_find(&tsearch, "Isabel");
	printf("Isabel: %ld\n", age);

	age = (long ) tsearch_find(&tsearch, "George");
	printf("George: %ld\n", age);

}
