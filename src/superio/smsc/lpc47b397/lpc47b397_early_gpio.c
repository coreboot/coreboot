/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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

static void lpc47b397_gpio_offset_out(unsigned iobase, unsigned offset, unsigned value)
{
	outb(value,iobase+offset);
}

static unsigned lpc47b397_gpio_offset_in(unsigned iobase, unsigned offset)
{
	return inb(iobase+offset);
}

#if 0
/* for GP60-GP64, GP66-GP85 */
#define LPC47B397_GPIO_CNTL_INDEX 0x70
#define LPC47B397_GPIO_CNTL_DATA 0x71

static void lpc47b397_gpio_index_out(unsigned iobase, unsigned index, unsigned value)
{
	outb(index,iobase+LPC47B397_GPIO_CNTL_INDEX);
	outb(value, iobase+LPC47B397_GPIO_CNTL_DATA);
}

static unsigned lpc47b397_gpio_index_in(unsigned iobase, unsigned index)
{
	outb(index,iobase+LPC47B397_GPIO_CNTL_INDEX);
	return inb(iobase+LPC47B397_GPIO_CNTL_DATA);
}
#endif
