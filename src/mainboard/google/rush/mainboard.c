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
#include <soc/funitcfg.h>

static const struct pad_config sdmmc3_pad[] = {
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
};

static const struct pad_config sdmmc4_pad[] = {
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

static const struct funit_cfg funitcfgs[] = {
	FUNIT_CFG(SDMMC3, PLLP, 48000, sdmmc3_pad, ARRAY_SIZE(sdmmc3_pad)),
	FUNIT_CFG(SDMMC4, PLLP, 48000, sdmmc4_pad, ARRAY_SIZE(sdmmc4_pad)),
};

static void setup_ec_spi(void)
{
	struct tegra_spi_channel *spi;

	spi = tegra_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);
}

static void mainboard_init(device_t dev)
{
	soc_configure_funits(funitcfgs, ARRAY_SIZE(funitcfgs));

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
