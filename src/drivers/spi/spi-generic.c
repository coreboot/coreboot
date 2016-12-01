/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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
 */

#include <spi-generic.h>
#include <string.h>

int spi_claim_bus(const struct spi_slave *slave)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	if (ctrlr && ctrlr->claim_bus)
		return ctrlr->claim_bus(slave);
	return 0;
}

void spi_release_bus(const struct spi_slave *slave)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	if (ctrlr && ctrlr->release_bus)
		ctrlr->release_bus(slave);
}

int spi_xfer(const struct spi_slave *slave, const void *dout, size_t bytesout,
	     void *din, size_t bytesin)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	if (ctrlr && ctrlr->xfer)
		return ctrlr->xfer(slave, dout, bytesout, din, bytesin);

	return -1;
}

void __attribute__((weak)) spi_init(void)
{
	/* Default weak implementation - do nothing. */
}

const struct spi_ctrlr_buses spi_ctrlr_bus_map[0] __attribute__((weak));
const size_t spi_ctrlr_bus_map_count __attribute__((weak));

int __attribute__((weak)) spi_setup_slave(unsigned int bus, unsigned int cs,
					  struct spi_slave *slave)
{
	size_t i;

	memset(slave, 0, sizeof(*slave));

	for (i = 0; i < spi_ctrlr_bus_map_count; i++) {
		if ((spi_ctrlr_bus_map[i].bus_start <= bus) &&
		    (spi_ctrlr_bus_map[i].bus_end >= bus)) {
			slave->ctrlr = spi_ctrlr_bus_map[i].ctrlr;
			break;
		}
	}

	if (slave->ctrlr == NULL)
		return -1;

	slave->bus = bus;
	slave->cs = cs;

	if (slave->ctrlr->setup)
		return slave->ctrlr->setup(slave);

	return 0;
}
