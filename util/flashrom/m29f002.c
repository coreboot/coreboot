/*
 * This file is part of flashrom.
 *
 * Copyright (C) 2009 Peter Stuge <peter@stuge.se>
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

#include "flash.h"

int erase_m29f002(struct flashchip *flash) {
	volatile uint8_t *bios = flash->virtual_memory;
	*(volatile uint8_t *)(bios + 0x555) = 0xaa;
	*(volatile uint8_t *)(bios + 0xaaa) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0x80;
	*(volatile uint8_t *)(bios + 0x555) = 0xaa;
	*(volatile uint8_t *)(bios + 0xaaa) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0x10;
	myusec_delay(10);
	toggle_ready_jedec(bios);
	return 0;
}

static void rewrite_block(volatile uint8_t *bios, uint8_t *src, volatile uint8_t *dst, int size) {
	/* erase */
	*(volatile uint8_t *)(bios + 0x555) = 0xaa;
	*(volatile uint8_t *)(bios + 0xaaa) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0x80;
	*(volatile uint8_t *)(bios + 0x555) = 0xaa;
	*(volatile uint8_t *)(bios + 0xaaa) = 0x55;
	*dst = 0x30;
	myusec_delay(10);
	toggle_ready_jedec(bios);

	/* program */
	while (size--) {
		*(volatile uint8_t *)(bios + 0x555) = 0xaa;
		*(volatile uint8_t *)(bios + 0xaaa) = 0x55;
		*(volatile uint8_t *)(bios + 0x555) = 0xa0;
		*dst = *src;
		toggle_ready_jedec(dst);
		dst++;
		src++;
	}
}

static void do_block(volatile uint8_t *bios, uint8_t *src, int i, unsigned long start, int size) {
	printf("%d at address: 0x%08lx", i, start);
	rewrite_block(bios, src + start, bios + start, size);
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
}

int write_m29f002t(struct flashchip *flash, uint8_t *buf) {
	int i, page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	/* M29F002(N)T has 7 blocks. From bottom to top their sizes are:
	 * 64k 64k 64k 32k 8k 8k 16k
	 * flash->page_size is set to 64k in flashchips.c
	 */

	printf("Programming block: ");
	for (i = 0; i < 3; i++)
		do_block(bios, buf, i, i*page_size, page_size);
	do_block(bios, buf, i++, 0x30000, 32*1024);
	do_block(bios, buf, i++, 0x38000, 8*1024);
	do_block(bios, buf, i++, 0x3a000, 8*1024);
	do_block(bios, buf, i, 0x3c000, 16*1024);

	printf("\n");
	return 0;
}

int write_m29f002b(struct flashchip *flash, uint8_t *buf) {
	int i = 0, page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	/* M29F002B has 7 blocks. From bottom to top their sizes are:
	 * 16k 8k 8k 32k 64k 64k 64k
	 * flash->page_size is set to 64k in flashchips.c
	 */

	printf("Programming block: ");
	do_block(bios, buf, i++, 0x00000, 16*1024);
	do_block(bios, buf, i++, 0x04000, 8*1024);
	do_block(bios, buf, i++, 0x06000, 8*1024);
	do_block(bios, buf, i++, 0x08000, 32*1024);
	for (; i < 7; i++)
		do_block(bios, buf, i, (i-3)*page_size, page_size);

	printf("\n");
	return 0;
}
