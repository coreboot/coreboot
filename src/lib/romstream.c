/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <stdlib.h>
#include <lib.h>
#include <console/console.h>

/*
 * romstream. This is a very simple stream interface. 
 * The stream interface has three functions. The first, start, accepts a void * and a size and returns a void *
 * which points to an opaque, and possibly internal, interface value. 
 * the second, read, accepts the opaque pointer, a data pointer, a length, and an offset and returns
 * the number of bytes read or -1 if there is an error.
 * The third, end, frees any internal stucts (if needed) and shuts do the stream (if needed)
 * we assume that flash is 4G or less.
 * The romstream is very simple: it's a u8 pointer. 
 */

void *
stream_start(void *v)
{
	/* this seems not needed but we're allowing for future changes */
	u8 *romstream = v;
	return romstream;
}

int
stream_read(void *stream, void *where, u32 len, u32 off)
{
	u8 *romstream = stream;
	u32 amount;
	u8 *from, *to;
	int i;

	if (off >= CONFIG_ROM_SIZE)
		return 0;

	amount = CONFIG_ROM_SIZE - off;
	if (amount > len)
		amount = len;
	from =  romstream+off;
	to = where;
	/* we don't have memmove in all modes, so we'll just it by ourselves. */
	for(i = 0; i < amount; i++)
		*to++ = *from++;

	return amount;
}

void
stream_fini(void *stream)
{
}
