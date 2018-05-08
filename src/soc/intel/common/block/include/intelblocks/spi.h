/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
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
