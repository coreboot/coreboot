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
		.sync = GPIO_TCHPAD_INT_ODL,
	};
	tps65132s_power_on(&config);
}

static struct panel_description wyrdeer_panels[] = {
	/* K&D Technology panel */
	[6] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "KD_KD110N11_51IE",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
};

struct panel_description *get_panel_description(void)
{
	uint32_t id = panel_id() & 0xF;
	if (id >= ARRAY_SIZE(wyrdeer_panels))
		return NULL;

	return &wyrdeer_panels[id];
}
