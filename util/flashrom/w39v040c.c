/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2008 Peter Stuge <peter@stuge.se>
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

int probe_w39v040c(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2, lock;

	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0x90;
	myusec_delay(10);

	id1 = *(volatile uint8_t *)bios;
	id2 = *(volatile uint8_t *)(bios + 1);
	lock = *(volatile uint8_t *)(bios + 0xfff2);

	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0xF0;
	myusec_delay(40);

	printf_debug("%s: id1 0x%x, id2 0x%x", __func__, id1, id2);
	if (!oddparity(id1))
		printf_debug(", id1 parity violation");
	printf_debug("\n");
	if (flash->manufacture_id == id1 && flash->model_id == id2) {
		printf("%s: Boot block #TBL is %slocked, rest of chip #WP is %slocked.\n",
			__func__, lock & 0x4 ? "" : "un", lock & 0x8 ? "" : "un");
		return 1;
	}

	return 0;
}

int erase_w39v040c(struct flashchip *flash)
{
	int i;
	unsigned int total_size = flash->total_size * 1024;
	volatile uint8_t *bios = flash->virtual_memory;

	for (i = 0; i < total_size; i += flash->page_size)
		erase_sector_jedec(flash->virtual_memory, i);

	for (i = 0; i < total_size; i++)
		if (0xff != bios[i]) {
			printf("ERASE FAILED at 0x%08x!  Expected=0xff, Read=0x%02x\n", i, bios[i]);
			return -1;
		}

	return 0;
}

int write_w39v040c(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	if (flash->erase(flash))
		return -1;

	printf("Programming page: ");
	for (i = 0; i < total_size / page_size; i++) {
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_sector_jedec(bios, buf + i * page_size,
				   bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");

	return 0;
}
