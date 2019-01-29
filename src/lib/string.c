#include <string.h>
#include <stddef.h>
#include <stdlib.h>

char *strdup(const char *s)
{
	size_t sz = strlen(s) + 1;
	char *d = malloc(sz);
	memcpy(d, s, sz);
	return d;
}

char *strconcat(const char *s1, const char *s2)
{
	size_t sz_1 = strlen(s1);
	size_t sz_2 = strlen(s2);
	char *d = malloc(sz_1 + sz_2 + 1);
	memcpy(d, s1, sz_1);
	memcpy(d + sz_1, s2, sz_2 + 1);
	return d;
}
