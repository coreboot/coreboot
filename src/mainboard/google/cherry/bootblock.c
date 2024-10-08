/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/i2c.h>
#include <soc/pcie.h>
#include <soc/spi.h>

#include "gpio.h"

static void nor_set_gpio_pinmux(void)
{
	const struct pad_func *ptr = NULL;

	/* GPIO 140 ~ 143 */
	struct pad_func nor_pinmux[] = {
		PAD_FUNC_UP(SPIM2_CSB, SPINOR_CS),
		PAD_FUNC_UP(SPIM2_CLK, SPINOR_CK),
		PAD_FUNC_UP(SPIM2_MO, SPINOR_IO0),
		PAD_FUNC_UP(SPIM2_MI, SPINOR_IO1),
	};

	ptr = nor_pinmux;
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++) {
		gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, ptr[i].select);
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
	}
}

static void usb3_hub_reset(void)
{
	gpio_output(GPIO(DGI_D7), 1);
}

void bootblock_mainboard_init(void)
{
	/*
	 * Initialize PCIe pinmux and assert PERST# early to reduce
	 * the impact of 100ms delay.
	 */
	if (CONFIG(PCI))
		mtk_pcie_pre_init();

	mtk_i2c_bus_init(CONFIG_DRIVER_TPM_I2C_BUS, I2C_SPEED_FAST);
	mtk_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, SPI_PAD0_MASK, 3 * MHz, 0);
	nor_set_gpio_pinmux();
	setup_chromeos_gpios();
	gpio_eint_configure(GPIO_GSC_AP_INT, IRQ_TYPE_EDGE_RISING);
	usb3_hub_reset();
}
