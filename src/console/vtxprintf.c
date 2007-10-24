/*  vtxprintf.c, from
 *    linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <stdarg.h>
#include <string.h>
#include <div64.h>

/* haha, don't need ctype.c */
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define is_digit isdigit
#define isxdigit(c)	(((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

#if 0
/* We are using number() instead. So this code is obsoleted and should
 * probably go away.
 */

static unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (!base) {
		base = 10;
		if (*cp == '0') {
			base = 8;
			cp++;
			if ((*cp == 'x') && isxdigit(cp[1])) {
				cp++;
				base = 16;
			}
		}
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

static long simple_strtol(const char *cp,char **endp,unsigned int base)
{
	if(*cp=='-')
		return -simple_strtoul(cp+1,endp,base);
	return simple_strtoul(cp,endp,base);
}
#endif

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

static int number(void (*tx_byte)(unsigned char byte), 
	unsigned long long num, int base, int size, int precision, int type)
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
	else while (num != 0)
		tmp[i++] = digits[do_div(num,base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			tx_byte(' '), count++;
	if (sign)
		tx_byte(sign), count++;
	if (type & SPECIAL) {
		if (base==8)
			tx_byte('0'), count++;
		else if (base==16) {
			tx_byte('0'), count++;
			tx_byte(digits[33]), count++;
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			tx_byte(c), count++;
	while (i < precision--)
		tx_byte('0'), count++;
	while (i-- > 0)
		tx_byte(tmp[i]), count++;
	while (size-- > 0)
		tx_byte(' '), count++;
	return count;
}


int vtxprintf(void (*tx_byte)(unsigned char byte), const char *fmt, va_list args)
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

	for (count=0; *fmt ; ++fmt) {
		if (*fmt != '%') {
			tx_byte(*fmt), count++;
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
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
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
					tx_byte(' '), count++;
			tx_byte((unsigned char) va_arg(args, int)), count++;
			while (--field_width > 0)
				tx_byte(' '), count++;
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					tx_byte(' '), count++;
			for (i = 0; i < len; ++i)
				tx_byte(*s++), count++;
			while (len < field_width--)
				tx_byte(' '), count++;
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			count += number(tx_byte,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
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
			tx_byte('%'), count++;
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
			tx_byte('%'), count++;
			if (*fmt)
				tx_byte(*fmt), count++;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'L') {
			num = va_arg(args, unsigned long long);
		} else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
		} else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		count += number(tx_byte, num, base, field_width, precision, flags);
	}
	return count;
}

