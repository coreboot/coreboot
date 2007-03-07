/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

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
#include <console/console.h>

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
