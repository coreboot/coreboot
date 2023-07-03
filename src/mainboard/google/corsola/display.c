/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "gpio.h"

void aw37503_init(unsigned int bus)
{
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x00, 0x14, 0x1F, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x01, 0x14, 0x1F, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x21, 0x4C, 0xFF, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x03, 0x43, 0xFF, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x21, 0x00, 0xFF, 0);
}

bool is_pmic_aw37503(unsigned int bus)
{
	u8 vendor_id;
	return (!i2c_read_field(bus, PMIC_AW37503_SLAVE,
				0x04, &vendor_id, 0x0F, 0) && vendor_id == 0x01);
}

static void backlight_control(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
	/* For staryu variants, GPIO_EN_PP3300_DISP_X is controlled in
	   mipi_panel_power_on() */
	if (!CONFIG(BOARD_GOOGLE_STARYU_COMMON))
		gpio_output(GPIO_EN_PP3300_DISP_X, 1);
}

struct panel_description *get_panel_from_cbfs(struct panel_description *desc)
{
	char cbfs_name[64];
	static union {
		u8 raw[4 * 1024];
		struct panel_serializable_data s;
	} buffer;

	if (!desc->name)
		return NULL;

	snprintf(cbfs_name, sizeof(cbfs_name), "panel-%s", desc->name);
	if (cbfs_load(cbfs_name, buffer.raw, sizeof(buffer)))
		desc->s = &buffer.s;
	else
		printk(BIOS_ERR, "Missing %s in CBFS.\n", cbfs_name);

	return desc->s ? desc : NULL;
}

static struct panel_description *get_active_panel(void)
{
	if (CONFIG(BOARD_GOOGLE_KINGLER_COMMON))
		if (CONFIG(BOARD_GOOGLE_STEELIX) && board_id() < 2)
			return get_ps8640_description();
		else
			return get_anx7625_description();
	else if (CONFIG(BOARD_GOOGLE_KRABBY_COMMON))
		return get_ps8640_description();
	else if (CONFIG(BOARD_GOOGLE_STARYU_COMMON))
		return get_panel_description();
	else
		return NULL;
}

int configure_display(void)
{
	/* Set up backlight control pins as output pin and power-off by default */
	backlight_control();

	const struct panel_description *panel = get_active_panel();

	if (!panel)
		return -1;

	printk(BIOS_INFO, "%s: Starting display init\n", __func__);

	if (panel->power_on)
		panel->power_on();

	struct edid edid = panel->s->edid;
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

	if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid,
			 panel->s->init) < 0) {
		printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
		return -1;
	}

	if (panel->post_power_on && panel->post_power_on(BRIDGE_I2C, &edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to post power on bridge\n", __func__);
		return -1;
	}

	mtk_ddp_mode_set(&edid);
	struct fb_info *info = fb_new_framebuffer_info_from_edid(&edid,
								 (uintptr_t)0);
	if (info)
		fb_set_orientation(info, panel->orientation);

	return 0;
}
