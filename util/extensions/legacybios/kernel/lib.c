/* lib.c
 * tag: simple function library
 *
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#include <stdarg.h>
#include <string.h>
#include "types.h"

int putchar(int c);

/* ****************************************************************** */

/* Convert the integer D to a string and save the string in BUF. If
 * BASE is equal to 'd', interpret that D is decimal, and if BASE is
 * equal to 'x', interpret that D is hexadecimal.  
 */

static void itoa(char *buf, int base, int d)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head.  */
	if (base == 'd' && d < 0) {
		*p++ = '-';
		buf++;
		ud = -d;
	} else if (base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0.  */
	do {
		int remainder = ud % divisor;

		*p++ =
		    (remainder <
		     10) ? remainder + '0' : remainder + 'a' - 10;
	}
	while (ud /= divisor);

	/* Terminate BUF.  */
	*p = 0;

	/* Reverse BUF.  */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

/* Format a string and print it on the screen, just like the libc
 * function printf. 
 */
void printk(const char *format, ...)
{
	va_list ap;
	int c, d;
	char buf[20];

	va_start(ap, format);
	while ((c = *format++) != 0) {
		char *p;
		if (c != '%') {
			putchar(c);
			continue;
		}

		c = *format++;
		switch (c) {
		case 'd':
		case 'u':
		case 'x':
			d = va_arg(ap, int);
			itoa(buf, c, d);
			p = buf;
			goto string;
			break;

		case 's':
			p = va_arg(ap, char *);
			if (!p)
				p = "(null)";

		      string:
			while (*p)
				putchar(*p++);
			break;

		default:
			putchar(va_arg(ap, int));
			break;
		}
	}
	va_end(ap);
}

/* memset might be a macro or alike. Disable it to be sure */
#undef memset

void *memset(void *s, int c, size_t count)
{
	char *xs = (char *) s;
	while (count--)
		*xs++ = c;
	return s;
}

void *memmove(void *dest, const void *src, size_t count)
{
	char *tmp, *s;
	if (dest <= src) {
		tmp = (char *) dest;
		s = (char *) src;
		while (count--)
			*tmp++ = *s++;
	} else {
		tmp = (char *) dest + count;
		s = (char *) src + count;
		while (count--)
			*--tmp = *--s;
	}
	return dest;
}

#ifdef DEBUG_GDB
void *memcpy(void *dest, const void *src, size_t count)
{
	char *tmp = (char *) dest, *s = (char *) src;

	while (count--)
		*tmp++ = *s++;

	return dest;
}

#undef strncmp
int strncmp(const char *cs, const char *ct, size_t count)
{
	register signed char __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}

size_t strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */ ;
	return sc - s;
}
#endif
