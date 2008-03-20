/*
 * This file is part of the libpayload project.
 *
 * It has originally been taken from the HelenOS project
 * (http://www.helenos.eu), and slightly modified for our purposes.
 *
 * Copyright (c) 2005 Martin Decky
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libpayload.h>

void *memset(void *s, int c, size_t n)
{
	char *os = s;

	while (n--)
		*(os++) = c;

	return s;
}

struct along {
	unsigned long n;
} __attribute__ ((packed));

static void *unaligned_memcpy(void *dst, const void *src, size_t n)
{
	int i, j;
	struct along *adst = dst;
	const struct along *asrc = src;

	for (i = 0; i < n / sizeof(unsigned long); i++)
		adst[i].n = asrc[i].n;

	for (j = 0; j < n % sizeof(unsigned long); j++)
		((unsigned char *)(((unsigned long *)dst) + i))[j] =
		    ((unsigned char *)(((unsigned long *)src) + i))[j];

	return (char *)src;
}

void *memcpy(void *dst, const void *src, size_t n)
{
	int i, j;

	if (((long)dst & (sizeof(long) - 1))
	    || ((long)src & (sizeof(long) - 1)))
		return unaligned_memcpy(dst, src, n);

	for (i = 0; i < n / sizeof(unsigned long); i++)
		((unsigned long *)dst)[i] = ((unsigned long *)src)[i];

	for (j = 0; j < n % sizeof(unsigned long); j++)
		((unsigned char *)(((unsigned long *)dst) + i))[j] =
		    ((unsigned char *)(((unsigned long *)src) + i))[j];

	return (char *)src;
}

void *memmove(void *dst, const void *src, size_t n)
{
	int i, j;

	if (src > dst)
		return memcpy(dst, src, n);

	for (j = (n % sizeof(unsigned long)) - 1; j >= 0; j--)
		((unsigned char *)((unsigned long *)dst))[j] =
		    ((unsigned char *)((unsigned long *)src))[j];

	for (i = n / sizeof(unsigned long) - 1; i >= 0; i--)
		((unsigned long *)dst)[i] = ((unsigned long *)src)[i];

	return (char *)src;
}

/**
 * Compare two memory areas.
 *
 * @param s1	Pointer to the first area to compare.
 * @param s2	Pointer to the second area to compare.
 * @param len	Size of the first area in bytes. Both areas must have the same
 *		length.
 * @return	If len is 0, return zero. If the areas match, return zero.
 *		Otherwise return non-zero.
 */
int memcmp(const char *s1, const char *s2, size_t len)
{
	for (; len && *s1++ == *s2++; len--) ;
	return len;
}
