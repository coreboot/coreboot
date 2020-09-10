/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <drivers/parade/ps8640/ps8640.h>
#include <edid.h>
#include <gpio.h>
#include <soc/i2c.h>

#include "panel.h"

static void power_on_ps8640(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO(PERIPHERAL_EN13), 0);
	gpio_output(GPIO(DISP_PWM), 0);

	/* Turn on bridge */
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

static struct panel_serializable_data ps8640_data = {
	.orientation = LB_FB_ORIENTATION_NORMAL,
	.init = { INIT_END_CMD },
};

static struct panel_description ps8640_panel = {
	.s = &ps8640_data,
	.power_on = dummy_power_on,
};

struct panel_description *get_panel_description(int panel_id)
{
	/* To read panel EDID, we have to first power on PS8640. */
	power_on_ps8640();

	u8 i2c_bus = 4, i2c_addr = 0x08;
	mtk_i2c_bus_init(i2c_bus);

	ps8640_init(i2c_bus, i2c_addr);
	struct edid *edid = &ps8640_data.edid;
	if (ps8640_get_edid(i2c_bus, i2c_addr, edid)) {
		printk(BIOS_ERR, "Can't get panel's edid\n");
		return NULL;
	}
	return &ps8640_panel;
}
