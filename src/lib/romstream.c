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

struct stream *
stream_start(struct stream *stream, u64 base, u32 size)
{
	/* this seems not needed but we're allowing for future changes */
	u8 *romstream = v;
#ifdef CONFIG_X86
	stream->base = u64(phys_to_virt(base));
	stream->size = size;
#else
#error "using romstream on non-x86 is not supported?"
#endif
	return romstream;
}

/* This is a memory-addressable stream. So we will be setting *where to the address */
int
stream_read(struct stream *stream, void *where, u32 len, u32 off)
{
	u8 *rom = (u8 *)stream->base;
	u32 amount;
	int i;
	u8 *from, *to;

	if (off >= stream->size)
		return 0;

	amount = stream->size - off;
	if (amount > len)
		amount = len;

	to = where;
	from = rom + off;
	
	for(i = 0; i < amount; i++)
		*from++ = *to++;

	return amount;
}

void
stream_fini(void *stream)
{
}
