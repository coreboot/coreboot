/*
 * This file is part of the coreboot project.
 *
 * It is based on the Linux kernel (lib/vsprintf.c).
 *
 * Modifications are:
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 */

/* Copyright (C) 1991, 1992  Linus Torvalds */
/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/* Wirzenius wrote this portably, Torvalds fucked it up :-) */

#include <stdarg.h>
#include <string.h>
#include <console.h>

int vtxprintf(void (*tx_byte) (unsigned char byte, void *arg), void *arg, const char *fmt,
	      va_list args);

/* the arg is the char ** for the buffer */
static void str_tx_byte(unsigned char byte, void *arg)
{
	unsigned char *cp = *(unsigned char **) arg;
	
	*cp = byte;
	cp++;
	/* paranoia, make sure it will be null terminated. The cost for this is small, 
	 * the benefit large.
	 */
	*cp = 0;
	*(unsigned char **) arg = cp;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;
	unsigned char *cp = (unsigned char *)buf;
	unsigned char **cpp = &cp;
	va_start(args, fmt);
	i = vtxprintf(str_tx_byte, cpp, fmt, args);
	va_end(args);
	return i;
}
