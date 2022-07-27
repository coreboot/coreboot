/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CTYPE_H
#define CTYPE_H

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

static inline int isprint(int c)
{
	return c >= ' ' && c <= '~';
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

#endif /* CTYPE_H */
