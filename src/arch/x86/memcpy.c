/*
 * This file is part of the coreboot project.
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

#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned long d0, d1, d2;

	asm volatile(
#ifdef __x86_64__
		"rep ; movsd\n\t"
		"mov %4,%%rcx\n\t"
#else
		"rep ; movsl\n\t"
		"movl %4,%%ecx\n\t"
#endif
		"rep ; movsb\n\t"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (n >> 2), "g" (n & 3), "1" (dest), "2" (src)
		: "memory"
	);

	return dest;
}
