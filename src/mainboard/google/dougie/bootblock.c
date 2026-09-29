/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/qcom_qup_se.h>
#include <soc/qupv3_spi_common.h>

void bootblock_mainboard_init(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		qup_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 1010 * KHz); /* EC SPI */
}
