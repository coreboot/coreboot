/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <edid.h>
#include <gpio.h>
#include <soc/dsi.h>
#include <soc/i2c.h>

#include "gpio.h"
#include "panel.h"

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

static int bridge_anx7625_get_edid(struct edid *edid)
{
	if (anx7625_init(BRIDGE_I2C) < 0) {
		printk(BIOS_ERR, "%s: Can't init ANX7625 bridge\n", __func__);
		return -1;
	}
	if (anx7625_dp_get_edid(BRIDGE_I2C, edid) < 0) {
		printk(BIOS_ERR, "%s: Can't get panel's edid\n", __func__);
		return -1;
	}
	return 0;
}

static int bridge_anx7625_post_power_on(const struct edid *edid)
{
	return anx7625_dp_start(BRIDGE_I2C, edid);
}

static void panel_power_on(void)
{
	/* Turn on the panel */
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
	bridge_anx7625_power_on();
}

static struct panel_description anx7625_bridge = {
	.configure_backlight = backlight_control,
	.power_on = panel_power_on,
	.get_edid = bridge_anx7625_get_edid,
	.post_power_on = bridge_anx7625_post_power_on,
	.disp_path = DISP_PATH_MIPI,
	.orientation = LB_FB_ORIENTATION_NORMAL,
};

struct panel_description *get_anx7625_description(void)
{
	mtk_i2c_bus_init(BRIDGE_I2C, I2C_SPEED_FAST);
	return &anx7625_bridge;
}

void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing)
{
	timing->da_hs_trail += 9;
}
