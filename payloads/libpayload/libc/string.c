/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload.h>
#include <arch/types.h>

/**
 * Calculate the length of a fixed-size string.
 *
 * @param str The input string.
 * @param maxlen Return at most maxlen characters as length of the string.
 * @return The length of the string, not including the final NUL character.
 * 	   The maximum length returned is maxlen.
 */
size_t strnlen(const char *str, size_t maxlen)
{
	size_t len = 0;

	/* NULL and empty strings have length 0. */
	if (!str)
		return 0;

	/* Loop until we find a NUL character, or maxlen is reached. */
	while ((*str++ != '\0') && (len < maxlen))
		len++;

	return len;
}

/**
 * Calculate the length of a string.
 *
 * @param str The input string.
 * @return The length of the string, not including the final NUL character.
 */
size_t strlen(const char *str)
{
	size_t len = 0;

	/* NULL and empty strings have length 0. */
	if (!str)
		return 0;

	/* Loop until we find a NUL character. */
	while (*str++ != '\0')
		len++;

	return len;
}

/**
 * Compare two strings.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @return Returns a value less than zero, if s1 is shorter than s2. Returns
 * 	   zero, if s1 equals s2. Returns a value greater than zero, if
 * 	   s1 is longer than s2.
 */
int strcmp(const char *s1, const char *s2)
{
	char c1, c2;

	/* Set c1 == c2, so that we can enter the while loop. */
	c1 = 0;
	c2 = 0;

	/* Compare characters until they differ, or one of the strings ends. */
	while (c1 == c2) {
		/* Read the next character from each string. */
		c1 = *s1++;
		c2 = *s2++;

		/* Return something negative (if s1 is shorter than s2), or
		   zero (if s1 equals s2). */
		if (c1 == '\0')
			return c1 - c2;
	}

	/* Return someting positive (if s1 is longer than s2), or zero (if s1
	   and s2 are equal). */
	return c1 - c2;
}

/**
 * Compare two strings with fixed length.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param maxlen Return at most maxlen characters as length of the string.
 * @return A non-zero value if s1 and s2 differ, or zero if s1 equals s2.
 */
int strncmp(const char *s1, const char *s2, int maxlen)
{
	int i;

	for (i = 0; i < maxlen; i++) {
		if (s1[i] != s2[i])
			return s1[i] - s2[i];
	}

	return 0;
}

char *strncpy(char *d, const char *s, int n)
{
	/* use +1 to get the null terminator */

	int max = n > strlen(s) + 1 ? strlen(s) + 1 : n;
	int i;

	for(i = 0; i < max; i++)
		d[i] = (char) s[i];

	return d;
}

char *strcpy(char *d, const char *s)
{
	return strncpy(d, s, strlen(s));
}

char *strncat(char *d, const char *s, int n)
{
	char *p = d + strlen(d);
	int max = n > strlen(s) ? strlen(s) : n;
	int i;

	for(i = 0; i < max; i++)
		p[i] = s[i];

	p[i] = '\0';
	return d;
}

char * strchr(const char *s, int c)
{
	char *p = (char *) s;

	for( ; *p != 0; p++) {
		if (*p == c)
			return p;
	}

	return NULL;
}


char *strdup(const char *s)
{
	int n = strlen(s);
	char *p = malloc(n);

	if (p != NULL)
		strncpy(p, s, n);

	return p;
}

char *strstr(const char *h, const char *n)
{
	int hn = strlen(h);
	int nn = strlen(n);
	int i;

	for(i = 0; i <= hn - nn; i++)
		if (!strcmp(&h[i], n))
			return (char *) &h[i];

	return NULL;
}

	
