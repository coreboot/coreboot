/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dptx.h>
#include <soc/gpio_common.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "gpio.h"

/* Set up backlight control pins as output pin and power-off by default */
static void configure_edp_panel_backlight(void)
{
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_EDP_BL_PWM_1V8, 0);
}

static void power_on_edp_panel(void)
{
	gpio_output(GPIO_EN_PP3300_EDP_DISP_X, 1);
	gpio_set_pull(GPIO_EDP_HPD_1V8, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_mode(GPIO_EDP_HPD_1V8, 4);
}

int configure_display(void)
{
	struct edid edid;
	struct fb_info *info;
	const char *name;

	printk(BIOS_INFO, "%s: Starting display initialization\n", __func__);

	mtcmos_display_power_on();
	configure_edp_panel_backlight();
	power_on_edp_panel();

	mtk_ddp_init();
	mdelay(200);

	if (mtk_edp_init(&edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to initialize eDP\n", __func__);
		return -1;
	}
	name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	mtk_ddp_mode_set(&edid);
	info = fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	if (info)
		fb_set_orientation(info, LB_FB_ORIENTATION_NORMAL);

	return 0;
}
