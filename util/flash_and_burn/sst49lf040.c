/* sst40lf020.c: driver for SST40LF040 flash models.
 *
 *
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *      4 MEgabit (512K x 8) SuperFlash EEPROM, SST49lF040 data sheet
 *
 * ToDo: Consilidated to standard JEDEC code.
 *
 * $Id$
 */
#include <stdio.h>
#include "flash.h"
#include "jedec.h"
#include "sst49lf040.h"

int write_49lf040(struct flashchip *flash, unsigned char *buf)
{
	int i;
	int total_size = flash->total_size * 1024, page_size =
	    flash->page_size;
	volatile char *bios = flash->virt_addr;

	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		/* erase the page before programming
		 * Chip erase only works in parallel programming mode for the 49lf040.
		 * Use sector-erase instead */
		erase_sector_jedec(bios, i * page_size);

		/* write to the sector */
		printf("%04d at address: 0x%08x ", i, i * page_size);
		write_sector_jedec(bios, buf + i * page_size,
				   bios + i * page_size, page_size);
		printf
		    ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		fflush(stdout);
	}
	printf("\n");

	return (0);
}
