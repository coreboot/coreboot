/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
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

#include <console/console.h>
#include <spi-generic.h>
#include "ec.h"
#include "ec_commands.h"

#define CROSEC_SPI_SPEED	(500000)

static int crosec_spi_io(uint8_t *write_bytes, size_t write_size,
			 uint8_t *read_bytes, size_t read_size,
			 void *context)
{
	struct spi_slave *slave = (struct spi_slave *)context;
	int rv;

	spi_claim_bus(slave);
	rv = spi_xfer(slave, write_bytes, write_size * 8, read_bytes,
		      read_size * 8);
	spi_release_bus(slave);

	if (rv != 0) {
		printk(BIOS_ERR, "%s: Cannot complete SPI I/O\n", __func__);
		return -1;
	}

	return 0;
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	static struct spi_slave *slave = NULL;
	if (!slave) {
		slave = spi_setup_slave(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS,
					CONFIG_EC_GOOGLE_CHROMEEC_SPI_CHIP,
					CROSEC_SPI_SPEED,
					SPI_READ_FLAG | SPI_WRITE_FLAG);
	}
	return crosec_command_proto(cec_command, crosec_spi_io, slave);
}

#ifndef __PRE_RAM__
u8 google_chromeec_get_event(void)
{
	printk(BIOS_ERR, "%s: Not supported.\n", __func__);
	return 0;
}
#endif
