#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdlib.h>

/* Stringify a token */
#ifndef STRINGIFY
#define _STRINGIFY(x)	#x
#define STRINGIFY(x)	_STRINGIFY(x)
#endif

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);
#if !defined(__PRE_RAM__)
int sprintf(char * buf, const char *fmt, ...);
int snprintf(char * buf, size_t size, const char *fmt, ...);
#endif

// simple string functions

static inline size_t strnlen(const char *src, size_t max)
{
	size_t i = 0;
	while((*src++) && (i < max)) {
		i++;
	}
	return i;
}

static inline size_t strlen(const char *src)
{
	size_t i = 0;
	while(*src++) {
		i++;
	}
	return i;
}

static inline char *strchr(const char *s, int c)
{
	for (; *s; s++) {
		if (*s == c)
			return (char *) s;
	}
	return 0;
}

#if !defined(__PRE_RAM__)
static inline char *strdup(const char *s)
{
	size_t sz = strlen(s) + 1;
	char *d = malloc(sz);
	memcpy(d, s, sz);
	return d;
}
#endif

static inline char *strncpy(char *to, const char *from, int count)
{
	register char *ret = to;

	while (count > 0) {
		count--;
		if ((*to++ = *from++) == '\0')
			break;
	}

	while (count > 0) {
		count--;
		*to++ = '\0';
	}
	return ret;
}

static inline char *strcpy(char *dst, const char *src)
{
	char *ptr = dst;

	while (*src)
		*dst++ = *src++;
	*dst = '\0';

	return ptr;
}

static inline int strcmp(const char *s1, const char *s2)
{
	int r;

	while ((r = (*s1 - *s2)) == 0 && *s1) {
		s1++;
		s2++;
	}
	return r;
}

static inline int strncmp(const char *s1, const char *s2, int maxlen)
{
	int i;

	for (i = 0; i < maxlen; i++) {
		if (s1[i] != s2[i])
			return s1[i] - s2[i];
	}

	return 0;
}

static inline int isspace(int c)
{
	switch (c) {
	case ' ': case '\f': case '\n':
	case '\r': case '\t': case '\v':
		return 1;
	default:
		return 0;
	}
}

static inline int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

static inline int isxdigit(int c)
{
	return ((c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F'));
}

static inline int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

static inline int islower(int c)
{
	return (c >= 'a' && c <= 'z');
}

static inline int toupper(int c)
{
        if (islower(c))
                c -= 'a'-'A';
        return c;
}

static inline int tolower(int c)
{
        if (isupper(c))
                c -= 'A'-'a';
        return c;
}
#endif /* STRING_H */
