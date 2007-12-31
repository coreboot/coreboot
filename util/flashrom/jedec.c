/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 * Copyright (C) 2006 Giampiero Giancipoli <gianci@email.it>
 * Copyright (C) 2006 coresystems GmbH <info@coresystems.de>
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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

#define MAX_REFLASH_TRIES 0x10

void toggle_ready_jedec(volatile uint8_t *dst)
{
	unsigned int i = 0;
	uint8_t tmp1, tmp2;

	tmp1 = *dst & 0x40;

	while (i++ < 0xFFFFFFF) {
		tmp2 = *dst & 0x40;
		if (tmp1 == tmp2) {
			break;
		}
		tmp1 = tmp2;
	}
}

void data_polling_jedec(volatile uint8_t *dst, uint8_t data)
{
	unsigned int i = 0;
	uint8_t tmp;

	data &= 0x80;

	while (i++ < 0xFFFFFFF) {
		tmp = *dst & 0x80;
		if (tmp == data) {
			break;
		}
	}
}

void unprotect_jedec(volatile uint8_t *bios)
{
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x80;
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x20;

	usleep(200);
}

void protect_jedec(volatile uint8_t *bios)
{
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xA0;

	usleep(200);
}

int probe_jedec(struct flashchip *flash)
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
	/* Older chips may need up to 100 us to respond. The ATMEL 29C020
	 * needs 10 ms according to the data sheet, but it has been tested
	 * to work reliably with 20 us. Allow a factor of 2 safety margin.
	 */
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

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, largeid1, largeid2);
	if (largeid1 == flash->manufacture_id && largeid2 == flash->model_id)
		return 1;

	return 0;
}

int erase_sector_jedec(volatile uint8_t *bios, unsigned int page)
{
	/*  Issue the Sector Erase command   */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0x80;
	myusec_delay(10);

	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + page) = 0x30;
	myusec_delay(10);

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios);

	return 0;
}

int erase_block_jedec(volatile uint8_t *bios, unsigned int block)
{
	/*  Issue the Sector Erase command   */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0x80;
	myusec_delay(10);

	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + block) = 0x50;
	myusec_delay(10);

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios);

	return 0;
}

int erase_chip_jedec(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;

	/*  Issue the JEDEC Chip Erase command   */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0x80;
	myusec_delay(10);

	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	myusec_delay(10);
	*(volatile uint8_t *)(bios + 0x5555) = 0x10;
	myusec_delay(10);

	toggle_ready_jedec(bios);

	return 0;
}

int write_page_write_jedec(volatile uint8_t *bios, uint8_t *src,
			   volatile uint8_t *dst, int page_size)
{
	int i, tried = 0, start_index = 0, ok;
	volatile uint8_t *d = dst;
	uint8_t *s = src;

retry:
	/* Issue JEDEC Data Unprotect comand */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xA0;

	/* transfer data from source to destination */
	for (i = start_index; i < page_size; i++) {
		/* If the data is 0xFF, don't program it */
		if (*src != 0xFF)
			*dst = *src;
		dst++;
		src++;
	}

	toggle_ready_jedec(dst - 1);

	dst = d;
	src = s;
	ok = 1;
	for (i = 0; i < page_size; i++) {
		if (*dst != *src) {
			ok = 0;
			break;
		}
		dst++;
		src++;
	}

	if (!ok && tried++ < MAX_REFLASH_TRIES) {
		start_index = i;
		goto retry;
	}
	if (!ok) {
		fprintf(stderr, " page %d failed!\n",
			(unsigned int)(d - bios) / page_size);
	}
	return !ok;
}

int write_byte_program_jedec(volatile uint8_t *bios, uint8_t *src,
			     volatile uint8_t *dst)
{
	int tried = 0, ok = 1;

	/* If the data is 0xFF, don't program it */
	if (*src == 0xFF) {
		return -1;
	}

retry:
	/* Issue JEDEC Byte Program command */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xA0;

	/* transfer data from source to destination */
	*dst = *src;
	toggle_ready_jedec(bios);

	if (*dst != *src && tried++ < MAX_REFLASH_TRIES) {
		goto retry;
	}

	if (tried >= MAX_REFLASH_TRIES)
		ok = 0;

	return !ok;
}

int write_sector_jedec(volatile uint8_t *bios, uint8_t *src,
		       volatile uint8_t *dst, unsigned int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		write_byte_program_jedec(bios, src, dst);
		dst++, src++;
	}

	return 0;
}

int write_jedec(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	erase_chip_jedec(flash);
	// dumb check if erase was successful.
	for (i = 0; i < total_size; i++) {
		if (bios[i] != (uint8_t) 0xff) {
			printf("ERASE FAILED @%d, val %02x!\n", i, bios[i]);
			return -1;
		}
	}

	printf("Programming page: ");
	for (i = 0; i < total_size / page_size; i++) {
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_page_write_jedec(bios, buf + i * page_size,
				       bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");
	protect_jedec(bios);

	return 0;
}
