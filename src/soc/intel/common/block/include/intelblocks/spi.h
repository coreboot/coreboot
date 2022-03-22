/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_SPI_H
#define SOC_INTEL_COMMON_BLOCK_SPI_H

#include <types.h>

/*
 * Check if write protection for CSE RO is enabled or not.
 * Returns true if write protection for CSE RO is enabled, false otherwise.
 */
bool is_spi_wp_cse_ro_en(void);

/* Gets CSE RO's write protected area's base address and limit */
void spi_get_wp_cse_ro_range(uint32_t *base, uint32_t *limit);

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
