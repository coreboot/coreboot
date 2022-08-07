/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
void *memmove(void *vdest, const void *vsrc, size_t count)
{
	const char *src = vsrc;
	char *dest = vdest;

	if (dest <= src) {
		while (count--)
			*dest++ = *src++;
	} else {
		src  += count - 1;
		dest += count - 1;
		while (count--)
			*dest-- = *src--;
	}
	return vdest;
}
