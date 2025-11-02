/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/tps65132s.h>

#include "gpio.h"
#include "panel.h"

static void mipi_panel_power_on(void)
{
	struct tps65132s_cfg config = {
		.i2c_bus = PMIC_I2C_BUS,
		.en = GPIO_EN_PP3300_EDP_X,
		.sync = GPIO_EN_PPVAR_MIPI_DISP,
	};
	tps65132s_power_on(&config);
}

static struct panel_description sapphire_panels[] = {
	[0x09] = {
		.configure_backlight = configure_backlight,
		.power_on = mipi_panel_power_on,
		.name = "BOE_NS130069_M00",
		.disp_path = DISP_PATH_DUAL_MIPI,
		.orientation = LB_FB_ORIENTATION_NORMAL,
	},
};

struct panel_description *get_panel_description(void)
{
	uint32_t id = panel_id();
	if (id >= ARRAY_SIZE(sapphire_panels))
		return NULL;

	return &sapphire_panels[id];
}
