/*
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 coresystems GmbH
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
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>

/**
 * Compare two strings.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @return Returns a value less than zero, if s1 is shorter than s2. Returns
 * 	   zero, if s1 equals s2. Returns a value greater than zero, if
 * 	   s1 is longer than s2.
 */
int strcasecmp(const char *s1, const char *s2)
{
	int res;

	for (size_t i = 0; 1; i++) {
		res = tolower(s1[i]) - tolower(s2[i]);
		if (res || (s1[i] == '\0'))
			break;
	}

	return res;
}

/**
 * Compare two strings with fixed length.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param maxlen Return at most maxlen characters as length of the string.
 * @return A non-zero value if s1 and s2 differ, or zero if s1 equals s2.
 */
int strncasecmp(const char *s1, const char *s2, size_t maxlen)
{
	int res = 0;

	for (size_t i = 0; i < maxlen; i++) {
		res = tolower(s1[i]) - tolower(s2[i]);
		if (res || (s1[i] == '\0'))
			break;
	}

	return res;
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
	int res;

	for (size_t i = 0; 1; i++) {
		res = s1[i] - s2[i];
		if (res || (s1[i] == '\0'))
			break;
	}

	return res;
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
	int res = 0;

	for (size_t i = 0; i < maxlen; i++) {
		res = s1[i] - s2[i];
		if (res || (s1[i] == '\0'))
			break;
	}

	return res;
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
	size_t max = n > strlen(s) + 1 ? strlen(s) + 1 : n;

	for (size_t i = 0; i < max; i++)
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
 * Concatenates two strings
 *
 * @param d The destination string.
 * @param s The source string.
 * @return A pointer to the destination string.
 */
char *strcat(char *d, const char *s)
{
	char *p = d + strlen(d);
	size_t sl = strlen(s);

	for (size_t i = 0; i < sl; i++)
		p[i] = s[i];

	p[sl] = '\0';
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
char *strncat(char *d, const char *s, size_t n)
{
	char *p = d + strlen(d);
	size_t sl = strlen(s);
	size_t max = n > sl ? sl : n;

	for (size_t i = 0; i < max; i++)
		p[i] = s[i];

	p[max] = '\0';
	return d;
}

/**
 * Concatenates two strings with a maximum length.
 *
 * @param d The destination string.
 * @param s The source string.
 * @param n d will have at most n-1 characters (plus NUL) after invocation.
 * @return The total length of the concatenated string.
 */
size_t strlcat(char *d, const char *s, size_t n)
{
	size_t sl = strlen(s);
	size_t dl = strlen(d);

	if (n <= dl + 1)
		return sl + dl;

	char *p = d + dl;
	size_t max = n > (sl + dl) ? sl : (n - dl - 1);

	for (size_t i = 0; i < max; i++)
		p[i] = s[i];

	p[max] = '\0';
	return sl + dl;
}

/**
 * Find a character in a string.
 *
 * @param s The string.
 * @param c The character.
 * @return A pointer to the first occurrence of the character in the
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
 * @return A pointer to the last occurrence of the character in the
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
	size_t n = strlen(s);
	char *p = malloc(n + 1);

	if (p != NULL) {
		strncpy(p, s, n);
		p[n] = 0;
	}
	return p;
}

/**
 * Duplicate a string with a max length of size
 *
 * @param s The string to duplicate.
 * @param size The max length of the string
 * @return A pointer to the copy of the original string.
 */
char *strndup(const char *s, size_t size)
{
	size_t n = strnlen(s, size);
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
 * @return A pointer to the first occurrence of the substring in
 * the string, or NULL if the substring was not encountered within the string.
 */
char *strstr(const char *h, const char *n)
{
	size_t hn = strlen(h);
	size_t nn = strlen(n);

	if (hn < nn)
		return NULL;

	for (size_t i = 0; i <= hn - nn; i++)
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
	while (*walk && !strchr(delim, *walk))
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
		return 10 + tolower(ch) - 'a';
}

/**
 * Convert the initial portion of a string into a signed int
 * @param ptr A pointer to the string to convert
 * @param endptr A pointer to the unconverted part of the string
 * @param base The base of the number to convert, or 0 for auto
 * @return A signed integer representation of the string
 */

long long int strtoll(const char *orig_ptr, char **endptr, int base)
{
	const char *ptr = orig_ptr;
	int is_negative = 0;

	/* Purge whitespace */

	for ( ; *ptr && isspace(*ptr); ptr++);

	if (ptr[0] == '-') {
		is_negative = 1;
		ptr++;
	}

	unsigned long long uval = strtoull(ptr, endptr, base);

	/* If the whole string is unparseable, endptr should point to start. */
	if (endptr && *endptr == ptr)
		*endptr = (char *)orig_ptr;

	if (uval > (unsigned long long)LLONG_MAX + !!is_negative)
		uval = (unsigned long long)LLONG_MAX + !!is_negative;

	if (is_negative)
		return -uval;
	else
		return uval;
}

long int strtol(const char *ptr, char **endptr, int base)
{
	long long int val = strtoll(ptr, endptr, base);
	if (val > LONG_MAX)
		return LONG_MAX;
	if (val < LONG_MIN)
		return LONG_MIN;
	return val;
}

long atol(const char *nptr)
{
	return strtol(nptr, NULL, 10);
}

/**
 * Convert the initial portion of a string into an unsigned int
 * @param ptr A pointer to the string to convert
 * @param endptr A pointer to the unconverted part of the string
 * @param base The base of the number to convert, or 0 for auto
 * @return An unsigned integer representation of the string
 */

unsigned long long int strtoull(const char *ptr, char **endptr, int base)
{
	unsigned long long int ret = 0;

	if (endptr != NULL)
		*endptr = (char *) ptr;

	/* Purge whitespace */

	for ( ; *ptr && isspace(*ptr); ptr++);

	if (!*ptr)
		return 0;

	/* Determine the base */

	if (base == 0) {
		if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
			base = 16;
		else if (ptr[0] == '0') {
			base = 8;
			ptr++;
		} else {
			base = 10;
		}
	}

	/* Base 16 allows the 0x on front - so skip over it */

	if (base == 16) {
		if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X') &&
		    _valid(ptr[2], base))
			ptr += 2;
	}

	for ( ; *ptr && _valid(*ptr, base); ptr++)
		ret = (ret * base) + _offset(*ptr, base);

	if (endptr != NULL)
		*endptr = (char *) ptr;

	return ret;
}

unsigned long int strtoul(const char *ptr, char **endptr, int base)
{
	unsigned long long val = strtoull(ptr, endptr, base);
	if (val > ULONG_MAX)
		return ULONG_MAX;
	return val;
}

/**
 * Determine the number of leading characters in s that match characters in a
 * @param s A pointer to the string to analyse
 * @param a A pointer to an array of characters that match the prefix
 * @return The number of matching characters
 */
size_t strspn(const char *s, const char *a)
{
	size_t i;
	size_t al = strlen(a);
	for (i = 0; s[i] != 0; i++) {
		int found = 0;
		for (size_t j = 0; j < al; j++) {
			if (s[i] == a[j]) {
				found = 1;
				break;
			}
		}
		if (!found)
			break;
	}
	return i;
}

/**
 * Determine the number of leading characters in s that do not match characters in a
 * @param s A pointer to the string to analyse
 * @param a A pointer to an array of characters that do not match the prefix
 * @return The number of not matching characters
 */
size_t strcspn(const char *s, const char *a)
{
	size_t i;
	size_t al = strlen(a);
	for (i = 0; s[i] != 0; i++) {
		int found = 0;
		for (size_t j = 0; j < al; j++) {
			if (s[i] == a[j]) {
				found = 1;
				break;
			}
		}
		if (found)
			break;
	}
	return i;
}

/**
 * Extract first token in string str that is delimited by a character in tokens.
 * Destroys str and eliminates the token delimiter.
 * @param str A pointer to the string to tokenize.
 * @param delim A pointer to an array of characters that delimit the token
 * @param ptr A pointer to a string pointer to keep state of the tokenizer
 * @return Pointer to token
 */
char *strtok_r(char *str, const char *delim, char **ptr)
{
	/* start new tokenizing job or continue existing one? */
	if (str == NULL)
		str = *ptr;

	/* skip over prefix delimiters */
	char *start = str + strspn(str, delim);

	if (start[0] == '\0')
		return NULL;

	/* find first delimiter character */
	char *end = start + strcspn(start, delim);
	*ptr = end;
	if (end[0] != '\0')
		*(*ptr)++ = '\0';

	return start;
}

/**
 * Extract first token in string str that is delimited by a character in tokens.
 * Destroys str, eliminates the token delimiter and uses global state.
 * @param str A pointer to the string to tokenize.
 * @param delim A pointer to an array of characters that delimit the token
 * @return Pointer to token
 */
char *strtok(char *str, const char *delim)
{
	static char *strtok_ptr;

	return strtok_r(str, delim, &strtok_ptr);
}

/**
 * Print error message and error number
 * @param s Error message to print
 */
void perror(const char *s)
{
	printf("%s: %d\n", s?s:"(none)", errno);
}

/**
 * Get a message string describing the given error number.
 *
 * @param errnum The error number to be interpreted
 * @return A pointer to a string describing the given error number
 */
char *strerror(int errnum)
{
	/* Reserve enough space for the string below + INT64_MIN in decimal + \0 */
	static char errstr[35];
	snprintf(errstr, sizeof(errstr), "Unknown error %d", errnum);
	return errstr;
}

/*
 * Simple routine to convert UTF-16 to ASCII, giving up with ? if too high.
 * A single code point may convert to ?? if not in the BMP.
 * @param utf16_string A string encoded in UTF-16LE
 * @param maxlen Maximum possible length of the string in code points
 * @return Newly allocated ASCII string
 */
char *utf16le_to_ascii(const uint16_t *utf16_string, size_t maxlen)
{
	char *ascii_string = xmalloc(maxlen + 1);  /* +1 for trailing \0 */
	ascii_string[maxlen] = '\0';
	for (size_t i = 0; i < maxlen; i++) {
		uint16_t wchar = utf16_string[i];
		ascii_string[i] = wchar > 0x7f ? '?' : (char)wchar;
	}
	return ascii_string;
}
