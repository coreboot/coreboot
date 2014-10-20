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

#include <arch/mmu.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <memrange.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/padconfig.h>

static const struct pad_config mmcpads[] = {
	/* MMC4 (eMMC) */
	PAD_CFG_SFIO(SDMMC4_CLK, PINMUX_INPUT_ENABLE|PINMUX_PULL_DOWN, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_CMD, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT0, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT1, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT2, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT3, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT4, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT5, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT6, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT7, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
};

static const struct funit_cfg funits[] = {
	/* MMC on SDMMC4 controller at 48MHz. */
	FUNIT_CFG(SDMMC4, PLLP, 48000, mmcpads, ARRAY_SIZE(mmcpads)),
	/* I2C6 for audio, temp sensor, etc. */
	FUNIT_CFG(I2C6, PLLP, 400, NULL, 0),
	FUNIT_CFG_USB(USBD),
};

static void mainboard_init(device_t dev)
{
	/* PLLD should be 2 * pixel clock (301620khz). */
	const uint32_t req_disp_clk =  301620 * 1000 * 2;
	uint32_t disp_clk;

	soc_configure_funits(funits, ARRAY_SIZE(funits));
	disp_clk = clock_display(req_disp_clk);

	if (disp_clk != req_disp_clk)
		printk(BIOS_DEBUG, "display clock: %u vs %u (r)\n", disp_clk,
			req_disp_clk);

	/* I2C6 bus (audio, etc.) */
	soc_configure_i2c6pad();
	i2c_init(I2C6_BUS);
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name   = "rush_ryu",
	.enable_dev = mainboard_enable,
};
