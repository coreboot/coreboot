/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
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

#include <console/console.h>
#include <stdlib.h>
#include "../flash.h"

struct data_amd800
{
    unsigned base;
    unsigned spacing;
    unsigned cs;
    const char *tag;
};

static const char *identify_amd (struct flash_device *flash_device);
static int erase_flash_amd800 (void *data, unsigned offset, unsigned length);
static int program_flash_amd800 (void *data, unsigned offset, const void *source,
				 unsigned length);
static uint8_t read_byte_amd800(void *data, unsigned offset);

static flash_fn fn_amd800 = {
    identify_amd,
    0,
    0,
    erase_flash_amd800,
    program_flash_amd800,
    read_byte_amd800
};

const char *identify_amd (struct flash_device *flash_device)
{
    struct data_amd800 *d800 = flash_device->data;

    if (!d800->tag)
    {
	volatile unsigned char *flash =

	    (volatile unsigned char *) d800->base;
	unsigned char type,
	 id;

	*(flash + 0xaaa * d800->spacing) = 0xaa;
	*(flash + 0x555 * d800->spacing) = 0x55;
	*(flash + 0xaaa * d800->spacing) = 0x90;
	type = *(flash + 2 * d800->spacing);
	id = *flash;
	*flash = 0xf0;
	if ((id == 1 || id == 0x20) && type == 0x5b)
	{
	    d800->cs = 45;
	    d800->tag = "Am29LV800BB";
    	    flash_device->base = d800->base;
    	    flash_device->size = 1024*1024;
	    flash_device->erase_size = 64*1024;
	    flash_device->store_size = 1;
	}
	else
	{
	    printk_info("Unknown flash ID: 0x%02x 0x%02x\n", id, type);
	}
    }
    return d800->tag;
}

int erase_flash_amd800 (void *data, unsigned offset, unsigned length)
{
    struct data_amd800 *d800 = data;
    volatile unsigned char *flash = (volatile unsigned char *) d800->base;
    volatile unsigned char *flash_aaa = flash + 0xaaa * d800->spacing;
    volatile unsigned char *flash_555 = flash + 0x555 * d800->spacing;
    int id;
    int cs = 9999;

    printk_info("Erase from 0x%08x to 0x%08x\n", offset, offset + length);
    *flash_aaa = 0xAA;		// Chip Erase
    *flash_555 = 0x55;
    *flash_aaa = 0x80;
    *flash_aaa = 0xAA;
    *flash_555 = 0x55;
    *flash_aaa = 0x10;

    for (; cs > 0; cs--)
    {
	id = *(flash + 16);
	if (id & 0xA0)		// DQ7 or DQ5 set: done or error
	    break;
	printk_info("%4d\b\b\b\b", cs);
    }

    *flash_aaa = 0xF0;		// In case of error

    printk_info("\b\b\b\b    \b\b\b\b");
    if (cs == 0)
    {
	printk_info("Could not erase flash, timeout.\n");
	return -1;
    }
    else if ((id & 0x80) == 0)
    {
	printk_info("Could not erase flash, status=%02x.\n", id);
	return -1;
    }
    printk_info("Flash erased\n");
    return 0;
}

int init_flash_amd800 (char *tag, unsigned base, unsigned spacing)
{
    struct data_amd800 *data = malloc (sizeof (struct data_amd800));

    if (data)
    {
	data->base = base;
	data->spacing = spacing;
	data->tag = 0;
	if (register_flash_device (&fn_amd800, tag, data) < 0)
	{
	    free (data);
	    return -1;
	}
    }
    else
	return -1;
    return 0;
}

int program_flash_amd800 (void *data, unsigned offset, const void *source,
			  unsigned length)
{
    struct data_amd800 *d800 = data;
    volatile unsigned char *flash = (volatile unsigned char *) d800->base;
    volatile unsigned char *flash_aaa = flash + 0xaaa * d800->spacing;
    volatile unsigned char *flash_555 = flash + 0x555 * d800->spacing;
    int id = 0;
    int cs;
    int errs = 0;
    volatile char *s;
    volatile char *d;

    printk_info("Program from 0x%08x to 0x%08x\n", offset, offset + length);
    printk_info("Data at %p\n", source);

    *flash_aaa = 0xAA;		// Unlock Bypass
    *flash_555 = 0x55;
    *flash_aaa = 0x20;

    s = (unsigned char *) source;
    d = flash + offset * d800->spacing;
    cs = length;

    while (cs > 0 && !errs)
    {
	*flash = 0xA0;	// Unlock Bypass Program
	*d = *s;		// Program data

	while (1)
	{
	    id = *d;
	    if ((id & 0x80) == (*s & 0x80))	// DQ7 right? => program done
		break;
	    else if (id & 0x20)
	    {			// DQ5 set? => maybe errors
		id = *d;
		if ((id & 0x80) != (*s & 0x80))
		{
		    errs++;
		    break;
		}
	    }
	}

	// PRINT("Set %08lx = %02x\n", d, *d);

	s += 1;
	d += d800->spacing;
	cs--;
    }

    *flash = 0x90;		// Unlock Bypass Program Reset
    *flash = 0x00;
    *flash = 0xF0;

    if (errs != 0)
    {
	printk_info("FAIL: Status=%02x Address=%p.\n", id, d - d800->spacing);
	return -1;
    }
    printk_info("OK.\n");


    // Step 4: Verify the flash.

    printk_info("  Verifying flash : ...");
    errs = 0;
    s = (unsigned char *) source;
    d = flash + offset * d800->spacing;
    for (cs = 0; cs < length; cs++)
    {
	if (*s != *d)
	{
	    if (errs == 0)
		printk_info("ERROR: Addr: %08p, PCI: %02x Lcl: %02x.\n",
		       s, *s, *d);
	    errs++;
	}
	s += 1;
	d += d800->spacing;
    }

    if (errs == 0)
	printk_info("OK.\n");
    else
    {
	printk_info("  FAIL: %d errors.\n", errs);
	return -1;
    }

    return 0;
}

uint8_t read_byte_amd800 (void *data, unsigned offset)
{
    struct data_amd800 *d800 = data;
    volatile unsigned char *flash = (volatile unsigned char *) d800->base;
    return *(flash + offset * d800->spacing);
}

