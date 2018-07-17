/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
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

#ifndef __COREBOOT_SRC_SOC_CN81XX_INCLUDE_SOC_SPI_H
#define __COREBOOT_SRC_SOC_CN81XX_INCLUDE_SOC_SPI_H

/* This driver serves as a CBFS media source. */
#include <spi-generic.h>
#include <stddef.h>

void spi_enable(const size_t bus);
void spi_disable(const size_t bus);
void spi_set_cs(const size_t bus,
		const size_t chip_select,
		const size_t assert_is_low);
void spi_set_clock(const size_t bus,
		   const size_t speed_hz,
		   const size_t idle_low,
		   const size_t idle_cycles);
void spi_set_lsbmsb(const size_t bus, const size_t lsb_first);
void spi_init_custom(const size_t bus,
		     const size_t speed_hz,
		     const size_t idle_low,
		     const size_t idle_cycles,
		     const size_t lsb_first,
		     const size_t chip_select,
		     const size_t assert_is_low);
#endif  /* ! __COREBOOT_SRC_SOC_CN81XX_INCLUDE_SOC_SPI_H */
