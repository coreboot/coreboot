/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <bootblock_common.h>
#include <gpio.h>
#include <device/mmio.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/mt6391.h>
#include <soc/pericfg.h>
#include <soc/spi.h>

#include "gpio.h"

static void i2c_set_gpio_pinmux(void)
{
	gpio_set_mode(GPIO(SDA1), PAD_SDA1_FUNC_SDA1);
	gpio_set_mode(GPIO(SCL1), PAD_SCL1_FUNC_SCL1);
	gpio_set_mode(GPIO(SDA4), PAD_SDA4_FUNC_SDA4);
	gpio_set_mode(GPIO(SCL4), PAD_SCL4_FUNC_SCL4);
}

static void nor_set_gpio_pinmux(void)
{
	/* Set driving strength of EINT4~EINT9 to 8mA
	 * 0: 2mA
	 * 1: 4mA
	 * 2: 8mA
	 * 3: 16mA
	 */
	/* EINT4: 0x10005B20[14:13] */
	clrsetbits16(&mtk_gpio->drv_mode[2].val, 0xf << 12, 2 << 13);
	/* EINT5~EINT9: 0x10005B30[2:1] */
	clrsetbits16(&mtk_gpio->drv_mode[3].val, 0xf << 0, 2 << 1);

	gpio_set_pull(GPIO(EINT4), GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_pull(GPIO(EINT5), GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_pull(GPIO(EINT6), GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_pull(GPIO(EINT7), GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_pull(GPIO(EINT8), GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_pull(GPIO(EINT9), GPIO_PULL_ENABLE, GPIO_PULL_UP);

	gpio_set_mode(GPIO(EINT4), PAD_EINT4_FUNC_SFWP_B);
	gpio_set_mode(GPIO(EINT5), PAD_EINT5_FUNC_SFOUT);
	gpio_set_mode(GPIO(EINT6), PAD_EINT6_FUNC_SFCS0);
	gpio_set_mode(GPIO(EINT7), PAD_EINT7_FUNC_SFHOLD);
	gpio_set_mode(GPIO(EINT8), PAD_EINT8_FUNC_SFIN);
	gpio_set_mode(GPIO(EINT9), PAD_EINT9_FUNC_SFCK);
}

void bootblock_mainboard_early_init(void)
{
	/* Clear UART0 power down signal */
	clrbits32(&mt8173_pericfg->pdn0_set, PERICFG_UART0_PDN);
}

void bootblock_mainboard_init(void)
{
	/* adjust gpio params when external voltage is 1.8V */
	gpio_init(GPIO_EINT_1P8V);

	/* set i2c related gpio */
	i2c_set_gpio_pinmux();

	/* set nor related GPIO */
	nor_set_gpio_pinmux();

	/* SPI_LEVEL_ENABLE: Enable 1.8V to 3.3V level shifter for EC SPI bus */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT > 4 &&
	    board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 8)
		gpio_output(GPIO(SRCLKENAI2), 1);

	/* Init i2c bus 2 Timing register for TPM */
	mtk_i2c_bus_init(CONFIG_DRIVER_TPM_I2C_BUS);

	mtk_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, SPI_PAD1_MASK, 6*MHz,
		     0);

	setup_chromeos_gpios();

	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 4)
		mt6391_enable_reset_when_ap_resets();
}
