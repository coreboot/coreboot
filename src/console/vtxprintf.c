/*  vtxprintf.c, from
 *    linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <string.h>
#include <console/console.h>
#include <console/vtxprintf.h>

#ifndef __ROMCC__
#define DATA_ARG , data
#define DATA_ARG_DECL , void *data
#else
#define DATA_ARG
#define DATA_ARG_DECL
#endif

#define call_tx(x) tx_byte(x DATA_ARG)

/* haha, don't need ctype.c */
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define is_digit isdigit
#define isxdigit(c)	(((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

static int number(void (*tx_byte)(unsigned char byte DATA_ARG_DECL),
	unsigned long long num, int base, int size, int precision, int type
	DATA_ARG_DECL)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;
	int count = 0;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if ((signed long long)num < 0) {
			sign = '-';
			num = -num;
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
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0){
		tmp[i++] = digits[num % base];
		num /= base;
	}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			call_tx(' '), count++;
	if (sign)
		call_tx(sign), count++;
	if (type & SPECIAL) {
		if (base==8)
			call_tx('0'), count++;
		else if (base==16) {
			call_tx('0'), count++;
			call_tx(digits[33]), count++;
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			call_tx(c), count++;
	while (i < precision--)
		call_tx('0'), count++;
	while (i-- > 0)
		call_tx(tmp[i]), count++;
	while (size-- > 0)
		call_tx(' '), count++;
	return count;
}


#ifndef __ROMCC__
int vtxdprintf(void (*tx_byte)(unsigned char byte, void *data),
	       const char *fmt, va_list args, void *data)
#else
int vtxprintf(void (*tx_byte)(unsigned char byte), const char *fmt, va_list args);
#endif
{
	int len;
	unsigned long long num;
	int i, base;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	int count;

#if defined(__SMM__) && CONFIG_SMM_TSEG
	/* Fix pointer in TSEG */
	tx_byte = console_tx_byte;
#endif

	for (count=0; *fmt ; ++fmt) {
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
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
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
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'z') {
			qualifier = *fmt;
			++fmt;
			if (*fmt == 'l') {
				qualifier = 'L';
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
			call_tx((unsigned char) va_arg(args, int)), count++;
			while (--field_width > 0)
				call_tx(' '), count++;
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					call_tx(' '), count++;
			for (i = 0; i < len; ++i)
				call_tx(*s++), count++;
			while (len < field_width--)
				call_tx(' '), count++;
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			count += number(tx_byte,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags DATA_ARG);
			continue;

		case 'n':
			if (qualifier == 'L') {
				long long *ip = va_arg(args, long long *);
				*ip = count;
			} else if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = count;
			} else {
				int * ip = va_arg(args, int *);
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
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
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
		} else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		count += number(tx_byte, num, base, field_width, precision, flags DATA_ARG);
	}
	return count;
}


#ifndef __ROMCC__
static void wrap_tx_byte (unsigned char byte, void *data)
{
  void (*tx_byte)(unsigned char byte) = data;
  tx_byte (byte);
}

int vtxprintf(void (*tx_byte)(unsigned char byte), const char *fmt, va_list args)
{
  return vtxdprintf(wrap_tx_byte, fmt, args, tx_byte);
}
#endif
