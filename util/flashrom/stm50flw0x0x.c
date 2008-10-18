/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2008 Claus Gindhart <claus.gindhart@kontron.com>
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

/*
 * This module is designed for supporting the devices
 * ST M50FLW040A (not yet tested)
 * ST M50FLW040B (not yet tested)
 * ST M50FLW080A
 * ST M50FLW080B (not yet tested)
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "flash.h"

void protect_stm50flw0x0x(volatile uint8_t *bios)
{
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xA0;

	usleep(200);
}

int probe_stm50flw0x0x(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;
	uint32_t largeid1, largeid2;

	/* Issue JEDEC Product ID Entry command */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0x90;
	myusec_delay(40);

	/* Read product ID */
	id1 = *(volatile uint8_t *)bios;
	id2 = *(volatile uint8_t *)(bios + 0x01);
	largeid1 = id1;
	largeid2 = id2;

	/* Check if it is a continuation ID, this should be a while loop. */
	if (id1 == 0x7F) {
		largeid1 <<= 8;
		id1 = *(volatile uint8_t *)(bios + 0x100);
		largeid1 |= id1;
	}
	if (id2 == 0x7F) {
		largeid2 <<= 8;
		id2 = *(volatile uint8_t *)(bios + 0x101);
		largeid2 |= id2;
	}

	/* Issue JEDEC Product ID Exit command */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0xF0;
	myusec_delay(40);

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, largeid1,
		     largeid2);

	if (largeid1 != flash->manufacture_id || largeid2 != flash->model_id)
		return 0;

	map_flash_registers(flash);

	return 1;
}

static void wait_stm50flw0x0x(volatile uint8_t *bios)
{
	uint8_t id1;
	// id2;

	*bios = 0x70;
	if ((*bios & 0x80) == 0) {	// it's busy
		while ((*bios & 0x80) == 0) ;
	}
	// put another command to get out of status register mode

	*bios = 0x90;
	myusec_delay(10);

	id1 = *(volatile uint8_t *)bios;

	// this is needed to jam it out of "read id" mode
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xF0;
}

/*
 * claus.gindhart@kontron.com
 * The ST M50FLW080B and STM50FLW080B chips have to be unlocked,
 * before you can erase them or write to them.
 */
int unlock_block_stm50flw0x0x(struct flashchip *flash, int offset)
{
	volatile uint8_t *flash_addr = flash->virtual_registers + 2;
	const uint8_t unlock_sector = 0x00;
	int j;

	/*
	 * These chips have to be unlocked before you can erase them or write
	 * to them. The size of the locking sectors depends on the type
	 * of chip.
	 *
	 * Sometimes, the BIOS does this for you; so you propably
	 * don't need to worry about that.
	 */

	/* Check, if it's is a top/bottom-block with 4k-sectors. */
	/* TODO: What about the other types? */
	if ((offset == 0) ||
	    (offset == (flash->model_id == ST_M50FLW080A ? 0xE0000 : 0x10000))
	    || (offset == 0xF0000)) {

		// unlock each 4k-sector
		for (j = 0; j < 0x10000; j += 0x1000) {
			printf_debug("unlocking at 0x%x\n", offset + j);
			*(flash_addr + offset + j) = unlock_sector;
			if (*(flash_addr + offset + j) != unlock_sector) {
				printf("Cannot unlock sector @ 0x%x\n",
				       offset + j);
				return -1;
			}
		}
	} else {
		printf_debug("unlocking at 0x%x\n", offset);
		*(flash_addr + offset) = unlock_sector;
		if (*(flash_addr + offset) != unlock_sector) {
			printf("Cannot unlock sector @ 0x%x\n", offset);
			return -1;
		}
	}

	return 0;
}

int erase_block_stm50flw0x0x(struct flashchip *flash, int offset)
{
	volatile uint8_t *bios = flash->virtual_memory + offset;
	int j;

	// clear status register
	*bios = 0x50;
	printf_debug("Erase at %p\n", bios);
	// now start it
	*(volatile uint8_t *)(bios) = 0x20;
	*(volatile uint8_t *)(bios) = 0xd0;
	myusec_delay(10);

	wait_stm50flw0x0x(flash->virtual_memory);

	for (j = 0; j < flash->page_size; j++) {
		if (*(bios + j) != 0xFF) {
			printf("Erase failed at 0x%x\n", offset + j);
			return -1;
		}
	}

	printf("DONE BLOCK 0x%x\n", offset);

	return 0;
}

int write_page_stm50flw0x0x(volatile uint8_t *bios, uint8_t *src,
			    volatile uint8_t *dst, int page_size)
{
	int i, rc = 0;
	volatile uint8_t *d = dst;
	uint8_t *s = src;

	/* transfer data from source to destination */
	for (i = 0; i < page_size; i++) {
		*dst = 0x40;
		*dst++ = *src++;
		wait_stm50flw0x0x(bios);
	}

/* claus.gindhart@kontron.com
 * TODO
 * I think, that verification is not required, but
 * i leave it in anyway
 */
	dst = d;
	src = s;
	for (i = 0; i < page_size; i++) {
		if (*dst != *src) {
			rc = -1;
			break;
		}
		dst++;
		src++;
	}

	if (rc) {
		fprintf(stderr, " page %d failed!\n",
			(unsigned int)(d - bios) / page_size);
	}

	return rc;
}

/* I simply erase block by block
 * I Chip This is not the fastest way, but it works
 */
int erase_stm50flw0x0x(struct flashchip *flash)
{
	int i, rc = 0;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	printf("Erasing page:\n");
	for (i = 0; (i < total_size / page_size) && (rc == 0); i++) {
		printf
		    ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("%04d at address: 0x%08x ", i, i * page_size);
		rc = unlock_block_stm50flw0x0x(flash, i * page_size);
		if (!rc)
			rc = erase_block_stm50flw0x0x(flash, i * page_size);
	}
	printf("\n");
	protect_stm50flw0x0x(bios);

	return rc;
}

int write_stm50flw0x0x(struct flashchip *flash, uint8_t * buf)
{
	int i, rc = 0;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	printf("Programming page: \n");
	for (i = 0; (i < total_size / page_size) && (rc == 0); i++) {
		printf
		    ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("%04d at address: 0x%08x ", i, i * page_size);

		/* Auto Skip Blocks, which already contain the desired data
		 * Faster, because we only write, what has changed
		 * More secure, because blocks, which are excluded
		 * (with the exclude or layout feature)
		 * are not erased and rewritten; data is retained also
		 * in sudden power off situations
		 */
		if (!memcmp((void *)(buf + i * page_size),
			    (void *)(bios + i * page_size), page_size)) {
			printf("SKIPPED\n");
			continue;
		}

		rc = unlock_block_stm50flw0x0x(flash, i * page_size);
		if (!rc)
			rc = erase_block_stm50flw0x0x(flash, i * page_size);
		if (!rc)
			write_page_stm50flw0x0x(bios, buf + i * page_size,
					bios + i * page_size, page_size);
	}
	printf("\n");
	protect_stm50flw0x0x(bios);

	return rc;
}
