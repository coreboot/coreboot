/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stddef.h>
#include <console/console.h>
#include <string.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <baytrail/nvm.h>

/* This module assumes the flash is memory mapped just below 4GiB in the
 * address space for reading. Also this module assumes an area it erased
 * when all bytes read as all 0xff's. */

static struct spi_flash *flash;

static int nvm_init(void)
{
	if (flash != NULL)
		return 0;

	spi_init();
	flash = spi_flash_probe(0, 0, 1000000, SPI_MODE_3);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return -1;
	}

	return 0;
}

/* Convert memory mapped pointer to flash offset. */
static inline uint32_t to_flash_offset(void *p)
{
	return CONFIG_ROM_SIZE + (uintptr_t)p;
}

int nvm_is_erased(const void *start, size_t size)
{
	const char *cur = start;

	while (size > 0) {
		if (*cur != 0xff)
			return 0;
		cur++;
		size--;
	}
	return 1;
}

int nvm_erase(void *start, size_t size)
{
	if (nvm_init() < 0)
		return -1;
	flash->erase(flash, to_flash_offset(start), size);
	return 0;
}

/* Write data to NVM. Returns 0 on success < 0 on error.  */
int nvm_write(void *start, const void *data, size_t size)
{
	if (nvm_init() < 0)
		return -1;
	flash->write(flash, to_flash_offset(start), size, data);
	return 0;
}
