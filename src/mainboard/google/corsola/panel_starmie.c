/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/tps65132s.h>

#include "gpio.h"
#include "panel.h"

static void mipi_panel_power_on(void)
{
	struct tps65132s_cfg config = {
		.i2c_bus = PMIC_I2C_BUS,
		.en = GPIO_EN_PP3300_DISP_X,
		.sync = GPIO_EN_PP3300_SDBRDG_X,
	};
	tps65132s_power_on(&config);
}

static struct panel_description starmie_panels[] = {
	/* K&D panel vendor and ILI9882T chip,
	   K&D and STA panel are identical except manufacturer_name. */
	[6] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "STA_ILI9882T",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
	/* STA panel vendor and ILI9882T chip */
	[9] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "STA_ILI9882T",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
	/* STA panel vendor and HIMAX83102_J02 chip */
	[10] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "STA_HIMAX83102_J02",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
};

struct panel_description *get_panel_description(void)
{
	uint32_t id = panel_id() & 0xF;
	if (id >= ARRAY_SIZE(starmie_panels))
		return NULL;

	return &starmie_panels[id];
}
