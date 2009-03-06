/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
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

#include <stdio.h>
#include <stdint.h>
#include "flash.h"

static __inline__ int erase_sector_29f040b(volatile uint8_t *bios,
					   unsigned long address)
{
	chip_writeb(0xAA, bios + 0x555);
	chip_writeb(0x55, bios + 0x2AA);
	chip_writeb(0x80, bios + 0x555);
	chip_writeb(0xAA, bios + 0x555);
	chip_writeb(0x55, bios + 0x2AA);
	chip_writeb(0x30, bios + address);

	sleep(2);

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios + address);

	return 0;
}

static __inline__ int write_sector_29f040b(volatile uint8_t *bios,
					   uint8_t *src,
					   volatile uint8_t *dst,
					   unsigned int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		if ((i & 0xfff) == 0xfff)
			printf("0x%08lx", (unsigned long)dst -
			       (unsigned long)bios);

		chip_writeb(0xAA, bios + 0x555);
		chip_writeb(0x55, bios + 0x2AA);
		chip_writeb(0xA0, bios + 0x555);
		chip_writeb(*src++, dst++);

		/* wait for Toggle bit ready */
		toggle_ready_jedec(bios);

		if ((i & 0xfff) == 0xfff)
			printf("\b\b\b\b\b\b\b\b\b\b");
	}

	return 0;
}

int probe_29f040b(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;

	chip_writeb(0xAA, bios + 0x555);
	chip_writeb(0x55, bios + 0x2AA);
	chip_writeb(0x90, bios + 0x555);

	id1 = chip_readb(bios);
	id2 = chip_readb(bios + 0x01);

	chip_writeb(0xF0, bios);

	myusec_delay(10);

	printf_debug("%s: id1 0x%02x, id2 0x%02x\n", __FUNCTION__, id1, id2);
	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

int erase_29f040b(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;

	chip_writeb(0xAA, bios + 0x555);
	chip_writeb(0x55, bios + 0x2AA);
	chip_writeb(0x80, bios + 0x555);
	chip_writeb(0xAA, bios + 0x555);
	chip_writeb(0x55, bios + 0x2AA);
	chip_writeb(0x10, bios + 0x555);

	myusec_delay(10);
	toggle_ready_jedec(bios);

	return 0;
}

int write_29f040b(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	printf("Programming page ");
	for (i = 0; i < total_size / page_size; i++) {
		/* erase the page before programming */
		erase_sector_29f040b(bios, i * page_size);

		/* write to the sector */
		printf("%04d at address: ", i);
		write_sector_29f040b(bios, buf + i * page_size,
				     bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");

	return 0;
}
