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

/*
 * Datasheet:
 *  - Name: Intel 82802AB/82802AC Firmware Hub (FWH)
 *  - URL: http://www.intel.com/design/chipsets/datashts/290658.htm
 *  - PDF: http://download.intel.com/design/chipsets/datashts/29065804.pdf
 *  - Order number: 290658-004
 */

#include <stdio.h>
#include <string.h>
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
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0x90, bios + 0x5555);
#endif

	chip_writeb(0xff, bios);
	myusec_delay(10);
	chip_writeb(0x90, bios);
	myusec_delay(10);

	id1 = chip_readb(bios);
	id2 = chip_readb(bios + 0x01);

	/* Leave ID mode */
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0xF0, bios + 0x5555);

	myusec_delay(10);

	printf_debug("%s: id1 0x%02x, id2 0x%02x\n", __FUNCTION__, id1, id2);

	if (id1 != flash->manufacture_id || id2 != flash->model_id)
		return 0;

	map_flash_registers(flash);

	return 1;
}

uint8_t wait_82802ab(volatile uint8_t *bios)
{
	uint8_t status;
	uint8_t id1, id2;

	chip_writeb(0x70, bios);
	if ((chip_readb(bios) & 0x80) == 0) {	// it's busy
		while ((chip_readb(bios) & 0x80) == 0) ;
	}

	status = chip_readb(bios);

	// put another command to get out of status register mode

	chip_writeb(0x90, bios);
	myusec_delay(10);

	id1 = chip_readb(bios);
	id2 = chip_readb(bios + 0x01);

	// this is needed to jam it out of "read id" mode
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0xF0, bios + 0x5555);

	return status;
}

int erase_82802ab_block(struct flashchip *flash, int offset)
{
	volatile uint8_t *bios = flash->virtual_memory + offset;
	volatile uint8_t *wrprotect = flash->virtual_registers + offset + 2;
	int j;
	uint8_t status;

	// clear status register
	chip_writeb(0x50, bios);
	//printf("Erase at %p\n", bios);
	// clear write protect
	//printf("write protect is at %p\n", (wrprotect));
	//printf("write protect is 0x%x\n", *(wrprotect));
	chip_writeb(0, wrprotect);
	//printf("write protect is 0x%x\n", *(wrprotect));

	// now start it
	chip_writeb(0x20, bios);
	chip_writeb(0xd0, bios);
	myusec_delay(10);
	// now let's see what the register is
	status = wait_82802ab(flash->virtual_memory);
	//print_82802ab_status(status);
	for (j = 0; j < flash->page_size; j++) {
		if (chip_readb(bios + j) != 0xFF) {
			printf("BLOCK ERASE failed at 0x%x\n", offset);
			return -1;
		}
	}
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
		chip_writeb(0x40, dst);
		chip_writeb(*src++, dst++);
		wait_82802ab(bios);
	}
}

int write_82802ab(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	printf("Programming page: \n");
	for (i = 0; i < total_size / page_size; i++) {
		printf
		    ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("%04d at address: 0x%08x", i, i * page_size);

		/* Auto Skip Blocks, which already contain the desired data
		 * Faster, because we only write, what has changed
		 * More secure, because blocks, which are excluded
		 * (with the exclude or layout feature)
		 * or not erased and rewritten; their data is retained also in
		 * sudden power off situations
		 */
		if (!memcmp((void *)(buf + i * page_size),
			    (void *)(bios + i * page_size), page_size)) {
			printf("SKIPPED\n");
			continue;
		}

		/* erase block by block and write block by block; this is the most secure way */
		erase_82802ab_block(flash, i * page_size);
		write_page_82802ab(bios, buf + i * page_size,
				   bios + i * page_size, page_size);
	}
	printf("\n");
	protect_jedec(bios);

	return 0;
}
