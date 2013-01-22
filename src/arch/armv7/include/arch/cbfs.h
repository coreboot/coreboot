/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __INCLUDE_ARCH_CBFS__
#define __INCLUDE_ARCH_CBFS__

#include <cbfs_core.h>
#include <arch/byteorder.h>

// TODO FIXME This file is only for providing CBFS function in bootblock.
// Should be removed once bootblock can link lib/* files.
#include "lib/cbfs.c"

// mem* and ulzma are now workarounds for bootblock compilation.
void *memcpy(void *dest, const void *src, size_t n) {
	char *d = (char *)dest;
	const char *s = (const char*)src;
	while (n-- > 0)
		*d++ = *s++;
	return dest;
}

void *memset(void *dest, int c, size_t n) {
	char *d = (char*)dest;
	while (n-- > 0)
		*d++ = c;
	return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t n) {
	const char *s1 = (const char*)ptr1, *s2 = (const char*)ptr2;
	int c;
	while (n-- > 0)
		if ((c = *s1++ - *s2++))
			return c;
	return 0;
}

unsigned long ulzma(unsigned char *src, unsigned char *dest) {
	// TODO remove this.
	return -1;
}

#endif // __INCLUDE_ARCH_CBFS__
