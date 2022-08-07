/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>

void *memset(void *s, int c, size_t n)
{
	int i;
	char *ss = (char *) s;

	for (i = 0; i < (int)n; i++)
		ss[i] = c;

	return s;
}
