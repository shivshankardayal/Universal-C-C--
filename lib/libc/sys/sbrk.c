#include <stddef.h>
#include <stdlib.h>
#include <errno.h>


void *sbrk(ptrdiff_t sz){
	errno=ENOMEM;
	return (void *)-1;
};

int brk(ptrdiff_t sz){
	errno=ENOMEM;
	return 0;
};

