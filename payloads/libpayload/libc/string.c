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

	/* Return something positive (if s1 is longer than s2), or zero (if s1
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
int strncmp(const char *s1, const char *s2, size_t maxlen)
{
	int i;

	for (i = 0; i < maxlen; i++) {
		if (s1[i] != s2[i])
			return s1[i] - s2[i];
	}

	return 0;
}

/**
 * Copy a string with a maximum length.
 *
 * @param d The destination memory.
 * @param s The source string.
 * @param n Copy at most n characters as length of the string.
 * @return A pointer to the destination memory.
 */
char *strncpy(char *d, const char *s, size_t n)
{
	/* Use +1 to get the NUL terminator. */
	int max = n > strlen(s) + 1 ? strlen(s) + 1 : n;
	int i;

	for (i = 0; i < max; i++)
		d[i] = (char)s[i];

	return d;
}

/**
 * Copy a string.
 *
 * @param d The destination memory.
 * @param s The source string.
 * @return A pointer to the destination memory.
 */
char *strcpy(char *d, const char *s)
{
	return strncpy(d, s, strlen(s) + 1);
}

/**
 * Concatenates two strings with a maximum length.
 *
 * @param d The destination string.
 * @param s The source string.
 * @param n Not more than n characters from s will be appended to d.
 * @return A pointer to the destination string.
 */
char *strncat(char *d, const char *s, size_t n)
{
	char *p = d + strlen(d);
	int sl = strlen(s);
	int max = n > sl ? sl : n;
	// int max = n > strlen(s) ? strlen(s) : n;
	int i;

	for (i = 0; i < max; i++)
		p[i] = s[i];

	p[i] = '\0';
	return d;
}

/**
 * Concatenates two strings with a maximum length.
 *
 * @param d The destination string.
 * @param s The source string.
 * @param n Not more than n characters from s will be appended to d.
 * @return A pointer to the destination string.
 */
size_t strlcat(char *d, const char *s, size_t n)
{
	int sl = strlen(s);
	int dl = strlen(d);

	char *p = d + dl;
	int max = n > (sl + dl) ? sl : (n - dl - 1);
	int i;

	for (i = 0; i < max; i++)
		p[i] = s[i];

	p[i] = '\0';
	return max;
}

/**
 * Find a character in a string.
 *
 * @param s The string.
 * @param c The character.
 * @return A pointer to the first occurence of the character in the
 * string, or NULL if the character was not encountered within the string.
 */
char *strchr(const char *s, int c)
{
	char *p = (char *)s;

	for (; *p != 0; p++) {
		if (*p == c)
			return p;
	}

	return NULL;
}

/**
 * Find a character in a string.
 *
 * @param s The string.
 * @param c The character.
 * @return A pointer to the last occurence of the character in the
 * string, or NULL if the character was not encountered within the string.
 */

char *strrchr(const char *s, int c)
{
	char *p = (char *)s + strlen(s);

	for (; p >= s; p--) {
		if (*p == c)
			return p;
	}

	return NULL;
}

/**
 * Duplicate a string.
 *
 * @param s The string to duplicate.
 * @return A pointer to the copy of the original string.
 */
char *strdup(const char *s)
{
	int n = strlen(s);
	char *p = malloc(n + 1);

	if (p != NULL) {
		strncpy(p, s, n);
		p[n] = 0;
	}
	return p;
}

/**
 * Find a substring within a string.
 *
 * @param h The haystack string.
 * @param n The needle string (substring).
 * @return A pointer to the first occurence of the substring in
 * the string, or NULL if the substring was not encountered within the string.
 */
char *strstr(const char *h, const char *n)
{
	int hn = strlen(h);
	int nn = strlen(n);
	int i;

	for (i = 0; i <= hn - nn; i++)
		if (!memcmp(&h[i], n, nn))
			return (char *)&h[i];

	return NULL;
}

/**
 * Separate strings.
 *
 * @param stringp reference of the string to separate.
 * @param delim string containing all delimiters.
 * @return Token string.
 */
char *strsep(char **stringp, const char *delim)
{
	char *walk, *token;

	if (!stringp || !*stringp || !**stringp)
		return NULL;

	token = walk = *stringp;

	/* Walk, search for delimiters */
	while(*walk && !strchr(delim, *walk))
		walk++;

	if (*walk) {
		/* NUL terminate */
		*walk = '\0';
		walk++;
	}

	*stringp = walk;

	return token;
}

/* Check that a character is in the valid range for the
   given base
*/

static int _valid(char ch, int base)
{
        char end = (base > 9) ? '9' : '0' + (base - 1);

        /* all bases will be some subset of the 0-9 range */

        if (ch >= '0' && ch <= end)
                return 1;

        /* Bases > 11 will also have to match in the a-z range */

        if (base > 11) {
                if (tolower(ch) >= 'a' &&
                    tolower(ch) <= 'a' + (base - 11))
                        return 1;
        }

        return 0;
}

/* Return the "value" of the character in the given base */

static int _offset(char ch, int base)
{
        if (ch >= '0' && ch <= '9')
                return ch - '0';
        else
                return tolower(ch) - 'a';
}

/**
 * Convert the initial portion of a string into an unsigned int
 * @param ptr A pointer to the string to convert
 * @param endptr A pointer to the unconverted part of the string
 * @param base The base of the number to convert, or 0 for auto
 * @return An unsigned integer representation of the string
 */

unsigned int strtoul(const char *ptr, char **endptr, int base)
{
        int ret = 0;

	if (endptr != NULL)
		*endptr = (char *) ptr;

        /* Purge whitespace */

        for( ; *ptr && isspace(*ptr); ptr++);

        if (!*ptr)
                return 0;

        /* Determine the base */

        if (base == 0) {
		if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
			base = 16;
		else if (ptr[0] == '0') {
			base = 8;
			ptr++;
		}
		else
			base = 10;
        }

	/* Base 16 allows the 0x on front - so skip over it */

	if (base == 16) {
		if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
			ptr += 2;
	}

	/* If the first character isn't valid, then don't
         * bother */

        if (!*ptr || !_valid(*ptr, base))
                return 0;

        for( ; *ptr && _valid(*ptr, base); ptr++)
                ret = (ret * base) + _offset(*ptr, base);

	if (endptr != NULL)
		*endptr = (char *) ptr;

        return ret;
}


