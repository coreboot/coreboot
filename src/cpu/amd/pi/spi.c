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

#include "s3_resume.h"

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

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	flash->erase(flash, pos, size);
	flash->write(flash, pos, sizeof(len), &len);
	flash->write(flash, pos + sizeof(len), len, buf);

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_release_bus(flash->spi);

	return;
}
