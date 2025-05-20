/*
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

void *default_memset(void *const s, const int c, size_t n)
{
	size_t i;
	u8 *dst = s;
	unsigned long w = c & 0xff;

	const u8 *const aligned_start =
		(const u8 *)ALIGN_UP((uintptr_t)dst, sizeof(unsigned long));
	for (; n > 0 && dst != aligned_start; --n, ++dst)
		*dst = (u8)c;

	for (i = 1; i < sizeof(unsigned long); i <<= 1)
		w = (w << (i * 8)) | w;

	for (i = 0; i < n / sizeof(unsigned long); i++)
		((unsigned long *)dst)[i] = w;

	dst += i * sizeof(unsigned long);

	for (i = 0; i < n % sizeof(unsigned long); i++)
		dst[i] = (u8)c;

	return s;
}

void *default_memcpy(void *dst, const void *src, size_t n)
{
	size_t i;
	void *ret = dst;

	if (IS_ALIGNED((uintptr_t)dst, sizeof(unsigned long)) &&
	    IS_ALIGNED((uintptr_t)src, sizeof(unsigned long))) {
		for (i = 0; i < n / sizeof(unsigned long); i++)
			((unsigned long *)dst)[i] = ((unsigned long *)src)[i];

		src += i * sizeof(unsigned long);
		dst += i * sizeof(unsigned long);
		n -= i * sizeof(unsigned long);
	}

	for (i = 0; i < n; i++)
		((u8 *)dst)[i] = ((u8 *)src)[i];

	return ret;
}

void *default_memmove(void *dst, const void *src, size_t n)
{
	size_t offs;
	ssize_t i;

	if (src > dst)
		return default_memcpy(dst, src, n);

	if (!IS_ALIGNED((uintptr_t)dst, sizeof(unsigned long)) ||
	    !IS_ALIGNED((uintptr_t)src, sizeof(unsigned long))) {
		for (i = n - 1; i >= 0; i--)
			((u8 *)dst)[i] = ((u8 *)src)[i];
		return dst;
	}

	offs = n - (n % sizeof(unsigned long));

	for (i = (n % sizeof(unsigned long)) - 1; i >= 0; i--)
		((u8 *)dst)[i + offs] = ((u8 *)src)[i + offs];

	for (i = n / sizeof(unsigned long) - 1; i >= 0; i--)
		((unsigned long *)dst)[i] = ((unsigned long *)src)[i];

	return dst;
}

#if !CONFIG(LP_ARCH_HAS_MEM_FUNCTIONS)
void *memcpy(void *dst, const void *src, size_t n)
	__attribute__((alias("default_memcpy")));
void *memset(void *s, int c, size_t n)
	__attribute__((alias("default_memset")));
void *memmove(void *dst, const void *src, size_t n)
	__attribute__((alias("default_memmove")));
#endif

/**
 * Compare two memory areas.
 *
 * @param s1 Pointer to the first area to compare.
 * @param s2 Pointer to the second area to compare.
 * @param n Size of the first area in bytes (both must have the same length).
 * @return If n is 0, return zero. Otherwise, return a value less than, equal
 * 	   to, or greater than zero if s1 is found less than, equal to, or
 * 	   greater than s2 respectively.
 */

int memcmp(const void *s1, const void *s2, size_t n)
{
	size_t i = 0;
	const unsigned long *w1 = s1, *w2 = s2;

	if (IS_ALIGNED((uintptr_t)s1, sizeof(unsigned long)) &&
	    IS_ALIGNED((uintptr_t)s2, sizeof(unsigned long)))
		for (; i < n / sizeof(unsigned long); i++)
			if (w1[i] != w2[i])
				break; /* fall through to find differing byte */

	for (i *= sizeof(unsigned long); i < n; i++)
		if (((u8 *)s1)[i] != ((u8 *)s2)[i])
			return ((u8 *)s1)[i] - ((u8 *)s2)[i];

	return 0;
}

void *memchr(const void *s, int c, size_t n)
{
	unsigned char *p = (unsigned char *)s;
	while (n--)
		if (*p != (unsigned char)c)
			p++;
		else
			return p;
	return 0;
}
