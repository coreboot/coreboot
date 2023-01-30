/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/spi.h>

#include "gpio.h"

static void usb3_hub_reset(void)
{
	gpio_output(GPIO_USB3_HUB_RST_L, 1);
}

void bootblock_mainboard_init(void)
{
	mtk_i2c_bus_init(CONFIG_DRIVER_TPM_I2C_BUS, I2C_SPEED_FAST);
	mtk_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, SPI_PAD0_MASK, 3 * MHz, 0);
	mtk_snfc_init();
	setup_chromeos_gpios();
	gpio_eint_configure(GPIO_GSC_AP_INT_ODL, IRQ_TYPE_EDGE_RISING);
	usb3_hub_reset();
}
