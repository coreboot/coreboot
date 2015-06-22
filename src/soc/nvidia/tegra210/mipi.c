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
	0x0c0, /* DSIA channel A & B pads */
	0x300, /* DSIB channel A & B pads */
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

static const struct tegra_mipi_pad tegra210_mipi_pads[] = {
	{ .data = MIPI_CAL_CONFIG_CSIA, .clk = 0 },
	{ .data = MIPI_CAL_CONFIG_CSIB, .clk = 0 },
	{ .data = MIPI_CAL_CONFIG_CSIC, .clk = 0 },
	{ .data = MIPI_CAL_CONFIG_CSID, .clk = 0 },
	{ .data = MIPI_CAL_CONFIG_CSIE, .clk = 0 },
	{ .data = MIPI_CAL_CONFIG_CSIF, .clk = 0 },
	{ .data = MIPI_CAL_CONFIG_DSIA, .clk = MIPI_CAL_CONFIG_DSIA_CLK },
	{ .data = MIPI_CAL_CONFIG_DSIB, .clk = MIPI_CAL_CONFIG_DSIB_CLK },
	{ .data = MIPI_CAL_CONFIG_DSIC, .clk = MIPI_CAL_CONFIG_DSIC_CLK },
	{ .data = MIPI_CAL_CONFIG_DSID, .clk = MIPI_CAL_CONFIG_DSID_CLK },
};

static const struct tegra_mipi_soc tegra210_mipi_soc = {
	.has_clk_lane = 1,
	.pads = tegra210_mipi_pads,
	.num_pads = ARRAY_SIZE(tegra210_mipi_pads),
	.clock_enable_override = 1,
	.needs_vclamp_ref = 0,
	.pad_drive_down_ref = 0x0,
	.pad_drive_up_ref = 0x3,
	.pad_vclamp_level = 0x1,
	.pad_vauxp_level = 0x1,
	.hspdos = 0x0,
	.hspuos = 0x2,
	.termos = 0x0,
	.hsclkpdos = 0x0,
	.hsclkpuos = 0x2,
};

struct tegra_mipi_device *tegra_mipi_request(struct tegra_mipi_device *device,
						int device_index)
{
	device->mipi = &mipi_data;
	device->mipi->soc = &tegra210_mipi_soc;
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
	const struct tegra_mipi_soc *soc = device->mipi->soc;
	unsigned int i;
	u32 value;
	int err;

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_BIAS_PAD_CFG0);
	value &= ~MIPI_CAL_BIAS_PAD_PDVCLAMP;

	if (soc->needs_vclamp_ref)
		value |= MIPI_CAL_BIAS_PAD_E_VCLAMP_REF;

	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG0);

	value = MIPI_CAL_BIAS_PAD_DRV_DN_REF(soc->pad_drive_down_ref) |
		MIPI_CAL_BIAS_PAD_DRV_UP_REF(soc->pad_drive_up_ref);
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG1);

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_BIAS_PAD_CFG2);
	value &= ~MIPI_CAL_BIAS_PAD_PDVREG;
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG2);

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_BIAS_PAD_CFG2);
	value &= ~MIPI_CAL_BIAS_PAD_VCLAMP(0x7);
	value &= ~MIPI_CAL_BIAS_PAD_VAUXP(0x7);
	value |= MIPI_CAL_BIAS_PAD_VCLAMP(soc->pad_vclamp_level);
	value |= MIPI_CAL_BIAS_PAD_VAUXP(soc->pad_vauxp_level);
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG2);

	for (i = 0; i < soc->num_pads; i++) {
		u32 clk = 0, data = 0;

		if (device->pads & BIT(i)) {
			data = MIPI_CAL_CONFIG_SELECT |
			       MIPI_CAL_CONFIG_HSPDOS(soc->hspdos) |
			       MIPI_CAL_CONFIG_HSPUOS(soc->hspuos) |
			       MIPI_CAL_CONFIG_TERMOS(soc->termos);
			clk = MIPI_CAL_CONFIG_SELECT |
			      MIPI_CAL_CONFIG_HSCLKPDOSD(soc->hsclkpdos) |
			      MIPI_CAL_CONFIG_HSCLKPUOSD(soc->hsclkpuos);
		}

		tegra_mipi_writel(device->mipi, data, soc->pads[i].data);

		if (soc->has_clk_lane && soc->pads[i].clk != 0)
			tegra_mipi_writel(device->mipi, clk, soc->pads[i].clk);
	}

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_CTRL);
	value &= ~MIPI_CAL_CTRL_NOISE_FILTER(0xf);
	value &= ~MIPI_CAL_CTRL_PRESCALE(0x3);
	value |= MIPI_CAL_CTRL_NOISE_FILTER(0xa);
	value |= MIPI_CAL_CTRL_PRESCALE(0x2);

	if (!soc->clock_enable_override)
		value &= ~MIPI_CAL_CTRL_CLKEN_OVR;
	else
		value |= MIPI_CAL_CTRL_CLKEN_OVR;

	tegra_mipi_writel(device->mipi, value, MIPI_CAL_CTRL);

	/* clear any pending status bits */
	value = tegra_mipi_readl(device->mipi, MIPI_CAL_STATUS);
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_STATUS);

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_CTRL);
	value |= MIPI_CAL_CTRL_START;
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_CTRL);

	err = tegra_mipi_wait(device->mipi);
	if (err < 0)
		printk(BIOS_ERR, "failed to calibrate MIPI pads: %d\n", err);
	else
		printk(BIOS_INFO, "MIPI calibration done\n");

	value = tegra_mipi_readl(device->mipi, MIPI_CAL_BIAS_PAD_CFG0);

	if (soc->needs_vclamp_ref)
		value &= ~MIPI_CAL_BIAS_PAD_E_VCLAMP_REF;

	value |= MIPI_CAL_BIAS_PAD_PDVCLAMP;
	tegra_mipi_writel(device->mipi, value, MIPI_CAL_BIAS_PAD_CFG0);

	return err;
}
