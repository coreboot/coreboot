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

#include <stdio.h>
#include <stdint.h>
#include "flash.h"
#include "jedec.h"
#include "debug.h"

int probe_jedec(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virt_addr;
	uint8_t id1, id2;

	/* Issue JEDEC Product ID Entry command */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x5555) = 0x90;
	myusec_delay(10);

	/* Read product ID */
	id1 = *(volatile uint8_t *) bios;
	id2 = *(volatile uint8_t *) (bios + 0x01);

	/* Issue JEDEC Product ID Exit command */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x5555) = 0xF0;
	myusec_delay(10);

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);
	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

int erase_sector_jedec(volatile uint8_t *bios, unsigned int page)
{
	/*  Issue the Sector Erase command   */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x5555) = 0x80;
	myusec_delay(10);

	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + page)   = 0x30;
	myusec_delay(10);

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios);

	return (0);
}

int erase_block_jedec(volatile uint8_t *bios, unsigned int block)
{
	/*  Issue the Sector Erase command   */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x5555) = 0x80;
	myusec_delay(10);

	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + block)  = 0x50;
	myusec_delay(10);

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios);

	return (0);
}

int erase_chip_jedec(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virt_addr;

	/*  Issue the JEDEC Chip Erase command   */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x5555) = 0x80;
	myusec_delay(10);

	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *) (bios + 0x5555) = 0x10;
	myusec_delay(10);

	toggle_ready_jedec(bios);

	return (0);
}

int write_page_write_jedec(volatile uint8_t *bios, uint8_t *src,
			   volatile uint8_t *dst, int page_size)
{
	int i;

	/* Issue JEDEC Data Unprotect comand */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *) (bios + 0x5555) = 0xA0;

	/* transfer data from source to destination */
	for (i = 0; i < page_size; i++) {
		/* If the data is 0xFF, don't program it */
		if (*src == 0xFF)
			continue;
		*dst++ = *src++;
	}

	toggle_ready_jedec(dst - 1);

	return 0;
}

int write_byte_program_jedec(volatile uint8_t *bios, uint8_t *src,
			     volatile uint8_t *dst)
{
	int tried = 0;

	/* If the data is 0xFF, don't program it */
	if (*src == 0xFF) {
		return 0;
	}

retry:
	/* Issue JEDEC Byte Program command */
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *) (bios + 0x5555) = 0xA0;

	/* transfer data from source to destination */
	*dst = *src;
	toggle_ready_jedec(bios);

	if (*dst != *src && tried++ < 0x10) {
 		goto retry;
 	}

	return 0;
}

int write_sector_jedec(volatile uint8_t *bios, uint8_t *src,
		       volatile uint8_t *dst, unsigned int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		write_byte_program_jedec(bios, src, dst);
		dst++, src++;
	}

	return (0);
}

int write_jedec(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virt_addr;

	erase_chip_jedec(flash);
	if (*bios != (uint8_t) 0xff) {
		printf("ERASE FAILED\n");
		return -1;
	}
	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_page_write_jedec(bios, buf + i * page_size,
				       bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");
	protect_jedec(bios);

	return (0);
}
