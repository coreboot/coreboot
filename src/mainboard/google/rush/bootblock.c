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
 */

#include <arch/io.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <device/i2c.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/padconfig.h>
#include <soc/spi.h>	/* FIXME: move back to soc code? */

#include "pmic.h"

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

	/* Power Button */
	PAD_CFG_GPIO_INPUT(KB_COL0, PINMUX_PULL_NONE),

	/* Lid Open Switch */
	PAD_CFG_GPIO_INPUT(KB_ROW4, PINMUX_PULL_UP),
};

static const struct pad_config i2cpad[] = {
	/* PMIC i2C bus */
	PAD_CFG_SFIO(PWR_I2C_SCL, PINMUX_INPUT_ENABLE, I2CPMU),
	PAD_CFG_SFIO(PWR_I2C_SDA, PINMUX_INPUT_ENABLE, I2CPMU),
};

static const struct pad_config spipad[] = {
	/* SPI fLash: mosi, miso, clk, cs0  */
	PAD_CFG_SFIO(GPIO_PG6, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SPI4),
	PAD_CFG_SFIO(GPIO_PG7, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SPI4),
	PAD_CFG_SFIO(GPIO_PG5, PINMUX_INPUT_ENABLE, SPI4),
	PAD_CFG_SFIO(GPIO_PI3, PINMUX_INPUT_ENABLE, SPI4),
};

static const struct funit_cfg funitcfgs[] = {
	FUNIT_CFG(I2C5, PLLP, 400, i2cpad, ARRAY_SIZE(i2cpad)),
	FUNIT_CFG(SBC4, PLLP, 33333, spipad, ARRAY_SIZE(spipad)),
};

void bootblock_mainboard_early_init(void)
{
	soc_configure_pads(uart_console_pads, ARRAY_SIZE(uart_console_pads));
}

static void set_clock_sources(void)
{
	/* UARTA gets PLLP, deactivate CLK_UART_DIV_OVERRIDE */
	write32(CLK_RST_REG(clk_src_uarta), PLLP << CLK_SOURCE_SHIFT);
}

void bootblock_mainboard_init(void)
{
	set_clock_sources();

	/* Set up the pads required to load romstage. */
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));
	soc_configure_funits(funitcfgs, ARRAY_SIZE(funitcfgs));

	i2c_init(4);
	pmic_init(4);

	tegra_spi_init(4);
}
