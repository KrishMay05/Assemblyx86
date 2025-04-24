// maxarray.c:
#include <stdio.h>

long a[] = {4, 6, 3, 7, 9 };
extern long maxarray(long, long*);

int main()
{
	printf("maxarray(5,a)=%ld\n", maxarray(5,a));
}

