/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include "board.h"
#include <soc/qupv3_i2c_common.h>
#include <soc/qcom_qup_se.h>
#include <soc/qupv3_spi_common.h>

void bootblock_mainboard_init(void)
{
	setup_chromeos_gpios();

	if (CONFIG(I2C_TPM))
		i2c_init(CONFIG_DRIVER_TPM_I2C_BUS, I2C_SPEED_FAST); /* H1/TPM I2C */

	if (CONFIG(SPI_TPM))
		qup_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, 1010 * KHz); /* H1/TPM SPI */

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		qup_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 1010 * KHz); /* EC SPI */
}
