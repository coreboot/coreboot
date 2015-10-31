 /*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include "string.h"

/*
 * Alternative string functions to the default ones are added
 * because there is no guarantee that the provided source and
 * destination addresses are properly aligned;
 * The default string functions work with multiple of 4 bytes
 * (sizeof(unsinged long)); MIPS will use LW/SW instructions
 * for these operations and if the source and destination
 * addresses are not aligned it will trigger an exception.
 */

void *memcpy(void *dest, const void *src, size_t n)
{
	u8 *ptr_d = dest;
	const u8 *ptr_s = src;
	size_t i;

	for (i = 0; i < n; i++)
		*ptr_d++ = *ptr_s++;

	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	if ((src < dest) && (dest - src < n)) {
		u8 *ptr_d = dest;
		const u8 *ptr_s = src;

		/* copy backwards */
		while (n--)
			ptr_d[n] = ptr_s[n];

		return dest;
	}

	/* copy forwards */
	return memcpy(dest, src, n);
}

void *memset(void *s, int c, size_t n)
{
	u8 *ptr = s;
	size_t i;

	for (i = 0; i < n; i++)
		*ptr++ = c;

	return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		if (((u8 *)s1)[i] != ((u8 *)s2)[i])
			return ((u8 *)s1)[i] - ((u8 *)s2)[i];
	return 0;
}
