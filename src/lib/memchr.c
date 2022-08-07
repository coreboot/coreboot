/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
void *memchr(const void *s, int c, size_t n)
{
	const unsigned char *sc = s;
	while (n--) {
		if (*sc == (unsigned char)c)
			return (void *)sc;
		sc++;
	}
	return NULL;
}
