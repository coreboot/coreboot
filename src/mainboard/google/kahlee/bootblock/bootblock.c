/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <soc/gpio.h>
#include <soc/southbridge.h>
#include <variant/ec.h>
#include <variant/gpio.h>

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	/* Enable the EC as soon as we have visibility */
	mainboard_ec_init();

	gpios = variant_early_gpio_table(&num_gpios);
	sb_program_gpios(gpios, num_gpios);
}

void bootblock_mainboard_init(void)
{
	if (IS_ENABLED(CONFIG_EM100)) {
		/*
		 * We should be able to rely on defaults, but it seems safer
		 * to explicitly set up these registers.
		 */
		sb_read_mode(SPI_READ_MODE_NOM);
		sb_set_spi100(SPI_SPEED_16M,		/* Normal */
				SPI_SPEED_16M,		/* Fast   */
				SPI_SPEED_16M,		/* AltIO  */
				SPI_SPEED_66M);		/* TPM    */
	} else {
		/*
		* W25Q128FW Setup
		* Normal Read 40MHz
		* Fast Read 104MHz
		* Dual Read IO (1-2-2)
		*/
		sb_read_mode(SPI_READ_MODE_DUAL122);

		/* Set SPI speeds before verstage. Needed for TPM */
		sb_set_spi100(SPI_SPEED_33M,		/* Normal */
				SPI_SPEED_66M,		/* Fast   */
				SPI_SPEED_66M,		/* AltIO  */
				SPI_SPEED_66M);		/* TPM    */
	}

	/* Setup TPM decode before verstage */
	sb_tpm_decode_spi();
}
