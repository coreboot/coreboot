/*
 * mx29f002.c: driver for MXIC MX29F002 flash models
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
 *	MX29F002/002N data sheet
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "flash.h"

int probe_29f002(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;

	*(bios + 0x5555) = 0xAA;
	*(bios + 0x2AAA) = 0x55;
	*(bios + 0x5555) = 0x90;

	id1 = *(volatile uint8_t *)bios;
	id2 = *(volatile uint8_t *)(bios + 0x01);

	*bios = 0xF0;

	myusec_delay(10);

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);
	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

int erase_29f002(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;

	*(bios + 0x555) = 0xF0;
	*(bios + 0x555) = 0xAA;
	*(bios + 0x2AA) = 0x55;
	*(bios + 0x555) = 0x80;
	*(bios + 0x555) = 0xAA;
	*(bios + 0x2AA) = 0x55;
	*(bios + 0x555) = 0x10;

	myusec_delay(100);
	toggle_ready_jedec(bios);

	//   while ((*bios & 0x40) != 0x40)
	//;

#if 0
	toggle_ready_jedec(bios);
	*(bios + 0x0ffff) = 0x30;
	*(bios + 0x1ffff) = 0x30;
	*(bios + 0x2ffff) = 0x30;
	*(bios + 0x37fff) = 0x30;
	*(bios + 0x39fff) = 0x30;
	*(bios + 0x3bfff) = 0x30;
#endif

	return (0);
}

int write_29f002(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	volatile uint8_t *bios = flash->virtual_memory;
	volatile uint8_t *dst = bios;

	*bios = 0xF0;
	myusec_delay(10);
	erase_29f002(flash);
	//*bios = 0xF0;
#if 1
	printf("Programming Page: ");
	for (i = 0; i < total_size; i++) {
		/* write to the sector */
		if ((i & 0xfff) == 0)
			printf("address: 0x%08lx", (unsigned long)i);
		*(bios + 0x5555) = 0xAA;
		*(bios + 0x2AAA) = 0x55;
		*(bios + 0x5555) = 0xA0;
		*dst++ = *buf++;

		/* wait for Toggle bit ready */
		toggle_ready_jedec(dst);

		if ((i & 0xfff) == 0)
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
#endif
	printf("\n");

	return (0);
}
