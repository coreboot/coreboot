/*
 * jedec.c: driver for programming JEDEC standard flash parts
 *
 *
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *
 * $Id$
 */

#include "flash.h"
#include "jedec.h"

int probe_jedec (struct flashchip * flash)
{
	char * volatile bios = flash->virt_addr;
	unsigned char  id1, id2;

	*(char *) (bios + 0x5555) = 0xAA;
	*(char *) (bios + 0x2AAA) = 0x55;
	*(char *) (bios + 0x5555) = 0x90;

	usleep(10);

	id1 = *(unsigned char *) bios;
	id2 = *(unsigned char *) (bios + 0x01);

	*(char *) (bios + 0x5555) = 0xAA;
	*(char *) (bios + 0x2AAA) = 0x55;
	*(char *) (bios + 0x5555) = 0xF0;

	usleep(10);

	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

int erase_jedec (struct flashchip * flash)
{
	char * volatile bios = flash->virt_addr;

	*(char *) (bios + 0x5555) = 0xAA;
	*(char *) (bios + 0x2AAA) = 0x55;
	*(char *) (bios + 0x5555) = 0x80;

	*(char *) (bios + 0x5555) = 0xAA;
	*(char *) (bios + 0x2AAA) = 0x55;
	*(char *) (bios + 0x5555) = 0x10;

	usleep(10);
	toggle_ready_jedec(bios);
}

int write_jedec (struct flashchip * flash, char * buf)
{
	int i;
	int total_size = flash->total_size *1024, page_size = flash->page_size;
	char * bios = flash->virt_addr;

	erase_jedec (flash);
	printf ("Programming Page: ");
	for (i = 0; i < total_size/page_size; i++) {
		printf ("%04d at address: 0x%08x", i, i * page_size);
		write_page_jedec(bios, buf + i * page_size, bios + i * page_size,
				 page_size);
		printf ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");
	protect_jedec (bios);
}
