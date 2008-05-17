/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2004 Tyan Corporation
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
 * Copyright (C) 2007 Reinder E.N. de Haan <lb_reha@mveas.com>
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
#include "flash.h"

extern int exclude_start_page, exclude_end_page;

void write_lockbits_49fl00x(volatile uint8_t *bios, int size,
					      unsigned char bits, int block_size)
{
	int i, left = size;

	for (i = 0; left >= block_size; i++, left -= block_size) {

		/* pm49fl002 */
		if (block_size == 16384 && i%2)
			continue;

		*(bios + (i * block_size) + 2) = bits;
	}
}

int probe_49fl00x(struct flashchip *flash)
{
	int ret = probe_jedec(flash);
	
	if (ret == 1)
		map_flash_registers(flash);

	return ret;
}

int erase_49fl00x(struct flashchip *flash)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	/* unprotected */
	write_lockbits_49fl00x(flash->virtual_registers, total_size, 0, page_size);

	//erase_chip_jedec will not work ... datasheet says "Chip erase is available in A/A Mux Mode only"
	printf("Erasing page: ");
	for (i = 0; i < total_size / page_size; i++) {
		if ((i >= exclude_start_page) && (i < exclude_end_page))
			continue;

		/* erase the page */
		erase_block_jedec(bios, i * page_size);
		printf("%04d at address: 0x%08x", i, i * page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		fflush(stdout);
	}
	printf("\n");

	/* protected */
	write_lockbits_49fl00x(flash->virtual_registers, total_size, 1, page_size);

	return 0;
}

int write_49fl00x(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	/* unprotected */
	write_lockbits_49fl00x(flash->virtual_registers, total_size, 0, page_size);

	printf("Programming page: ");
	for (i = 0; i < total_size / page_size; i++) {
		if ((i >= exclude_start_page) && (i < exclude_end_page))
			continue;

		/* erase the page before programming */
		erase_block_jedec(bios, i * page_size);

		/* write to the sector */
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_sector_jedec(bios, buf + i * page_size,
				   bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		fflush(stdout);
	}
	printf("\n");
	
	/* protected */
	write_lockbits_49fl00x(flash->virtual_registers, total_size, 1, page_size);

	return 0;
}
