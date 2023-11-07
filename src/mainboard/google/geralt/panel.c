/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <edid.h>
#include <gpio.h>
#include <identity.h>
#include <soc/gpio_common.h>
#include <string.h>

#include "gpio.h"
#include "panel.h"

static void get_mipi_cmd_from_cbfs(struct panel_description *desc)
{
	/*
	 * The CBFS file name is panel-{MANUFACTURER}-${PANEL_NAME}, where MANUFACTURER is 3
	 * characters and PANEL_NAME is usually 13 characters.
	 */
	char cbfs_name[64];
	static union {
		u8 raw[4 * 1024];  /* Most panels only need < 2K. */
		struct panel_serializable_data s;
	} buffer;

	if (!desc->name) {
		printk(BIOS_ERR, "Missing panel CBFS file name.\n");
		return;
	}

	snprintf(cbfs_name, sizeof(cbfs_name), "panel-%s", desc->name);
	if (cbfs_load(cbfs_name, buffer.raw, sizeof(buffer)))
		desc->s = &buffer.s;
	else
		printk(BIOS_ERR, "Missing %s in CBFS.\n", cbfs_name);
}

struct panel_description __weak *get_panel_description(uint32_t panel_id)
{
	printk(BIOS_WARNING, "%s: %s: the panel configuration is not ready\n",
	       __func__, mainboard_part_number);
	return NULL;
}

struct panel_description *get_active_panel(void)
{
	uint32_t active_panel_id = panel_id();

	struct panel_description *panel = get_panel_description(active_panel_id);
	if (!panel || panel->disp_path == DISP_PATH_NONE) {
		printk(BIOS_ERR, "%s: Panel %u is not supported.\n", __func__, active_panel_id);
		return NULL;
	}

	/* For eDP, we will get edid after eDP initialization is done, so we return directly. */
	if (panel->disp_path == DISP_PATH_EDP) {
		printk(BIOS_INFO, "%s: Use eDP as the display\n", __func__);
		return panel;
	}

	/* We need to find init cmds for MIPI panel from CBFS */
	get_mipi_cmd_from_cbfs(panel);
	if (!panel->s)
		return NULL;

	const struct edid *edid = &panel->s->edid;
	const char *name = edid->ascii_string;
	if (name[0] == '\0') {
		name = "unknown name";
		printk(BIOS_INFO, "%s: Found ID %u: '%s %s' %dx%d@%dHz\n", __func__,
		       active_panel_id, edid->manufacturer_name, name, edid->mode.ha,
		       edid->mode.va, edid->mode.refresh);
	}
	return panel;
}

void configure_mipi_pwm_backlight(void)
{
	gpio_output(GPIO_AP_DISP_BKLTEN, 0);
	gpio_output(GPIO_MIPI_BL_PWM_1V8, 0);
}

void fill_lp_backlight_gpios(struct lb_gpios *gpios)
{
	struct panel_description *panel = get_active_panel();
	if (!panel || panel->disp_path == DISP_PATH_NONE)
		return;

	struct lb_gpio mipi_pwm_gpios[] = {
		{GPIO_MIPI_BL_PWM_1V8.id, ACTIVE_HIGH, -1, "PWM control"},
	};

	struct lb_gpio edp_pwm_gpios[] = {
		{GPIO_EDP_BL_PWM_1V8.id, ACTIVE_HIGH, -1, "PWM control"},
	};

	if (panel->pwm_ctrl_gpio) {
		/* PWM control for typical eDP and MIPI panels */
		if (panel->disp_path == DISP_PATH_MIPI)
			lb_add_gpios(gpios, mipi_pwm_gpios, ARRAY_SIZE(mipi_pwm_gpios));
		else
			lb_add_gpios(gpios, edp_pwm_gpios, ARRAY_SIZE(edp_pwm_gpios));
	}

	struct lb_gpio backlight_gpios[] = {
		{GPIO_AP_DISP_BKLTEN.id, ACTIVE_HIGH, -1, "backlight enable"},
	};

	lb_add_gpios(gpios, backlight_gpios, ARRAY_SIZE(backlight_gpios));
}
