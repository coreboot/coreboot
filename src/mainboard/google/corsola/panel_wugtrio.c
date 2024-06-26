/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <gpio.h>
#include <soc/regulator.h>

#include "gpio.h"
#include "panel.h"

static void mipi_panel_power_on(void)
{
	mainboard_set_regulator_voltage(MTK_REGULATOR_VIO18, 1800000);
	udelay(100);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VCN33, 3300000);

	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
	mdelay(5);

	/* DISP_RST_1V8_L */
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
	mdelay(15);
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	udelay(20);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}

static struct panel_description wugtrio_panels[] = {
	/* STA Technology panel with bias IC on it */
	[0] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "STA_ER88577",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_RIGHT_UP,
	},
	/* K&D Technology panel with bias IC on it */
	[4] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "KD_KD101NE3_40TI",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_RIGHT_UP,
	},
	/* LCE Corporation panel with bias IC on it */
	[7] = {
		.configure_backlight = backlight_control,
		.power_on = mipi_panel_power_on,
		.name = "LCE_LMFBX101117480",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_RIGHT_UP,
	},
};

struct panel_description *get_panel_description(void)
{
	uint32_t id = panel_id() & 0xF;
	if (id >= ARRAY_SIZE(wugtrio_panels))
		return NULL;

	return &wugtrio_panels[id];
}
