/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include "board.h"
#include <soc/qupv3_spi_common.h>

void bootblock_mainboard_init(void)
{
	setup_chromeos_gpios();
	qup_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, 1010 * KHz);
	qup_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 1010 * KHz);
}
