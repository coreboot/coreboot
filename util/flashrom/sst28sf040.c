/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 * Copyright (C) 2005 coresystems GmbH <stepan@openbios.org>
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

#define AUTO_PG_ERASE1		0x20
#define AUTO_PG_ERASE2		0xD0
#define AUTO_PGRM		0x10
#define CHIP_ERASE		0x30
#define RESET			0xFF
#define READ_ID			0x90

static __inline__ void protect_28sf040(volatile uint8_t *bios)
{
	/* ask compiler not to optimize this */
	volatile uint8_t tmp;

	tmp = *(volatile uint8_t *)(bios + 0x1823);
	tmp = *(volatile uint8_t *)(bios + 0x1820);
	tmp = *(volatile uint8_t *)(bios + 0x1822);
	tmp = *(volatile uint8_t *)(bios + 0x0418);
	tmp = *(volatile uint8_t *)(bios + 0x041B);
	tmp = *(volatile uint8_t *)(bios + 0x0419);
	tmp = *(volatile uint8_t *)(bios + 0x040A);
}

static __inline__ void unprotect_28sf040(volatile uint8_t *bios)
{
	/* ask compiler not to optimize this */
	volatile uint8_t tmp;

	tmp = *(volatile uint8_t *)(bios + 0x1823);
	tmp = *(volatile uint8_t *)(bios + 0x1820);
	tmp = *(volatile uint8_t *)(bios + 0x1822);
	tmp = *(volatile uint8_t *)(bios + 0x0418);
	tmp = *(volatile uint8_t *)(bios + 0x041B);
	tmp = *(volatile uint8_t *)(bios + 0x0419);
	tmp = *(volatile uint8_t *)(bios + 0x041A);
}

static __inline__ int erase_sector_28sf040(volatile uint8_t *bios,
					   unsigned long address)
{
	*bios = AUTO_PG_ERASE1;
	*(bios + address) = AUTO_PG_ERASE2;

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios);

	return 0;
}

static __inline__ int write_sector_28sf040(volatile uint8_t *bios,
					   uint8_t *src,
					   volatile uint8_t *dst,
					   unsigned int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		/* transfer data from source to destination */
		if (*src == 0xFF) {
			dst++, src++;
			/* If the data is 0xFF, don't program it */
			continue;
		}
		/*issue AUTO PROGRAM command */
		*dst = AUTO_PGRM;
		*dst++ = *src++;

		/* wait for Toggle bit ready */
		toggle_ready_jedec(bios);
	}

	return 0;
}

int probe_28sf040(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;

	*bios = RESET;
	myusec_delay(10);

	*bios = READ_ID;
	myusec_delay(10);
	id1 = *(volatile uint8_t *)bios;
	myusec_delay(10);
	id2 = *(volatile uint8_t *)(bios + 0x01);

	*bios = RESET;
	myusec_delay(10);

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);
	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

int erase_28sf040(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;

	unprotect_28sf040(bios);
	*bios = CHIP_ERASE;
	*bios = CHIP_ERASE;
	protect_28sf040(bios);

	myusec_delay(10);
	toggle_ready_jedec(bios);

	return 0;
}

int write_28sf040(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	unprotect_28sf040(bios);

	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		/* erase the page before programming */
		erase_sector_28sf040(bios, i * page_size);

		/* write to the sector */
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_sector_28sf040(bios, buf + i * page_size,
				     bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");

	protect_28sf040(bios);

	return 0;
}
