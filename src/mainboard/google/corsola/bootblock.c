/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <soc/spi.h>

#include "gpio.h"

void bootblock_mainboard_init(void)
{
	mtk_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, SPI_PAD0_MASK, 3 * MHz, 0);
	mtk_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, SPI_PAD0_MASK, 1 * MHz, 0);
	mtk_snfc_init(SPI_NOR_GPIO_SET1);
	setup_chromeos_gpios();
	gpio_eint_configure(GPIO_GSC_AP_INT_ODL, IRQ_TYPE_EDGE_RISING);
}
