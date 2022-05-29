/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * vtxprintf.c, originally from linux/lib/vsprintf.c
 */

#include <console/vtxprintf.h>
#include <ctype.h>
#include <string.h>
#include <types.h>

#define call_tx(x) tx_byte(x, data)

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

static int number(void (*tx_byte)(unsigned char byte, void *data), unsigned long long inum,
		  int base, int size, int precision, int type, void *data)
{
	char c, sign, tmp[66];
	const char *digits = "0123456789abcdef";
	int i;
	int count = 0;
	unsigned long long num = inum;
	long long snum = num;

	if (type & LARGE)
		digits = "0123456789ABCDEF";
	if (type & LEFT)
		type &= ~ZEROPAD;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (snum < 0) {
			sign = '-';
			num = -snum;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0) {
		tmp[i++] = '0';
	} else {
		while (num != 0) {
			tmp[i++] = digits[num % base];
			num /= base;
		}
	}
	if (i > precision) {
		precision = i;
	}
	size -= precision;
	if (!(type & (ZEROPAD + LEFT))) {
		while (size-- > 0)
			call_tx(' '), count++;
	}
	if (sign) {
		call_tx(sign), count++;
	}
	if (type & SPECIAL) {
		if (base == 8)
			call_tx('0'), count++;
		else if (base == 16) {
			call_tx('0'), count++;
			if (type & LARGE)
				call_tx('X'), count++;
			else
				call_tx('x'), count++;
		}
	}
	if (!(type & LEFT)) {
		while (size-- > 0)
			call_tx(c), count++;
	}
	while (i < precision--)
		call_tx('0'), count++;
	while (i-- > 0)
		call_tx(tmp[i]), count++;
	while (size-- > 0)
		call_tx(' '), count++;
	return count;
}

int vtxprintf(void (*tx_byte)(unsigned char byte, void *data), const char *fmt, va_list args,
	      void *data)
{
	int len;
	unsigned long long num;
	int i, base;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'H', 'l', 'L', 'z', or 'j' for integer fields */

	int count;

	for (count = 0; *fmt; ++fmt) {
		if (*fmt != '%') {
			call_tx(*fmt), count++;
			continue;
		}

		/* process flags */
		flags = 0;
repeat:
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
		case '-': flags |= LEFT; goto repeat;
		case '+': flags |= PLUS; goto repeat;
		case ' ': flags |= SPACE; goto repeat;
		case '#': flags |= SPECIAL; goto repeat;
		case '0': flags |= ZEROPAD; goto repeat;
		}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt)) {
			field_width = skip_atoi((char **)&fmt);
		} else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt)) {
				precision = skip_atoi((char **)&fmt);
			} else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0) {
				precision = 0;
			}
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'z' || *fmt == 'j') {
			qualifier = *fmt;
			++fmt;
			if (*fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
			if (*fmt == 'h') {
				qualifier = 'H';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					call_tx(' '), count++;
			call_tx((unsigned char)va_arg(args, int)), count++;
			while (--field_width > 0)
				call_tx(' '), count++;
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, (size_t)precision);

			if (!(flags & LEFT)) {
				while (len < field_width--)
					call_tx(' '), count++;
			}
			for (i = 0; i < len; ++i)
				call_tx(*s++), count++;
			while (len < field_width--)
				call_tx(' '), count++;
			continue;

		case 'p':
			/* even on 64-bit systems, coreboot only resides in the
			   low 4GB so pad pointers to 32-bit for readability. */
			if (field_width == -1 && precision == -1)
				precision = 2 * sizeof(uint32_t);
			flags |= SPECIAL;
			count += number(tx_byte, (unsigned long)va_arg(args, void *), 16,
					field_width, precision, flags, data);
			continue;

		case 'n':
			if (qualifier == 'L') {
				long long *ip = va_arg(args, long long *);
				*ip = count;
			} else if (qualifier == 'l') {
				long *ip = va_arg(args, long *);
				*ip = count;
			} else {
				int *ip = va_arg(args, int *);
				*ip = count;
			}
			continue;

		case '%':
			call_tx('%'), count++;
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
			__fallthrough;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
			__fallthrough;
		case 'u':
			break;

		default:
			call_tx('%'), count++;
			if (*fmt)
				call_tx(*fmt), count++;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'L') {
			num = va_arg(args, unsigned long long);
		} else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
		} else if (qualifier == 'z') {
			num = va_arg(args, size_t);
		} else if (qualifier == 'j') {
			num = va_arg(args, uintmax_t);
		} else if (qualifier == 'h') {
			num = (unsigned short)va_arg(args, int);
			if (flags & SIGN)
				num = (short)num;
		} else if (qualifier == 'H') {
			num = (unsigned char)va_arg(args, int);
			if (flags & SIGN)
				num = (signed char)num;
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		count += number(tx_byte, num, base, field_width, precision, flags, data);
	}
	return count;
}
