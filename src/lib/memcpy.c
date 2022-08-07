/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
void *memcpy(void *vdest, const void *vsrc, size_t bytes)
{
	const char *src = vsrc;
	char *dest = vdest;
	int i;

	for (i = 0; i < (int)bytes; i++)
		dest[i] = src[i];

	return vdest;
}
