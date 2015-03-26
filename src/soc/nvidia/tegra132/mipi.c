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
 * Foundation, Inc.
 */
#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <lib.h>
#include <stdlib.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <device/device.h>
#include <soc/nvidia/tegra/types.h>
#include <soc/display.h>
#include <soc/mipi_dsi.h>
#include <soc/tegra_dsi.h>
#include "jdi_25x18_display/panel-jdi-lpm102a188a.h"

static unsigned long dsi_pads[] = {
	0x060, /* DSIA & DSIB pads */
	0x180, /* DSIC & DSID pads */
};

static struct tegra_mipi mipi_data = {
	.regs = (void *)TEGRA_MIPI_CAL_BASE,
};

static inline unsigned long tegra_mipi_readl(struct tegra_mipi *mipi,
					     unsigned long reg)
{
	return read32(mipi->regs + (reg << 2));
}

static inline void tegra_mipi_writel(struct tegra_mipi *mipi,
				     unsigned long value, unsigned long reg)
{
	write32(mipi->regs + (reg << 2), value);
}

static const struct calibration_regs tegra124_mipi_calibration_regs[] = {
	{ .data = MIPI_CAL_CONFIG_CSIA, .clk = MIPI_CAL_CONFIG_CSIAB_CLK },
	{ .data = MIPI_CAL_CONFIG_CSIB, .clk = MIPI_CAL_CONFIG_CSIAB_CLK },
	{ .data = MIPI_CAL_CONFIG_CSIC, .clk = MIPI_CAL_CONFIG_CSICD_CLK },
	{ .data = MIPI_CAL_CONFIG_CSID, .clk = MIPI_CAL_CONFIG_CSICD_CLK },
	{ .data = MIPI_CAL_CONFIG_CSIE, .clk = MIPI_CAL_CONFIG_CSIE_CLK },
	{ .data = MIPI_CAL_CONFIG_DSIA, .clk = MIPI_CAL_CONFIG_DSIAB_CLK },
	{ .data = MIPI_CAL_CONFIG_DSIB, .clk = MIPI_CAL_CONFIG_DSIAB_CLK },
};
static const struct tegra_mipi_config tegra124_mipi_config = {
	.calibrate_clk_lane = 1,
	.regs = tegra124_mipi_calibration_regs,
	.num_pads = ARRAY_SIZE(tegra124_mipi_calibration_regs),
};

struct tegra_mipi_device *tegra_mipi_request(struct tegra_mipi_device *device,
						int device_index)
{
	device->mipi = &mipi_data;
	device->config = &tegra124_mipi_config;
	device->pads = dsi_pads[device_index];

	return device;
}

static int tegra_mipi_wait(struct tegra_mipi *mipi)
{
	u32 poll_interval_us = 1000;
	u32 timeout_us = 250 * 1000;
	unsigned long value;

	do {
		value = tegra_mipi_readl(mipi, MIPI_CAL_STATUS);
		if ((value & MIPI_CAL_STATUS_ACTIVE) == 0 &&
		    (value & MIPI_CAL_STATUS_DONE) != 0)
			return 0;

		if (timeout_us > poll_interval_us)
			timeout_us -= poll_interval_us;
		else
			break;

		udelay(poll_interval_us);
	} while (1);

	printk(BIOS_ERR, "%s: ERROR: timeout\n", __func__);
	return -ETIMEDOUT;
}

int tegra_mipi_calibrate(struct tegra_mipi_device *device)
{
	const struct tegra_mipi_config *cfg = device->config;
	unsigned long value, clk_value;
	unsigned int i;
	int err;

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_BIAS_PAD_CFG0);
	value &= ~MIPI_CAL_BIAS_PAD_PDVCLAMP;
	value |= MIPI_CAL_BIAS_PAD_E_VCLAMP_REF;
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG0);

	tegra_mipi_writel(device->mipi, MIPI_CAL_BIAS_PAD_CFG1_DEFAULT,
			 MIPI_CAL_BIAS_PAD_CFG1);

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_BIAS_PAD_CFG2);
	value &= ~MIPI_CAL_BIAS_PAD_PDVREG;
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG2);

	for (i = 0; i < cfg->num_pads; i++) {
		if (device->pads & BIT(i)) {
			value = MIPI_CAL_CONFIG_SELECT |
				MIPI_CAL_CONFIG_HSPDOS(0) |
				MIPI_CAL_CONFIG_HSPUOS(4) |
				MIPI_CAL_CONFIG_TERMOS(5);
			clk_value = MIPI_CAL_CONFIG_SELECT |
				    MIPI_CAL_CONFIG_HSCLKPDOSD(0) |
				    MIPI_CAL_CONFIG_HSCLKPUOSD(4);
		} else {
			value = 0;
			clk_value = 0;
		}

		tegra_mipi_writel(device->mipi, value, cfg->regs[i].data);

		if (cfg->calibrate_clk_lane)
			tegra_mipi_writel(device->mipi, clk_value,
					cfg->regs[i].clk);
	}

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_CTRL);
	value |= MIPI_CAL_CTRL_START;
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_CTRL);

	err = tegra_mipi_wait(device->mipi);

	return err;
}
