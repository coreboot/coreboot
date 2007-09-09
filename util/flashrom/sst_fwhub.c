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

/* Adapted from the Intel FW hub stuff for 82802ax parts. */

#include <stdio.h>
#include "flash.h"

// I need that Berkeley bit-map printer
void print_sst_fwhub_status(uint8_t status)
{
	printf("%s", status & 0x80 ? "Ready:" : "Busy:");
	printf("%s", status & 0x40 ? "BE SUSPEND:" : "BE RUN/FINISH:");
	printf("%s", status & 0x20 ? "BE ERROR:" : "BE OK:");
	printf("%s", status & 0x10 ? "PROG ERR:" : "PROG OK:");
	printf("%s", status & 0x8 ? "VP ERR:" : "VPP OK:");
	printf("%s", status & 0x4 ? "PROG SUSPEND:" : "PROG RUN/FINISH:");
	printf("%s", status & 0x2 ? "WP|TBL#|WP#,ABORT:" : "UNLOCK:");
}

/* probe_jedec works fine for probing */
int probe_sst_fwhub(struct flashchip *flash)
{
	if (probe_jedec(flash) == 0)
		return 0;

	map_flash_registers(flash);

	return 1;
}

int erase_sst_fwhub_block(struct flashchip *flash, int offset)
{
	volatile uint8_t *wrprotect = flash->virtual_registers + offset + 2;

	// clear write protect
	*(wrprotect) = 0;

	erase_block_jedec(flash->virtual_memory, offset);
	toggle_ready_jedec(flash->virtual_memory);

	return 0;
}

int erase_sst_fwhub(struct flashchip *flash)
{
	int i;
	unsigned int total_size = flash->total_size * 1024;

	for (i = 0; i < total_size; i += flash->page_size)
		erase_sst_fwhub_block(flash, i);

	return 0;
}

int write_sst_fwhub(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	// FIXME: We want block wide erase instead of ironing the whole chip
	erase_sst_fwhub(flash);

	// dumb check if erase was successful.
	for (i = 0; i < total_size; i++) {
		if (bios[i] != 0xff) {
			printf("ERASE FAILED\n");
			return -1;
		}
	}

	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_sector_jedec(bios, buf + i * page_size,
				   bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");

	return 0;
}
