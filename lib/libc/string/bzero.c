#include <string.h>

void bzero(b, length)
	void *b;
	register size_t length;
{
	memset(b,0,length);
}
