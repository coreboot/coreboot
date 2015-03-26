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
#include "../chip.h"
#include <soc/display.h>
#include <soc/mipi_dsi.h>
#include <soc/tegra_dsi.h>
#include "panel-jdi-lpm102a188a.h"

struct panel_jdi jdi_data[NUM_DSI];

int panel_jdi_prepare(struct panel_jdi *jdi)
{
	int ret;
	u8 data;

	if (jdi->enabled)
		return 0;

	ret = mipi_dsi_dcs_set_column_address(jdi->dsi, 0,
				jdi->mode->xres / 2 - 1); // 2560/2
	if (ret < 0)
		printk(BIOS_ERR, "failed to set column address: %d\n", ret);

	ret = mipi_dsi_dcs_set_column_address(jdi->dsi->slave, 0,
				jdi->mode->xres / 2 - 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set column address: %d\n", ret);

	ret = mipi_dsi_dcs_set_page_address(jdi->dsi, 0,
				jdi->mode->yres - 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set page address: %d\n", ret);

	ret = mipi_dsi_dcs_set_page_address(jdi->dsi->slave, 0,
				jdi->mode->yres - 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set page address: %d\n", ret);

	ret = mipi_dsi_dcs_exit_sleep_mode(jdi->dsi);
	if (ret < 0)
		printk(BIOS_ERR, "failed to exit sleep mode: %d\n", ret);

	ret = mipi_dsi_dcs_exit_sleep_mode(jdi->dsi->slave);
	if (ret < 0)
		printk(BIOS_ERR, "failed to exit sleep mode: %d\n", ret);

	ret = mipi_dsi_dcs_set_tear_on(jdi->dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set tear on: %d\n", ret);

	ret = mipi_dsi_dcs_set_tear_on(jdi->dsi->slave,
			MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set tear on: %d\n", ret);

	ret = mipi_dsi_dcs_set_address_mode(jdi->dsi, false, false, false,
			false, false, false, false, false);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set address mode: %d\n", ret);

	ret = mipi_dsi_dcs_set_address_mode(jdi->dsi->slave, false, false,
			false, false, false, false, false, false);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set address mode: %d\n", ret);

	ret = mipi_dsi_dcs_set_pixel_format(jdi->dsi, 0x77);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set pixel format: %d\n", ret);

	ret = mipi_dsi_dcs_set_pixel_format(jdi->dsi->slave, 0x77);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set pixel format: %d\n", ret);

	data = 0xFF;
	ret = mipi_dsi_dcs_write(jdi->dsi, 0x51, &data, 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set 0x51: %d\n", ret);

	data = 0xFF;
	ret = mipi_dsi_dcs_write(jdi->dsi->slave, 0x51, &data, 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set 0x51: %d\n", ret);

	data = 0x24;
	ret = mipi_dsi_dcs_write(jdi->dsi, 0x53, &data, 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set 0x53: %d\n", ret);

	data = 0x24;
	ret = mipi_dsi_dcs_write(jdi->dsi->slave, 0x53, &data, 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set 0x53: %d\n", ret);

	data = 0x00;
	ret = mipi_dsi_dcs_write(jdi->dsi, 0x55, &data, 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set 0x55: %d\n", ret);

	data = 0x00;
	ret = mipi_dsi_dcs_write(jdi->dsi->slave, 0x55, &data, 1);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set 0x55: %d\n", ret);

	ret = mipi_dsi_dcs_set_display_on(jdi->dsi);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set display on: %d\n", ret);

	ret = mipi_dsi_dcs_set_display_on(jdi->dsi->slave);
	if (ret < 0)
		printk(BIOS_ERR, "failed to set display on: %d\n", ret);

	jdi->enabled = true;

	return 0;
}

static int panel_jdi_enslave(struct mipi_dsi_device *master,
			struct mipi_dsi_device *slave)
{
	int ret;

	ret = mipi_dsi_attach(master);
	if (ret < 0)
		return ret;

	return ret;
}

static int panel_jdi_liberate(struct mipi_dsi_device *master,
			struct mipi_dsi_device *slave)
{
	int ret;

	ret = mipi_dsi_detach(master);
	if (ret < 0)
		return ret;

	return 0;
}

static const struct mipi_dsi_master_ops panel_jdi_master_ops = {
	.enslave = panel_jdi_enslave,
	.liberate = panel_jdi_liberate,
};

struct panel_jdi *panel_jdi_dsi_probe(struct mipi_dsi_device *dsi)
{
	static int index = 0;
	struct panel_jdi *jdi;
	int ret;

	if (index >= NUM_DSI)
		return (void *)-EPTR;

	jdi = &jdi_data[index++];

	jdi->dsi = dsi;

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = 0;

	if (dsi->master) {
		ret = mipi_dsi_attach(dsi);
		if (ret < 0) {
			printk(BIOS_ERR, "mipi_dsi_attach() failed: %d\n", ret);
			return (void *)-EPTR;
		}

		ret = mipi_dsi_enslave(dsi->master, dsi);
		if (ret < 0) {
			printk(BIOS_ERR, "mipi_dsi_enslave() failed: %d\n",
				ret);
			return (void *)-EPTR;
		}

		return jdi;
	}

	dsi->ops = &panel_jdi_master_ops;

	jdi->enabled = 0;
	jdi->width_mm = 211;
	jdi->height_mm = 148;

	return jdi;
}
