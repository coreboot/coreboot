/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <drivers/parade/ps8640/ps8640.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/regulator.h>
#include <soc/i2c.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "gpio.h"

/* Bridge functions */
static void bridge_ps8640_power_on(void)
{
	/*
	 * PS8640 power-on sequence is described in chapter 14, PS8640_DS_V1.4_20200210.docx
	 * - set VDD12 to be 1.2V
	 * - delay 100us
	 * - set VDD33 to be 3.3V
	 * - pull hign PD#
	 * - pull down RST#
	 * - delay 2ms
	 * - pull high RST#
	 * - delay more than 50ms (55ms for margin)
	 * - pull down RST#
	 * - delay more than 50ms (55ms for margin)
	 * - pull high RST#
	 */

	/* Set VRF12 to 1.2V and VCN33 to 3.3V */
	mainboard_set_regulator_voltage(MTK_REGULATOR_VRF12, 1200000);
	udelay(100);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VCN33, 3300000);
	udelay(200);

	/* Turn on bridge */
	gpio_output(GPIO_EDPBRDG_PWREN, 1);
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	mdelay(2);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
	mdelay(55);
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	mdelay(55);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}

static int bridge_ps8640_get_edid(u8 i2c_bus, struct edid *edid)
{
	const u8 chip = 0x8;

	if (ps8640_init(i2c_bus, chip) < 0) {
		printk(BIOS_ERR, "%s: Can't init PS8640 bridge\n", __func__);
		return -1;
	}
	if (ps8640_get_edid(i2c_bus, chip, edid) < 0) {
		printk(BIOS_ERR, "%s: Can't get panel's edid\n", __func__);
		return -1;
	}
	return 0;
}

static int bridge_ps8640_post_power_on(u8 i2c_bus, struct edid *edid)
{
	/* Do nothing */
	return 0;
}

static void bridge_anx7625_power_on(void)
{
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

/* Display function */
static void backlight_control(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
}

static const struct edp_bridge anx7625_bridge = {
	.power_on = bridge_anx7625_power_on,
	.get_edid = bridge_anx7625_get_edid,
	.post_power_on = bridge_anx7625_post_power_on,
};

static const struct edp_bridge ps8640_bridge = {
	.power_on = bridge_ps8640_power_on,
	.get_edid = bridge_ps8640_get_edid,
	.post_power_on = bridge_ps8640_post_power_on,
};

_Static_assert(CONFIG(DRIVER_ANALOGIX_ANX7625) + CONFIG(DRIVER_PARADE_PS8640) == 1,
	       "Exactly one of ANX7625 and PS8640 must be set");

int configure_display(void)
{
	struct edid edid;
	const u8 i2c_bus = I2C0;
	const struct edp_bridge *bridge;

	if (CONFIG(DRIVER_ANALOGIX_ANX7625))
		bridge = &anx7625_bridge;
	else if (CONFIG(DRIVER_PARADE_PS8640))
		bridge = &ps8640_bridge;

	printk(BIOS_INFO, "%s: Starting display init\n", __func__);

	mtk_i2c_bus_init(i2c_bus, I2C_SPEED_FAST);

	/* Set up backlight control pins as output pin and power-off by default */
	backlight_control();

	assert(bridge->power_on);
	bridge->power_on();

	assert(bridge->get_edid);
	if (bridge->get_edid(i2c_bus, &edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to get edid\n", __func__);
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

	if (bridge->post_power_on(i2c_bus, &edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to post power on bridge\n", __func__);
		return -1;
	}

	mtk_ddp_mode_set(&edid);
	fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);

	return 0;
}
