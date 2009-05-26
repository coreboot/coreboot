/*
 *  linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */
#include <stdarg.h>
#include <string.h>

int vtxprintf(void (*tx_byte)(unsigned char byte), const char *fmt, va_list args);

int vsprintf(char * buf, const char *fmt, va_list args)
{
	char *str_buf;

	/* this function is only used by vsprint.
	   To keep str_buf local (for reentrancy
	   and to avoid .bss use, nest it */
	void str_tx_byte(unsigned char byte)
	{
		*str_buf = byte;
		str_buf++;
	}

	int i;
	str_buf = buf;
	i = vtxprintf(str_tx_byte, fmt, args);
	*str_buf = '\0';
	return i;
}

int sprintf(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	return i;
}
