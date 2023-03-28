/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <edid.h>
#include <gpio.h>
#include <soc/i2c.h>

#include "display.h"
#include "gpio.h"

static void bridge_anx7625_power_on(void)
{
	/* Turn on bridge */
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	gpio_output(GPIO_EN_PP1000_EDPBRDG, 1);
	gpio_output(GPIO_EN_PP1800_EDPBRDG, 1);
	gpio_output(GPIO_EN_PP3300_EDPBRDG, 1);
	mdelay(14);
	gpio_output(GPIO_EDPBRDG_PWREN, 1);
	mdelay(80);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}

static int bridge_anx7625_get_edid(u8 i2c_bus, struct edid *edid)
{
	if (anx7625_init(i2c_bus) < 0) {
		printk(BIOS_ERR, "%s: Can't init ANX7625 bridge\n", __func__);
		return -1;
	}
	if (anx7625_dp_get_edid(i2c_bus, edid) < 0) {
		printk(BIOS_ERR, "%s: Can't get panel's edid\n", __func__);
		return -1;
	}
	return 0;
}

static int bridge_anx7625_post_power_on(u8 i2c_bus, struct edid *edid)
{
	return anx7625_dp_start(i2c_bus, edid);
}

static struct panel_serializable_data anx7625_data;

static struct panel_description anx7625_bridge = {
	.s = &anx7625_data,
	.post_power_on = bridge_anx7625_post_power_on,
	.orientation = LB_FB_ORIENTATION_NORMAL,
};

struct panel_description *get_anx7625_description(void)
{
	mtk_i2c_bus_init(BRIDGE_I2C, I2C_SPEED_FAST);
	bridge_anx7625_power_on();
	if (bridge_anx7625_get_edid(BRIDGE_I2C, &anx7625_bridge.s->edid) < 0) {
		printk(BIOS_ERR, "Can't get panel's edid\n");
		return NULL;
	}
	return &anx7625_bridge;
}
