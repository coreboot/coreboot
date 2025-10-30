/*
 *
 * It has originally been taken from the HelenOS project
 * (http://www.helenos.eu), and slightly modified for our purposes.
 *
 * Copyright (C) 2001-2004 Jakub Jermar
 * Copyright (C) 2006 Josef Cejka
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <ctype.h>

static struct _FILE {
} _stdout, _stdin, _stderr;

FILE *stdout = &_stdout;
FILE *stdin = &_stdin;
FILE *stderr = &_stderr;

/** Structure for specifying output methods for different printf clones. */
struct printf_spec {
	/* Output function, returns count of printed characters or EOF. */
	int (*write) (const char *, size_t, void *);
	/* Support data - output stream specification, its state, locks, ... */
	void *data;
};

/** Show prefixes 0x or 0. */
#define __PRINTF_FLAG_PREFIX		0x00000001
/** Signed / unsigned number. */
#define __PRINTF_FLAG_SIGNED		0x00000002
/** Print leading zeroes. */
#define __PRINTF_FLAG_ZEROPADDED	0x00000004
/** Align to left. */
#define __PRINTF_FLAG_LEFTALIGNED	0x00000010
/** Always show + sign. */
#define __PRINTF_FLAG_SHOWPLUS		0x00000020
/** Print space instead of plus. */
#define __PRINTF_FLAG_SPACESIGN		0x00000040
/** Show big characters. */
#define __PRINTF_FLAG_BIGCHARS		0x00000080
/** Number has - sign. */
#define __PRINTF_FLAG_NEGATIVE		0x00000100

/**
 * Buffer big enough for 64-bit number printed in base 2, sign, and prefix.
 * Add some more to support sane amounts of zero-padding.
 */
#define PRINT_BUFFER_SIZE		(64 + 1 + 2 + 13)

/** Enumeration of possible arguments types. */
typedef enum {
	PrintfQualifierByte = 0,
	PrintfQualifierShort,
	PrintfQualifierInt,
	PrintfQualifierLong,
	PrintfQualifierLongLong,
	PrintfQualifierPointer,
} qualifier_t;

static const char digits_small[] = "0123456789abcdef";
static const char digits_big[] = "0123456789ABCDEF";

/**
 * Print one or more characters without adding newline.
 *
 * @param buf	Buffer of >= count bytesi size. NULL pointer is not allowed!
 * @param count	Number of characters to print.
 * @param ps	Output method and its data.
 * @return	Number of characters printed.
 */
static int printf_putnchars(const char *buf, size_t count,
			    struct printf_spec *ps)
{
	return ps->write(buf, count, ps->data);
}

/**
 * Print one character.
 *
 * @param c	Character to be printed.
 * @param ps	Output method.
 * @return	Number of characters printed.
 */
static int printf_putchar(int c, struct printf_spec *ps)
{
	char ch = c;

	return ps->write(&ch, 1, ps->data);
}

/* Print spaces for padding. Ignores negative counts. */
static int print_spaces(int count, struct printf_spec *ps)
{
	int tmp, ret;
	char buffer[PRINT_BUFFER_SIZE];

	if (count <= 0)
		return 0;

	memset(buffer, ' ', MIN(PRINT_BUFFER_SIZE, count));
	for (tmp = count; tmp > PRINT_BUFFER_SIZE; tmp -= PRINT_BUFFER_SIZE)
		if ((ret = printf_putnchars(buffer, PRINT_BUFFER_SIZE, ps)) < 0)
			return ret;

	if ((ret = printf_putnchars(buffer, tmp, ps)) < 0)
		return ret;

	return count;
}

/**
 * Print one formatted character.
 *
 * @param c	Character to print.
 * @param width	Width modifier.
 * @param flags	Flags that change the way the character is printed.
 * @param ps	Output methods spec for different printf clones.
 * @return	Number of characters printed, negative value on failure.
 */
static int print_char(char c, int width, uint64_t flags, struct printf_spec *ps)
{
	int retval;
	int counter = 1;

	if (!(flags & __PRINTF_FLAG_LEFTALIGNED)) {
		if ((retval = print_spaces(width - 1, ps)) < 0)
			return retval;
		else
			counter += retval;
	}

	if ((retval = printf_putchar(c, ps)) < 0)
		return retval;

	if (flags & __PRINTF_FLAG_LEFTALIGNED) {
		if ((retval = print_spaces(width - 1, ps)) < 0)
			return retval;
		else
			counter += retval;
	}

	return counter;
}

/**
 * Print string.
 *
 * @param s		String to be printed.
 * @param width		Width modifier.
 * @param precision	Precision modifier.
 * @param flags		Flags that modify the way the string is printed.
 * @param ps		Output methods spec for different printf clones.
 * @return		Number of characters printed, negative value on	failure.
 */
/** Structure for specifying output methods for different printf clones. */
static int print_string(const char *s, int width, int precision,
			uint64_t flags, struct printf_spec *ps)
{
	int counter = 0, retval;
	size_t size;

	if (s == NULL)
		s = "(null)";
	if (precision >= 0)
		size = strnlen(s, precision);
	else
		size = strlen(s);
	width -= size;

	if (!(flags & __PRINTF_FLAG_LEFTALIGNED)) {
		if ((retval = print_spaces(width, ps)) < 0)
			return retval;
		else
			counter += retval;
	}

	if ((retval = printf_putnchars(s, size, ps)) < 0)
		return retval;
	counter += retval;

	if (flags & __PRINTF_FLAG_LEFTALIGNED) {
		if ((retval = print_spaces(width, ps)) < 0)
			return retval;
		else
			counter += retval;
	}

	return counter;
}

/**
 * Print a number in a given base.
 *
 * Print significant digits of a number in given base.
 *
 * @param num		Number to print.
 * @param width		Width modifier.
 * @param precision	Precision modifier.
 * @param base		Base to print the number in (must be between 2 and 16).
 * @param flags		Flags that modify the way the number is printed.
 * @param ps		Output methods spec for different printf clones.
 * @return		Number of characters printed.
 */
static int print_number(uint64_t num, int width, int precision, int base,
			uint64_t flags, struct printf_spec *ps)
{
	const char *digits = digits_small;
	char d[PRINT_BUFFER_SIZE];
	char *ptr = &d[PRINT_BUFFER_SIZE];
	int size = 0;			/* Size of the string in ptr */
	int counter = 0;		/* Amount of actually printed bytes. */
	char sgn;
	int retval;

	if (flags & __PRINTF_FLAG_BIGCHARS)
		digits = digits_big;

	if (num == 0) {
		*--ptr = '0';
		size++;
	} else {
		do {
			*--ptr = digits[num % base];
			size++;
		} while (num /= base);
	}

	/* Both precision and LEFTALIGNED overrule ZEROPADDED. */
	if ((flags & __PRINTF_FLAG_LEFTALIGNED) || precision >= 0)
		flags &= ~__PRINTF_FLAG_ZEROPADDED;

	/* Fix precision now since it doesn't count prefixes/signs. */
	precision -= size;

	/* Reserve size for prefixes/signs before filling up padding. */
	sgn = 0;
	if (flags & __PRINTF_FLAG_SIGNED) {
		if (flags & __PRINTF_FLAG_NEGATIVE) {
			sgn = '-';
			size++;
		} else if (flags & __PRINTF_FLAG_SHOWPLUS) {
			sgn = '+';
			size++;
		} else if (flags & __PRINTF_FLAG_SPACESIGN) {
			sgn = ' ';
			size++;
		}
	}
	if (flags & __PRINTF_FLAG_PREFIX) {
		switch (base) {
		case 2:	/* Binary formating is not standard, but useful. */
			size += 2;
			break;
		case 8:
			size++;
			break;
		case 16:
			size += 2;
			break;
		}
	}

	/* If this is still set we didn't have a precision, so repurpose it */
	if (flags & __PRINTF_FLAG_ZEROPADDED)
		precision = width - size;

	/* Pad smaller numbers with 0 (larger numbers lead to precision < 0). */
	if (precision > 0) {
		precision = MIN(precision, PRINT_BUFFER_SIZE - size);
		ptr -= precision;
		size += precision;
		memset(ptr, '0', precision);
	}

	/* Add sign and prefix (we adjusted size for this beforehand). */
	if (flags & __PRINTF_FLAG_PREFIX) {
		switch (base) {
		case 2:	/* Binary formating is not standard, but useful. */
			*--ptr = (flags & __PRINTF_FLAG_BIGCHARS) ? 'B' : 'b';
			*--ptr = '0';
			break;
		case 8:
			*--ptr = '0';
			break;
		case 16:
			*--ptr = (flags & __PRINTF_FLAG_BIGCHARS) ? 'X' : 'x';
			*--ptr = '0';
			break;
		}
	}
	if (sgn)
		*--ptr = sgn;

	/* Pad with spaces up to width, try to avoid extra putnchar if we can */
	width -= size;
	if (width > 0 && !(flags & __PRINTF_FLAG_LEFTALIGNED)) {
		int tmp = MIN(width, PRINT_BUFFER_SIZE - size);
		ptr -= tmp;
		size += tmp;
		memset(ptr, ' ', tmp);
		if ((retval = print_spaces(width - tmp, ps)) < 0)
			return retval;
		else
			counter += retval;
	}

	/* Now print the whole thing at once. */
	if ((retval = printf_putnchars(ptr, size, ps)) < 0)
		return retval;
	counter += retval;

	/* Edge case: left-aligned with width (should be rare). */
	if (flags & __PRINTF_FLAG_LEFTALIGNED) {
		if ((retval = print_spaces(width, ps)) < 0)
			return retval;
		else
			counter += retval;
	}

	return counter;
}

/**
 * Print formatted string.
 *
 * Print string formatted according to the fmt parameter and variadic arguments.
 * Each formatting directive must have the following form:
 *
 * 	\% [ FLAGS ] [ WIDTH ] [ .PRECISION ] [ TYPE ] CONVERSION
 *
 * FLAGS:@n
 * 	- "#"	Force to print prefix.For \%o conversion, the prefix is 0, for
 *		\%x and \%X prefixes are 0x and	0X and for conversion \%b the
 *		prefix is 0b.
 *
 * 	- "-"	Align to left.
 *
 * 	- "+"	Print positive sign just as negative.
 *
 * 	- " "	If the printed number is positive and "+" flag is not set,
 *		print space in place of sign.
 *
 * 	- "0"	Print 0 as padding instead of spaces. Zeroes are placed between
 *		sign and the rest of the number. This flag is ignored if "-"
 *		flag is specified.
 *
 * WIDTH:@n
 * 	- Specify the minimal width of a printed argument. If it is bigger,
 *	width is ignored. If width is specified with a "*" character instead of
 *	number, width is taken from parameter list. And integer parameter is
 *	expected before parameter for processed conversion specification. If
 *	this value is negative its absolute value is taken and the "-" flag is
 *	set.
 *
 * PRECISION:@n
 * 	- Value precision. For numbers it specifies minimum valid numbers.
 *	Smaller numbers are printed with leading zeroes. Bigger numbers are not
 *	affected. Strings with more than precision characters are cut off. Just
 *	as with width, an "*" can be used used instead of a number. An integer
 *	value is then expected in parameters. When both width and precision are
 *	specified using "*", the first parameter is used for width and the
 *	second one for precision.
 *
 * TYPE:@n
 * 	- "hh"	Signed or unsigned char.@n
 * 	- "h"	Signed or unsigned short.@n
 * 	- ""	Signed or unsigned int (default value).@n
 * 	- "l"	Signed or unsigned long int.@n
 * 	- "ll"	Signed or unsigned long long int.@n
 *
 *
 * CONVERSION:@n
 * 	- %	Print percentile character itself.
 *
 * 	- c	Print single character.
 *
 * 	- s	Print zero terminated string. If a NULL value is passed as
 *		value, "(NULL)" is printed instead.
 *
 * 	- P, p	Print value of a pointer. Void * value is expected and it is
 *		printed in hexadecimal notation with prefix (as with \%#X / \%#x
 *		for 32-bit or \%#X / \%#x for 64-bit long pointers).
 *
 * 	- b	Print value as unsigned binary number. Prefix is not printed by
 *		default. (Nonstandard extension.)
 *
 * 	- o	Print value as unsigned octal number. Prefix is not printed by
 *		default.
 *
 * 	- d, i	Print signed decimal number. There is no difference between d
 *		and i conversion.
 *
 * 	- u	Print unsigned decimal number.
 *
 * 	- X, x	Print hexadecimal number with upper- or lower-case. Prefix is
 *		not printed by default.
 *
 * All other characters from fmt except the formatting directives are printed in
 * verbatim.
 *
 * @param fmt	Formatting NULL terminated string.
 * @param ps	TODO.
 * @param ap	TODO.
 * @return	Number of characters printed, negative value on failure.
 */
static int printf_core(const char *fmt, struct printf_spec *ps, va_list ap)
{
	int i = 0;		/* Index of the currently processed char from fmt */
	int j = 0;		/* Index to the first not printed nonformating character */
	int end;
	int counter;		/* Counter of printed characters */
	int retval;		/* Used to store return values from called functions */
	char c;
	qualifier_t qualifier;	/* Type of argument */
	int base;		/* Base in which a numeric parameter will be printed */
	uint64_t number;	/* Argument value */
	size_t size;		/* Byte size of integer parameter */
	int width, precision;
	uint64_t flags;

	counter = 0;

	while ((c = fmt[i])) {
		/* Control character. */
		if (c == '%') {
			/* Print common characters if any processed. */
			if (i > j) {
				if ((retval = printf_putnchars(&fmt[j],
				    (size_t) (i - j), ps)) < 0)
				    	return retval;
				counter += retval;
			}

			j = i;
			/* Parse modifiers. */
			flags = 0;
			end = 0;

			do {
				++i;
				switch (c = fmt[i]) {
				case '#':
					flags |= __PRINTF_FLAG_PREFIX;
					break;
				case '-':
					flags |= __PRINTF_FLAG_LEFTALIGNED;
					break;
				case '+':
					flags |= __PRINTF_FLAG_SHOWPLUS;
					break;
				case ' ':
					flags |= __PRINTF_FLAG_SPACESIGN;
					break;
				case '0':
					flags |= __PRINTF_FLAG_ZEROPADDED;
					break;
				default:
					end = 1;
				};

			} while (end == 0);

			/* Width & '*' operator. */
			width = 0;
			if (isdigit(fmt[i])) {
				while (isdigit(fmt[i])) {
					width *= 10;
					width += fmt[i++] - '0';
				}
			} else if (fmt[i] == '*') {
				/* Get width value from argument list. */
				i++;
				width = (int)va_arg(ap, int);
				if (width < 0) {
					/* Negative width sets '-' flag. */
					width *= -1;
					flags |= __PRINTF_FLAG_LEFTALIGNED;
				}
			}

			/* Precision and '*' operator. */
			precision = -1;
			if (fmt[i] == '.') {
				++i;
				precision = 0;
				if (isdigit(fmt[i])) {
					while (isdigit(fmt[i])) {
						precision *= 10;
						precision += fmt[i++] - '0';
					}
				} else if (fmt[i] == '*') {
					/* Get precision from argument list. */
					i++;
					precision = (int)va_arg(ap, int);
				}
			}

			switch (fmt[i++]) {
			/** @todo unimplemented qualifiers:
			 * t ptrdiff_t - ISO C 99
			 */
			case 'h':	/* char or short */
				qualifier = PrintfQualifierShort;
				if (fmt[i] == 'h') {
					i++;
					qualifier = PrintfQualifierByte;
				}
				break;
			case 'z':	/* size_t or ssize_t */
				qualifier = PrintfQualifierLong;
				break;
			case 'l':	/* long or long long */
				qualifier = PrintfQualifierLong;
				if (fmt[i] == 'l') {
					i++;
					qualifier = PrintfQualifierLongLong;
				}
				break;
			default:
				/* default type */
				qualifier = PrintfQualifierInt;
				--i;
			}

			base = 10;

			switch (c = fmt[i]) {
			/* String and character conversions */
			case 's':
				if ((retval = print_string(va_arg(ap, char *),
				    width, precision, flags, ps)) < 0)
				    	return retval;
				counter += retval;
				j = i + 1;
				goto next_char;
			case 'c':
				c = va_arg(ap, unsigned int);
				if ((retval = print_char(c, width, flags, ps)) < 0)
					return retval;
				counter += retval;
				j = i + 1;
				goto next_char;

			/* Integer values */
			case 'P':	/* pointer */
				flags |= __PRINTF_FLAG_BIGCHARS;
				__fallthrough;
			case 'p':
				flags |= __PRINTF_FLAG_PREFIX;
				base = 16;
				qualifier = PrintfQualifierPointer;
				break;
			case 'b':
				base = 2;
				break;
			case 'o':
				base = 8;
				break;
			case 'd':
			case 'i':
				flags |= __PRINTF_FLAG_SIGNED;
				break;
			case 'u':
				break;
			case 'X':
				flags |= __PRINTF_FLAG_BIGCHARS;
				__fallthrough;
			case 'x':
				base = 16;
				break;
			case '%': /* percentile itself */
				j = i;
				goto next_char;
			default: /* Bad formatting */
				/*
				 * Unknown format. Now, j is the index of '%'
				 * so we will print whole bad format sequence.
				 */
				goto next_char;
			}

			/* Print integers. */
			/* Print number. */
			switch (qualifier) {
			case PrintfQualifierByte:
				size = sizeof(unsigned char);
				number = (uint64_t) va_arg(ap, unsigned int);
				break;
			case PrintfQualifierShort:
				size = sizeof(unsigned short);
				number = (uint64_t) va_arg(ap, unsigned int);
				break;
			case PrintfQualifierInt:
				size = sizeof(unsigned int);
				number = (uint64_t) va_arg(ap, unsigned int);
				break;
			case PrintfQualifierLong:
				size = sizeof(unsigned long);
				number = (uint64_t) va_arg(ap, unsigned long);
				break;
			case PrintfQualifierLongLong:
				size = sizeof(unsigned long long);
				number = (uint64_t) va_arg(ap, unsigned long long);
				break;
			case PrintfQualifierPointer:
				size = sizeof(void *);
				number = (uint64_t) (unsigned long)va_arg(ap, void *);
				break;
			}

			if (flags & __PRINTF_FLAG_SIGNED) {
				if (number & (0x1ULL << (size * 8 - 1))) {
					flags |= __PRINTF_FLAG_NEGATIVE;

					if (size == sizeof(uint64_t)) {
						number = -((int64_t) number);
					} else {
						number = ~number;
						number &= ~(0xFFFFFFFFFFFFFFFFll << (size * 8));
						number++;
					}
				}
			}

			if ((retval = print_number(number, width, precision,
						   base, flags, ps)) < 0)
				return retval;

			counter += retval;
			j = i + 1;
		}
next_char:
		++i;
	}

	if (i > j) {
		if ((retval = printf_putnchars(&fmt[j],
		    (u64) (i - j), ps)) < 0)
		    	return retval;
		counter += retval;
	}

	return counter;
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vsnprintf(str, size, fmt, args);
	va_end(args);

	return ret;
}

int sprintf(char *str, const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vsprintf(str, fmt, args);
	va_end(args);

	return ret;
}

int fprintf(FILE *file, const char *fmt, ...)
{
	int ret;
	if ((file == stdout) || (file == stderr)) {
		va_list args;
		va_start(args, fmt);
		ret = vprintf(fmt, args);
		va_end(args);

		return ret;
	}
	return -1;
}

struct vsnprintf_data {
	size_t size;		/* Total space for string */
	size_t len;		/* Count of currently used characters */
	char *string;		/* Destination string */
};

/**
 * Write string to given buffer.
 *
 * Write at most data->size characters including trailing zero. According to
 * C99, snprintf() has to return number of characters that would have been
 * written if enough space had been available. Hence the return value is not
 * number of really printed characters but size of the input string.
 * Number of really used characters is stored in data->len.
 *
 * @param str	Source string to print.
 * @param count	Size of source string.
 * @param _data	Structure with destination string, counter of used space
 *              and total string size.
 * @return Number of characters to print (not characters really printed!).
 */
static int vsnprintf_write(const char *str, size_t count, void *_data)
{
	struct vsnprintf_data *data = _data;
	size_t i;

	i = data->size - data->len;
	if (i == 0)
		return count;

	/* We have only one free byte left in buffer => write trailing zero. */
	if (i == 1) {
		data->string[data->size - 1] = 0;
		data->len = data->size;
		return count;
	}

	/*
	 * We have not enough space for whole string with the trailing
	 * zero => print only a part of string.
	 */
	if (i <= count) {
		memcpy((void *)(data->string + data->len), (void *)str, i - 1);
		data->string[data->size - 1] = 0;
		data->len = data->size;
		return count;
	}

	/* Buffer is big enough to print whole string. */
	memcpy((void *)(data->string + data->len), (void *)str, count);
	data->len += count;
	/*
	 * Put trailing zero at end, but not count it into data->len so
	 * it could be rewritten next time.
	 */
	data->string[data->len] = 0;

	return count;
}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
	struct vsnprintf_data data = { size, 0, str };
	struct printf_spec ps = { vsnprintf_write, &data };

	/* Print 0 at end of string - fix case that nothing will be printed. */
	if (size > 0)
		str[0] = 0;

	/* vsnprintf_write() ensures that str will be terminated by zero. */
	return printf_core(fmt, &ps, ap);
}

int vsprintf(char *str, const char *fmt, va_list ap)
{
	return vsnprintf(str, (size_t) - 1, fmt, ap);
}

int printf(const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vprintf(fmt, args);
	va_end(args);

	return ret;
}

static int vprintf_write(const char *str, size_t count, void *unused)
{
	console_write(str, count);
	return count;
}

int vprintf(const char *fmt, va_list ap)
{
	struct printf_spec ps = { vprintf_write, NULL };

	return printf_core(fmt, &ps, ap);
}
