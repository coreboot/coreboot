/*
 * (C) Copyright 2001
 * Humboldt Solutions Ltd, adrian@humboldt.co.uk.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <arch/io.h>
#include "../nvram.h"

static unsigned bsp_size(struct nvram_device *data)
{
    return 8 * 1024;   
}

static int bsp_read_block(struct nvram_device *dev, unsigned offset,
	    unsigned char *data, unsigned length)
{
    unsigned i;
    
    for(i = 0; i < length; i++)
    {
	outb(((offset + i) >> 8) & 0xff, 0x74);
	outb((offset + i) & 0xff, 0x75);
	data[i] = inb(0x76);
    }
    return length;
}
   
static int bsp_write_byte(struct nvram_device *data, unsigned offset, unsigned char byte)
{
    outb((offset >> 8) & 0xff, 0x74);
    outb(offset & 0xff, 0x75);
    outb(byte, 0x76);
    return 1;
}

nvram_device bsp_nvram = {
    bsp_size, bsp_read_block, bsp_write_byte, 0, 0   
};
    
