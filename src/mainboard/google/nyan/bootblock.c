/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/pinmux.h>
#include <soc/spi.h>	/* FIXME: move back to soc code? */

#include "pmic.h"

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void set_clock_sources(void)
{
	/* UARTA gets PLLP, deactivate CLK_UART_DIV_OVERRIDE */
	write32(&clk_rst->clk_src_uarta, PLLP << CLK_SOURCE_SHIFT);

	clock_configure_source(mselect, PLLP, 102000);

	/* The PMIC is on I2C5 and can run at 400 KHz. */
	clock_configure_i2c_scl_freq(i2c5, PLLP, 400);

	/* TODO: We should be able to set this to 50MHz, but that did not seem
	 * reliable. */
	clock_configure_source(sbc4, PLLP, 33333);
}

void bootblock_mainboard_init(void)
{
	set_clock_sources();

	clock_enable_clear_reset(CLK_L_CACHE2 | CLK_L_TMR,
				 CLK_H_I2C5 | CLK_H_APBDMA,
				 0, CLK_V_MSELECT, 0, 0);

	// Board ID GPIOs, bits 0-3.
	gpio_input(GPIO(Q3));
	gpio_input(GPIO(T1));
	gpio_input(GPIO(X1));
	gpio_input(GPIO(X4));

	// I2C5 (PMU) clock.
	pinmux_set_config(PINMUX_PWR_I2C_SCL_INDEX,
			  PINMUX_PWR_I2C_SCL_FUNC_I2CPMU | PINMUX_INPUT_ENABLE);
	// I2C5 (PMU) data.
	pinmux_set_config(PINMUX_PWR_I2C_SDA_INDEX,
			  PINMUX_PWR_I2C_SDA_FUNC_I2CPMU | PINMUX_INPUT_ENABLE);
	i2c_init(4);
	pmic_init(4);

	/* SPI4 data out (MOSI) */
	pinmux_set_config(PINMUX_GPIO_PG6_INDEX,
			  PINMUX_GPIO_PG6_FUNC_SPI4 | PINMUX_INPUT_ENABLE |
			  PINMUX_PULL_UP);
	/* SPI4 data in (MISO) */
	pinmux_set_config(PINMUX_GPIO_PG7_INDEX,
			  PINMUX_GPIO_PG7_FUNC_SPI4 | PINMUX_INPUT_ENABLE |
			  PINMUX_PULL_UP);
	/* SPI4 clock */
	pinmux_set_config(PINMUX_GPIO_PG5_INDEX,
			  PINMUX_GPIO_PG5_FUNC_SPI4 | PINMUX_INPUT_ENABLE);
	/* SPI4 chip select 0 */
	pinmux_set_config(PINMUX_GPIO_PI3_INDEX,
			  PINMUX_GPIO_PI3_FUNC_SPI4 | PINMUX_INPUT_ENABLE);

	tegra_spi_init(4);
}
