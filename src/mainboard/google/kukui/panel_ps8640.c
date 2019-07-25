/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google Inc.
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

#include <console/console.h>
#include <delay.h>
#include <drivers/parade/ps8640/ps8640.h>
#include <edid.h>
#include <gpio.h>
#include <soc/i2c.h>
#include <string.h>

#include "panel.h"


static void power_on_ps8640(void)
{
	gpio_output(GPIO_MIPIBRDG_RST_L_1V8, 0);
	gpio_output(GPIO_PP1200_MIPIBRDG_EN, 1);
	gpio_output(GPIO_VDDIO_MIPIBRDG_EN, 1);
	mdelay(2);
	gpio_output(GPIO_MIPIBRDG_PWRDN_L_1V8, 1);
	mdelay(2);
	gpio_output(GPIO_MIPIBRDG_RST_L_1V8, 1);
	gpio_output(GPIO_PP3300_LCM_EN, 1);
}

static void dummy_power_on(void)
{
	/* The panel has been already powered on when getting panel information
	 * so we should do nothing here.
	 */
}

static struct panel_description ps8640_panel = {
	.power_on = dummy_power_on,
	.orientation = LB_FB_ORIENTATION_NORMAL,
	.init = { INIT_END_CMD },
};

struct panel_description *get_panel_description(int panel_id)
{
	static char mode_name[64];

	/* To read panel EDID, we have to first power on PS8640. */
	power_on_ps8640();

	u8 i2c_bus = 4, i2c_addr = 0x08;
	mtk_i2c_bus_init(i2c_bus);

	ps8640_init(i2c_bus, i2c_addr);
	struct edid *edid = &ps8640_panel.edid;
	if (ps8640_get_edid(i2c_bus, i2c_addr, edid)) {
		printk(BIOS_ERR, "Can't get panel's edid\n");
		return NULL;
	}
	/* TODO(hungte) Move this to ps8640_get_edid */
	snprintf(mode_name, sizeof(mode_name), "%dx%d@%dHz", edid->x_resolution,
		 edid->y_resolution, edid->mode.refresh);
	ps8640_panel.edid.mode.name = mode_name;
	return &ps8640_panel;
}
