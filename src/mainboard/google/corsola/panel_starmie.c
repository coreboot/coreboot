/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <gpio.h>
#include <soc/regulator.h>

#include "display.h"
#include "gpio.h"

static void mipi_panel_power_on(void)
{
	tps65132s_program_eeprom();
	mainboard_set_regulator_voltage(MTK_REGULATOR_VIO18, 1800000);
	mdelay(1);
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
	gpio_output(GPIO_EN_PP3300_SDBRDG_X, 1);
	mdelay(1);
	/* DISP_RST_1V8_L */
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
	mdelay(1);
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	udelay(20);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}

static struct panel_description starmie_panels[] = {
	[8] = {
		.power_on = mipi_panel_power_on,
		.name = "STA_ILI9882T",
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
	[10] = {
		.power_on = mipi_panel_power_on,
		.name = "STA_HIMAX83102_J02",
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
};

struct panel_description *get_panel_description(void)
{
	uint32_t id = panel_id() & 0xF;
	if (id >= ARRAY_SIZE(starmie_panels))
		return NULL;

	return get_panel_from_cbfs(&starmie_panels[id]);
}
