/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <edid.h>
#include <gpio.h>
#include <soc/i2c.h>

#include "panel.h"

#define ANX7625_I2C_BUS 4

static struct panel_serializable_data anx7625_data = {
	.orientation = LB_FB_ORIENTATION_NORMAL,
	.init = { INIT_END_CMD },
};

static void dummy_power_on(void)
{
	/*
	 * The panel has been already powered on when getting panel information
	 * so we should do nothing here.
	 */
}

static void start_anx7625(void)
{
	if (anx7625_dp_start(ANX7625_I2C_BUS, &anx7625_data.edid) < 0)
		printk(BIOS_ERR, "Can't start display via ANX7625.\n");
}

static struct panel_description anx7625_panel = {
	.s = &anx7625_data,
	.power_on = dummy_power_on,
	.post_power_on = start_anx7625,
};

static void power_on_anx7625(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO(PERIPHERAL_EN13), 0);
	gpio_output(GPIO(DISP_PWM), 0);

	/* Turn on bridge */
	gpio_output(GPIO_MIPIBRDG_RST_L_1V8, 0);
	gpio_output(GPIO_PP1200_MIPIBRDG_EN, 1);
	gpio_output(GPIO_VDDIO_MIPIBRDG_EN, 1);
	gpio_output(GPIO_PP1800_LCM_EN, 1);
	mdelay(2);
	gpio_output(GPIO_MIPIBRDG_PWRDN_L_1V8, 1);
	mdelay(10);
	gpio_output(GPIO_MIPIBRDG_RST_L_1V8, 1);
	gpio_output(GPIO_PP3300_LCM_EN, 1);
}

struct panel_description *get_panel_description(int panel_id)
{
	/* To read panel EDID, we have to first power on anx7625. */
	power_on_anx7625();

	mtk_i2c_bus_init(ANX7625_I2C_BUS);

	if (anx7625_init(ANX7625_I2C_BUS)) {
		printk(BIOS_ERR, "Can't init ANX7625 bridge.\n");
		return NULL;
	}

	if (anx7625_dp_get_edid(ANX7625_I2C_BUS, &anx7625_data.edid)) {
		printk(BIOS_ERR, "Can't get panel's edid.\n");
		return NULL;
	}
	return &anx7625_panel;
}
