/*
 * This file is part of the coreboot project.
 *
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

#include <soc/spi.h>
#include <string.h>

/*
 * SPI VSCC configuration W25Q64FW.
 */
#define SPI_VSCC (WG_64_BYTE | EO(0x20) | BES_4_KB)

static const struct vscc_config spi_config = {
	.lvscc =  SPI_VSCC,
	.uvscc =  SPI_VSCC,
};

int mainboard_get_spi_vscc_config(struct vscc_config *cfg)
{
	memcpy(cfg, &spi_config, sizeof(*cfg));

	return 0;
}
