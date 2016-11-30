/*
 * This file is part of the coreboot project.
 * Copyright 2014 Google Inc.
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

#include <stddef.h>
#include <spi-generic.h>

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	return NULL;
}

int spi_claim_bus(const struct spi_slave *slave)
{
	return 0;
}

void spi_release_bus(const struct spi_slave *slave)
{
}

int spi_xfer(const struct spi_slave *slave, const void *dout,
	     size_t out_bytes, void *din, size_t in_bytes)
{
	return 0;
}
