/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "gpio.h"

static void backlight_control(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
}

int panel_pmic_reg_mask(unsigned int bus, uint8_t chip, uint8_t addr,
			uint8_t val, uint8_t mask)
{
	uint8_t msg = 0;

	if (i2c_read_field(bus, chip, addr, &msg, 0xFF, 0) < 0) {
		printk(BIOS_ERR, "%s: Failed to read i2c(%u): addr(%u)\n",
			__func__, bus, addr);
		return -1;
	}

	msg &= ~mask;
	msg |= val;

	return i2c_write_field(bus, chip, addr, msg, 0xFF, 0);
}

void tps65132s_program_eeprom(void)
{
	u8 value = 0;
	u8 value1 = 0;

	/* Initialize I2C6 for PMIC TPS65132 */
	mtk_i2c_bus_init(PMIC_TPS65132_I2C, I2C_SPEED_FAST);
	mdelay(10);

	/* EN_PP6000_MIPI_DISP */
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
	/* EN_PP6000_MIPI_DISP_150MA */
	gpio_output(GPIO_EN_PP3300_SDBRDG_X, 1);
	mdelay(10);

	i2c_read_field(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x00, &value, 0xFF, 0);
	i2c_read_field(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x01, &value1, 0xFF, 0);

	if (value != 0x14 || value1 != 0x14) {
		printk(BIOS_INFO, "Set AVDD AVEE 6.0V to EEPROM Data in first time\n");

		/* Set AVDD = 6.0V */
		if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x00, 0x14,
					0x1F) < 0)
			return;

		/* Set AVEE = -6.0V */
		if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x01, 0x14,
					0x1F) < 0)
			return;

		/* Set EEPROM Data */
		if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0xFF, 0x80,
					0xFC) < 0)
			return;
		mdelay(50);
	}
	/* EN_PP6000_MIPI_DISP */
	gpio_output(GPIO_EN_PP3300_DISP_X, 0);
	/* EN_PP6000_MIPI_DISP_150MA */
	gpio_output(GPIO_EN_PP3300_SDBRDG_X, 0);
	mdelay(5);
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
	const struct panel_description *panel = get_active_panel();

	if (!panel)
		return -1;

	printk(BIOS_INFO, "%s: Starting display init\n", __func__);

	/* Set up backlight control pins as output pin and power-off by default */
	backlight_control();

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
