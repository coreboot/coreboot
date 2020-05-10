/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/mt8183.h>
#include <soc/spi.h>

#include "early_init.h"
#include "gpio.h"

#define BOOTBLOCK_EN_L (GPIO(KPROW0))
#define AP_IN_SLEEP_L (GPIO(SRCLKENA0))

void mainboard_early_init(void)
{
	mt8183_early_init();

	/* Turn on real eMMC and allow communication to EC. */
	gpio_output(BOOTBLOCK_EN_L, 1);

	setup_chromeos_gpios();

	gpio_set_mode(AP_IN_SLEEP_L, PAD_SRCLKENA0_FUNC_SRCLKENA0);

	mtk_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, SPI_PAD0_MASK, 1 * MHz, 0);
	gpio_eint_configure(CR50_IRQ, IRQ_TYPE_EDGE_RISING);
}
