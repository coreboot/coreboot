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

#include <device/device.h>
#include <boot/coreboot_tables.h>

#include <soc/clock.h>
#include <soc/nvidia/tegra132/clk_rst.h>
#include <soc/nvidia/tegra132/spi.h>
#include <soc/addressmap.h>
#include <soc/padconfig.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static const struct pad_config padcfgs[] = {
	/* MMC3(SDCARD) */
	PAD_CFG_SFIO(SDMMC3_CLK, PINMUX_INPUT_ENABLE, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_CMD, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT0, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT2, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT3, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_CLK_LB_IN, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_CLK_LB_OUT, PINMUX_INPUT_ENABLE | PINMUX_PULL_DOWN, SDMMC3),
	/* MMC3 Card Detect pin */
	PAD_CFG_GPIO_INPUT(SDMMC3_CD_N, PINMUX_PULL_UP),
	/* Disable SD card reader power so it can be reset even on warm boot.
	   Payloads must enable power before accessing SD card slots. */
	PAD_CFG_GPIO_OUT0(KB_ROW0, PINMUX_PULL_NONE),
	/* MMC4 (eMMC) */
	PAD_CFG_SFIO(SDMMC4_CLK, PINMUX_INPUT_ENABLE, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_CMD, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT0, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT2, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT3, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT4, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT5, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT6, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT7, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
};

static void configure_clocks(void)
{
	/* MMC */
	clock_enable_clear_reset(CLK_L_SDMMC4, 0, CLK_U_SDMMC3, 0, 0, 0);
	clock_configure_source(sdmmc3, PLLP, 48000);
	clock_configure_source(sdmmc4, PLLP, 48000);
}

static void setup_ec_spi(void)
{
	struct tegra_spi_channel *spi;

	spi = tegra_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);
}

static void mainboard_init(device_t dev)
{
	configure_clocks();

	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));

	setup_ec_spi();
}

static void mainboard_enable(device_t dev)
{
        dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
        .name   = "rush",
        .enable_dev = mainboard_enable,
};
