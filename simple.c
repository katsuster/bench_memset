#include <string.h>

void *memset_simple(void *dest, int c, size_t n)
{
	unsigned char *s = dest;

	for (; n; n--, s++) *s = c;

	return dest;
}
