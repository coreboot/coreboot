/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SOC_INTEL_COMMON_BLOCK_SPI_H
#define SOC_INTEL_COMMON_BLOCK_SPI_H

/*
 * SoC overrides
 *
 * All new SoC must implement below functionality.
 */

/* Function to convert input device function to bus number
 * Input: Device Function number
 * Output: -1 translate to Error, >=0 is bus number
 */
int spi_soc_devfn_to_bus(unsigned int devfn);

#endif	/* SOC_INTEL_COMMON_BLOCK_SPI_H */
