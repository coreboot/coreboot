/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <edid.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "gpio.h"

/* Set up backlight control pins as output pin and power-off by default */
static void configure_backlight_and_bridge(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);

	/* Turn on bridge */
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	gpio_output(GPIO_EN_PP1000_EDPBRDG, 1);
	gpio_output(GPIO_EN_PP1800_EDPBRDG, 1);
	gpio_output(GPIO_EN_PP3300_EDPBRDG, 1);
	mdelay(14);
	gpio_output(GPIO_EDPBRDG_PWREN, 1);
	mdelay(10);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}

static int anx7625_setup(u8 i2c_bus, struct edid *edid)
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

static int bridge_setup(u8 i2c_bus, struct edid *edid)
{
	if (CONFIG(BOARD_GOOGLE_KINGLER_COMMON))
		return anx7625_setup(i2c_bus, edid);
	else
		printk(BIOS_ERR, "%s: There is no bridge IC supported\n", __func__);

	return -1;
}

static int bridge_post_poweron(u8 i2c_bus, struct edid *edid)
{
	if (CONFIG(BOARD_GOOGLE_KINGLER_COMMON))
		return anx7625_dp_start(i2c_bus, edid);

	return 0;
}

int configure_display(void)
{
	struct edid edid;
	const u8 i2c_bus = I2C0;

	printk(BIOS_INFO, "%s: Starting display init\n", __func__);

	configure_backlight_and_bridge();
	mtk_i2c_bus_init(i2c_bus, I2C_SPEED_FAST);

	if (bridge_setup(i2c_bus, &edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to set bridge\n", __func__);
		return -1;
	}

	const char *name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	mtcmos_display_power_on();
	mtcmos_protect_display_bus();

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);
	mtk_ddp_init();
	u32 mipi_dsi_flags = (MIPI_DSI_MODE_VIDEO |
			      MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			      MIPI_DSI_MODE_LPM |
			      MIPI_DSI_MODE_EOT_PACKET);

	if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid, NULL) < 0) {
		printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
		return -1;
	}

	if (bridge_post_poweron(i2c_bus, &edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to post poweron bridge\n", __func__);
		return -1;
	}

	mtk_ddp_mode_set(&edid);
	fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);

	return 0;
}
