/* SPDX-License-Identifier: GPL-2.0-only */

#include <ctype.h>
#include <lib.h>

size_t hexstrtobin(const char *str, uint8_t *buf, size_t len)
{
	size_t count, ptr = 0;
	uint8_t byte;

	for (byte = count = 0; str && *str; str++) {
		uint8_t c = *str;

		if (!isxdigit(c))
			continue;
		if (isdigit(c))
			c -= '0';
		else
			c = tolower(c) - 'a' + 10;

		byte <<= 4;
		byte |= c;

		if (++count > 1) {
			if (ptr >= len)
				return ptr;
			buf[ptr++] = byte;
			byte = count = 0;
		}
	}

	return ptr;
}
