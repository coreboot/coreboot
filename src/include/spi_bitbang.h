/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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
#ifndef _SPI_BITBANG_H_
#define _SPI_BITBANG_H_

#include <types.h>

struct spi_bitbang_ops {
	int (*get_miso)(const struct spi_bitbang_ops *ops);
	void (*set_mosi)(const struct spi_bitbang_ops *ops, int value);
	void (*set_clk)(const struct spi_bitbang_ops *ops, int value);
	void (*set_cs)(const struct spi_bitbang_ops *ops, int value);
};

int spi_bitbang_claim_bus(const struct spi_bitbang_ops *ops);
void spi_bitbang_release_bus(const struct spi_bitbang_ops *ops);
int spi_bitbang_xfer(const struct spi_bitbang_ops *ops, const void *dout,
		     size_t bytes_out, void *din, size_t bytes_in);

#endif /* _SPI_BITBANG_H_ */
