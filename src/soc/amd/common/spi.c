/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <spi-generic.h>
#include <spi_flash.h>

void spi_SaveS3info(u32 pos, u32 size, u8 *buf, u32 len)
{
	struct spi_flash *flash;

	spi_init();
	flash = spi_flash_probe(0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		/* Dont make flow stop. */
		return;
	}

	spi_flash_volatile_group_begin(flash);

	spi_flash_erase(flash, pos, size);
	spi_flash_write(flash, pos, sizeof(len), &len);
	spi_flash_write(flash, pos + sizeof(len), len, buf);

	spi_flash_volatile_group_end(flash);

	return;
}
