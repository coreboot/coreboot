/*
 * 82802ab.c: driver for programming JEDEC standard flash parts
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
 * Reference: http://www.intel.com/design/chipsets/datashts/290658.htm
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "flash.h"

// I need that Berkeley bit-map printer
void print_82802ab_status(uint8_t status)
{
	printf("%s", status & 0x80 ? "Ready:" : "Busy:");
	printf("%s", status & 0x40 ? "BE SUSPEND:" : "BE RUN/FINISH:");
	printf("%s", status & 0x20 ? "BE ERROR:" : "BE OK:");
	printf("%s", status & 0x10 ? "PROG ERR:" : "PROG OK:");
	printf("%s", status & 0x8 ? "VP ERR:" : "VPP OK:");
	printf("%s", status & 0x4 ? "PROG SUSPEND:" : "PROG RUN/FINISH:");
	printf("%s", status & 0x2 ? "WP|TBL#|WP#,ABORT:" : "UNLOCK:");
}

int probe_82802ab(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;

#if 0
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x90;
#endif

	*bios = 0xff;
	myusec_delay(10);
	*bios = 0x90;
	myusec_delay(10);

	id1 = *(volatile uint8_t *)bios;
	id2 = *(volatile uint8_t *)(bios + 0x01);

	/* Leave ID mode */
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xF0;

	myusec_delay(10);

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);

	if (id1 != flash->manufacture_id || id2 != flash->model_id)
		return 0;

	map_flash_registers(flash);

	return 1;
}

uint8_t wait_82802ab(volatile uint8_t *bios)
{
	uint8_t status;
	uint8_t id1, id2;

	*bios = 0x70;
	if ((*bios & 0x80) == 0) {	// it's busy
		while ((*bios & 0x80) == 0) ;
	}

	status = *bios;

	// put another command to get out of status register mode

	*bios = 0x90;
	myusec_delay(10);

	id1 = *(volatile uint8_t *)bios;
	id2 = *(volatile uint8_t *)(bios + 0x01);

	// this is needed to jam it out of "read id" mode
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xF0;

	return status;
}

int erase_82802ab_block(struct flashchip *flash, int offset)
{
	volatile uint8_t *bios = flash->virtual_memory + offset;
	volatile uint8_t *wrprotect = flash->virtual_registers + offset + 2;
	uint8_t status;

	// clear status register
	*bios = 0x50;
	//printf("Erase at %p\n", bios);
	// clear write protect
	//printf("write protect is at %p\n", (wrprotect));
	//printf("write protect is 0x%x\n", *(wrprotect));
	*(wrprotect) = 0;
	//printf("write protect is 0x%x\n", *(wrprotect));

	// now start it
	*(volatile uint8_t *)(bios) = 0x20;
	*(volatile uint8_t *)(bios) = 0xd0;
	myusec_delay(10);
	// now let's see what the register is
	status = wait_82802ab(flash->virtual_memory);
	//print_82802ab_status(status);
	printf("DONE BLOCK 0x%x\n", offset);

	return 0;
}

int erase_82802ab(struct flashchip *flash)
{
	int i;
	unsigned int total_size = flash->total_size * 1024;

	printf("total_size is %d; flash->page_size is %d\n",
	       total_size, flash->page_size);
	for (i = 0; i < total_size; i += flash->page_size)
		erase_82802ab_block(flash, i);
	printf("DONE ERASE\n");

	return 0;
}

void write_page_82802ab(volatile uint8_t *bios, uint8_t *src,
			volatile uint8_t *dst, int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		/* transfer data from source to destination */
		*dst = 0x40;
		*dst++ = *src++;
		wait_82802ab(bios);
	}
}

int write_82802ab(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	erase_82802ab(flash);
	if (*bios != 0xff) {
		printf("ERASE FAILED\n");
		return -1;
	}
	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_page_82802ab(bios, buf + i * page_size,
				   bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");
	protect_jedec(bios);

	return 0;
}
