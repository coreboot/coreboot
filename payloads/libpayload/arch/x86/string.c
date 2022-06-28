/*
 * Copyright (C) 1991,1992,1993,1997,1998,2003, 2005 Free Software Foundation, Inc.
 * Copyright (c) 2011 The ChromiumOS Authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* From glibc-2.14, sysdeps/i386/memset.c */

#include <stdint.h>

#include "string.h"

typedef uint32_t op_t;

void *memset(void *dstpp, int c, size_t len)
{
	int d0;
	unsigned long int dstp = (unsigned long int) dstpp;

	/* This explicit register allocation improves code very much indeed. */
	register op_t x asm("ax");

	x = (unsigned char) c;

	/* Clear the direction flag, so filling will move forward.  */
	asm volatile("cld");

	/* This threshold value is optimal.  */
	if (len >= 12) {
		/* Fill X with four copies of the char we want to fill with. */
		x |= (x << 8);
		x |= (x << 16);

		/* Adjust LEN for the bytes handled in the first loop.  */
		len -= (-dstp) % sizeof(op_t);

		/*
		 * There are at least some bytes to set. No need to test for
		 * LEN == 0 in this alignment loop.
		 */

		/* Fill bytes until DSTP is aligned on a longword boundary. */
		asm volatile(
			"rep\n"
			"stosb" /* %0, %2, %3 */ :
			"=D" (dstp), "=c" (d0) :
			"0" (dstp), "1" ((-dstp) % sizeof(op_t)), "a" (x) :
			"memory");

		/* Fill longwords.  */
		asm volatile(
			"rep\n"
			"stosl" /* %0, %2, %3 */ :
			"=D" (dstp), "=c" (d0) :
			"0" (dstp), "1" (len / sizeof(op_t)), "a" (x) :
			"memory");
		len %= sizeof(op_t);
	}

	/* Write the last few bytes. */
	asm volatile(
		"rep\n"
		"stosb" /* %0, %2, %3 */ :
		"=D" (dstp), "=c" (d0) :
		"0" (dstp), "1" (len), "a" (x) :
		"memory");

	return dstpp;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned long d0, d1, d2;

	asm volatile(
		"rep ; movsl\n\t"
		"movl %4,%%ecx\n\t"
		"rep ; movsb\n\t"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (n >> 2), "g" (n & 3), "1" (dest), "2" (src)
		: "memory"
	);

	return dest;
}
