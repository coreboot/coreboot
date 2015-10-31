/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <stdint.h>
#include <stddef.h>
#include <console/console.h>
#include <spi-generic.h>
#include <boot/coreboot_tables.h>

struct spi_flash {
	struct spi_slave *spi;

	const char	*name;

	u32		size;

	u32		sector_size;

	u8		erase_cmd;

	u8		status_cmd;

	/* All callbacks return 0 on success and != 0 on error. */
	int		(*read)(struct spi_flash *flash, u32 offset,
				size_t len, void *buf);
	int		(*write)(struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);
	int		(*erase)(struct spi_flash *flash, u32 offset,
				size_t len);
	int		(*status)(struct spi_flash *flash, u8 *reg);
};

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs);

void lb_spi_flash(struct lb_header *header);

#endif /* _SPI_FLASH_H_ */
