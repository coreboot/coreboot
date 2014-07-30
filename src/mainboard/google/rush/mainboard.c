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
#include <soc/nvidia/tegra132/gpio.h>
#include <soc/nvidia/tegra132/clk_rst.h>
#include <soc/nvidia/tegra132/spi.h>
#include <soc/addressmap.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void init_mmc(void)
{
	clock_configure_source(sdmmc3, PLLP, 48000);
	clock_configure_source(sdmmc4, PLLP, 48000);

	uint32_t pin_up = PINMUX_PULL_UP | PINMUX_INPUT_ENABLE,
		 pin_down = PINMUX_PULL_DOWN | PINMUX_INPUT_ENABLE,
		 pin_none = PINMUX_PULL_NONE | PINMUX_INPUT_ENABLE;

	// MMC3 (sdcard reader)
	pinmux_set_config(PINMUX_SDMMC3_CLK_INDEX,
			  PINMUX_SDMMC3_CLK_FUNC_SDMMC3 | pin_none);
	pinmux_set_config(PINMUX_SDMMC3_CMD_INDEX,
			  PINMUX_SDMMC3_CMD_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT0_INDEX,
			  PINMUX_SDMMC3_DAT0_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT1_INDEX,
			  PINMUX_SDMMC3_DAT1_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT2_INDEX,
			  PINMUX_SDMMC3_DAT2_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT3_INDEX,
			  PINMUX_SDMMC3_DAT3_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_CLK_LB_IN_INDEX,
			  PINMUX_SDMMC3_CLK_LB_IN_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_CLK_LB_OUT_INDEX,
			  PINMUX_SDMMC3_CLK_LB_OUT_FUNC_SDMMC3 | pin_down);

	// MMC3 Card Detect pin.
	gpio_input_pullup(GPIO(V2));
	// Disable SD card reader power so it can be reset even on warm boot.
	// Payloads must enable power before accessing SD card slots.
	gpio_output(GPIO(R0), 0);

	// MMC4 (eMMC)
	pinmux_set_config(PINMUX_SDMMC4_CLK_INDEX,
			  PINMUX_SDMMC4_CLK_FUNC_SDMMC4 | pin_none);
	pinmux_set_config(PINMUX_SDMMC4_CMD_INDEX,
			  PINMUX_SDMMC4_CMD_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT0_INDEX,
			  PINMUX_SDMMC4_DAT0_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT1_INDEX,
			  PINMUX_SDMMC4_DAT1_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT2_INDEX,
			  PINMUX_SDMMC4_DAT2_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT3_INDEX,
			  PINMUX_SDMMC4_DAT3_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT4_INDEX,
			  PINMUX_SDMMC4_DAT4_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT5_INDEX,
			  PINMUX_SDMMC4_DAT5_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT6_INDEX,
			  PINMUX_SDMMC4_DAT6_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT7_INDEX,
			  PINMUX_SDMMC4_DAT7_FUNC_SDMMC4 | pin_up);

}

static void setup_ec_spi(void)
{
	struct tegra_spi_channel *spi;

	spi = tegra_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);
}

static void mainboard_init(device_t dev)
{
	clock_enable_clear_reset(CLK_L_SDMMC4, 0, CLK_U_SDMMC3, 0, 0, 0);

	init_mmc();
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
