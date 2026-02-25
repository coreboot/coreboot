/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/mt6363.h>

#include "gpio.h"
#include "panel.h"

static void mipi_panel_power_on(void)
{
	mt6363_enable_vrf18(true); /* VDD */
	mdelay(3); /* tsVSP */
	gpio_output(GPIO_EN_PP3300_EDP_X, 1); /* AVDD */
	mdelay(2); /* tPON1 */
	gpio_output(GPIO_EN_PPVAR_MIPI_DISP, 1); /* AVEE */
	mdelay(2);
	mdelay(10); /* tRW */
	gpio_output(GPIO_LCM_RST_1V8_L, 1); /* RESX */
	mdelay(5);
	gpio_output(GPIO_LCM_RST_1V8_L, 0);
	mdelay(5);
	gpio_output(GPIO_LCM_RST_1V8_L, 1);
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
