/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/spi.h>

void bootblock_mainboard_init(void)
{
	mtk_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, SPI_PAD0_MASK, 1 * MHz, 0);
	mtk_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, SPI_PAD0_MASK, 1 * MHz, 0);
}
