/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_BLOCK_CHIP_H__
#define __AMD_BLOCK_CHIP_H__

#include <amdblocks/espi.h>
#include <amdblocks/spi.h>

struct soc_amd_common_config {
	/*
	 * SPI configuration
	 * Default values if not overridden by mainboard:
	 * Read mode - Normal 33MHz
	 * Normal speed - 66MHz
	 * Fast speed - 66MHz
	 * Alt speed - 66MHz
	 * TPM speed - 66MHz
	 */
	struct spi_config spi_config;

	/* eSPI configuration */
	struct espi_config espi_config;
};

/*
 * SoC callback that returns pointer to soc_amd_common_config structure embedded within the chip
 * soc config.
 */
const struct soc_amd_common_config *soc_get_common_config(void);

#endif /* __AMD_BLOCK_CHIP_H__ */
