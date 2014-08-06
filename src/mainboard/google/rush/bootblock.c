/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/i2c.h>
#include <soc/addressmap.h>
#include <soc/bootblock.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra132/clk_rst.h>
#include <soc/nvidia/tegra132/spi.h>	/* FIXME: move back to soc code? */

#include "pmic.h"

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static const struct pad_config uart_console_pads[] = {
	/* UARTA: tx and rx. */
	PAD_CFG_SFIO(KB_ROW9, PINMUX_PULL_NONE, UA3),
	PAD_CFG_SFIO(KB_ROW10, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, UA3),
	/*
	 * Disable UART2 pads as they are default connected to UARTA controller.
	 */
	PAD_CFG_UNUSED(UART2_RXD),
	PAD_CFG_UNUSED(UART2_TXD),
	PAD_CFG_UNUSED(UART2_RTS_N),
	PAD_CFG_UNUSED(UART2_CTS_N),
};

static const struct pad_config padcfgs[] = {
	/* Board ID bits 3:0 */
	PAD_CFG_GPIO_INPUT(GPIO_X4_AUD, PINMUX_PULL_NONE),
	PAD_CFG_GPIO_INPUT(GPIO_X1_AUD, PINMUX_PULL_NONE),
	PAD_CFG_GPIO_INPUT(KB_ROW17, PINMUX_PULL_NONE),
	PAD_CFG_GPIO_INPUT(KB_COL3, PINMUX_PULL_NONE),
	/* PMIC i2C bus */
	PAD_CFG_SFIO(PWR_I2C_SCL, PINMUX_INPUT_ENABLE, I2CPMU),
	PAD_CFG_SFIO(PWR_I2C_SDA, PINMUX_INPUT_ENABLE, I2CPMU),
	/* SPI fLash: mosi, miso, clk, cs0  */
	PAD_CFG_SFIO(GPIO_PG6, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SPI4),
	PAD_CFG_SFIO(GPIO_PG7, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SPI4),
	PAD_CFG_SFIO(GPIO_PG5, PINMUX_INPUT_ENABLE, SPI4),
	PAD_CFG_SFIO(GPIO_PI3, PINMUX_INPUT_ENABLE, SPI4),
};

void bootblock_mainboard_early_init(void)
{
	soc_configure_pads(uart_console_pads, ARRAY_SIZE(uart_console_pads));
}

static void set_clock_sources(void)
{
	/* UARTA gets PLLP, deactivate CLK_UART_DIV_OVERRIDE */
	writel(PLLP << CLK_SOURCE_SHIFT, &clk_rst->clk_src_uarta);

	/* The PMIC is on I2C5 and can run at 400 KHz. */
	clock_configure_i2c_scl_freq(i2c5, PLLP, 400);

	/* TODO: We should be able to set this to 50MHz, but that did not seem
	 * reliable. */
	clock_configure_source(sbc4, PLLP, 33333);
}

void bootblock_mainboard_init(void)
{
	set_clock_sources();

	/* Enable PMIC I2C controller. */
	clock_enable_clear_reset(0, CLK_H_I2C5, 0, 0, 0, 0);

	/* Set up the pads required to load romstage. */
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));

	i2c_init(4);
	pmic_init(4);

	tegra_spi_init(4);
}
