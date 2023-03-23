/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dptx.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "gpio.h"
#include "panel.h"

#define PMIC_TPS65132_I2C	I2C3
#define PMIC_TPS65132_SLAVE	0x3E

int configure_display(void)
{
	struct edid edid;
	struct fb_info *info;
	const char *name;
	struct panel_description *panel = get_active_panel();
	if (!panel)
		return -1;

	printk(BIOS_INFO, "%s: Starting display initialization\n", __func__);

	mtcmos_display_power_on();
	mtcmos_protect_display_bus();

	panel->configure_panel_backlight();
	panel->power_on();

	mtk_ddp_init();
	mdelay(200);

	if (panel->disp_path == DISP_PATH_EDP) {
		if (mtk_edp_init(&edid) < 0) {
			printk(BIOS_ERR, "%s: Failed to initialize eDP\n", __func__);
			return -1;
		}

	} else {
		u32 mipi_dsi_flags = (MIPI_DSI_MODE_VIDEO |
				      MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
				      MIPI_DSI_MODE_LPM |
				      MIPI_DSI_MODE_EOT_PACKET);

		edid = panel->s->edid;

		if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid,
				 panel->s->init) < 0) {
			printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
			return -1;
		}
	}

	name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	mtk_ddp_mode_set(&edid, panel->disp_path);
	info = fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	if (info)
		fb_set_orientation(info, LB_FB_ORIENTATION_BOTTOM_UP);

	return 0;
}

void tps65132s_program_eeprom(void)
{
	u8 value = 0;
	u8 value1 = 0;

	/* Initialize I2C3 for PMIC TPS65132 */
	mtk_i2c_bus_init(PMIC_TPS65132_I2C, I2C_SPEED_FAST);
	mdelay(10);

	gpio_output(GPIO_EN_PPVAR_MIPI_DISP, 1);
	gpio_output(GPIO_EN_PPVAR_MIPI_DISP_150MA, 1);
	mdelay(10);

	i2c_read_field(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x00, &value, 0xFF, 0);
	i2c_read_field(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x01, &value1, 0xFF, 0);

	if (value != 0x11 || value1 != 0x11) {
		printk(BIOS_INFO, "Just set AVDD AVEE 5.7V to EEPROM Data in first time.\n");

		/* Set AVDD = 5.7V */
		if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x00, 0x11,
					0x1F) < 0)
			return;

		/* Set AVEE = -5.7V */
		if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x01, 0x11,
					0x1F) < 0)
			return;

		/* Set EEPROM Data */
		if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0xFF, 0x80,
					0xFC) < 0)
			return;
		mdelay(50);
	}

	gpio_output(GPIO_EN_PPVAR_MIPI_DISP, 0);
	gpio_output(GPIO_EN_PPVAR_MIPI_DISP_150MA, 0);
	mdelay(5);
}
