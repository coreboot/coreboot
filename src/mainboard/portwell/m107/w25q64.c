/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/spi.h>
#include <string.h>

/*
 * SPI VSCC configuration
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
