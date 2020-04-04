/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
uint64_t spi_get_clock(const size_t bus);
void spi_set_lsbmsb(const size_t bus, const size_t lsb_first);
void spi_init_custom(const size_t bus,
		     const size_t speed_hz,
		     const size_t idle_low,
		     const size_t idle_cycles,
		     const size_t lsb_first,
		     const size_t chip_select,
		     const size_t assert_is_low);
#endif  /* ! __COREBOOT_SRC_SOC_CN81XX_INCLUDE_SOC_SPI_H */
