/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

int probe_winbond_fwhub(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t vid, did;

	/* Product Identification Entry */
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0x90, bios + 0x5555);
	myusec_delay(10);

	/* Read product ID */
	vid = chip_readb(bios);
	did = chip_readb(bios + 0x01);

	/* Product Identifixation Exit */
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0xF0, bios + 0x5555);
	myusec_delay(10);

	printf_debug("%s: vid 0x%x, did 0x%x\n", __FUNCTION__, vid, did);

	if (vid != flash->manufacture_id || did != flash->model_id)
		return 0;

	map_flash_registers(flash);

	return 1;
}

static int unlock_block_winbond_fwhub(struct flashchip *flash, int offset)
{
	volatile uint8_t *wrprotect = flash->virtual_registers + offset + 2;
	uint8_t locking;

	printf_debug("Trying to unlock block @0x%08x = 0x%02x\n", offset,
		     chip_readb(wrprotect));

	locking = chip_readb(wrprotect);
	switch (locking & 0x7) {
	case 0:
		printf_debug("Full Access.\n");
		return 0;
	case 1:
		printf_debug("Write Lock (Default State).\n");
		chip_writeb(0, wrprotect);
		return 0;
	case 2:
		printf_debug("Locked Open (Full Access, Lock Down).\n");
		return 0;
	case 3:
		fprintf(stderr, "Error: Write Lock, Locked Down.\n");
		return -1;
	case 4:
		printf_debug("Read Lock.\n");
		chip_writeb(0, wrprotect);
		return 0;
	case 5:
		printf_debug("Read/Write Lock.\n");
		chip_writeb(0, wrprotect);
		return 0;
	case 6:
		fprintf(stderr, "Error: Read Lock, Locked Down.\n");
		return -1;
	case 7:
		fprintf(stderr, "Error: Read/Write Lock, Locked Down.\n");
		return -1;
	}

	/* We will never reach this point, but GCC doesn't know */
	return -1;
}

int unlock_winbond_fwhub(struct flashchip *flash)
{
	int i, total_size = flash->total_size * 1024;
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t locking;

	/* Are there any hardware restrictions that we can't overcome? 
	 * If flashrom fail here, someone's got to check all those GPIOs.
	 */

	/* Product Identification Entry */
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0x90, bios + 0x5555);
	myusec_delay(10);

	/* Read Hardware Lock Bits */
	locking = chip_readb(bios + 0xffff2);

	/* Product Identification Exit */
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0xF0, bios + 0x5555);
	myusec_delay(10);

	printf_debug("Lockout bits:\n");

	if (locking & (1 << 2))
		fprintf(stderr, "Error: hardware bootblock locking (#TBL).\n");
	else
		printf_debug("No hardware bootblock locking (good!)\n");

	if (locking & (1 << 3))
		fprintf(stderr, "Error: hardware block locking (#WP).\n");
	else
		printf_debug("No hardware block locking (good!)\n");

	if (locking & ((1 << 2) | (1 << 3)))
		return -1;

	/* Unlock the complete chip */
	for (i = 0; i < total_size; i += flash->page_size)
		if (unlock_block_winbond_fwhub(flash, i))
			return -1;

	return 0;
}

static int erase_sector_winbond_fwhub(volatile uint8_t *bios,
				      unsigned int sector)
{
	/* Remember: too much sleep can waste your day. */

	printf("0x%08x\b\b\b\b\b\b\b\b\b\b", sector);

	/* Sector Erase */
	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0x80, bios + 0x5555);

	chip_writeb(0xAA, bios + 0x5555);
	chip_writeb(0x55, bios + 0x2AAA);
	chip_writeb(0x30, bios + sector);

	/* wait for Toggle bit ready */
	toggle_ready_jedec(bios);

	return 0;
}

int erase_winbond_fwhub(struct flashchip *flash)
{
	int i, total_size = flash->total_size * 1024;
	volatile uint8_t *bios = flash->virtual_memory;

	unlock_winbond_fwhub(flash);

	printf("Erasing:     ");

	for (i = 0; i < total_size; i += flash->page_size)
		erase_sector_winbond_fwhub(bios, i);

	printf("\n");

	for (i = 0; i < total_size; i++) {
		if (bios[i] != 0xff) {
			fprintf(stderr, "Error: Flash chip erase failed at 0x%08x(0x%02x)\n", i, bios[i]);
			return -1;
		}
	}

	return 0;
}

int write_winbond_fwhub(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	volatile uint8_t *bios = flash->virtual_memory;

	if (erase_winbond_fwhub(flash))
		return -1;

	printf("Programming: ");
	for (i = 0; i < total_size; i += flash->page_size) {
		printf("0x%08x\b\b\b\b\b\b\b\b\b\b", i);
		write_sector_jedec(bios, buf + i, bios + i, flash->page_size);
	}
	printf("\n");

	return 0;
}
