/*
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <lib.h>
#include <string.h>

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
