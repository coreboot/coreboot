#include <string.h>
void *memcpy(void *__dest, __const void *__src, size_t __n)
{
	int i;
	char *d = (char *) __dest, *s = (char *) __src;

	for (i = 0; i < __n; i++)
		d[i] = s[i];

	return __dest;
}
