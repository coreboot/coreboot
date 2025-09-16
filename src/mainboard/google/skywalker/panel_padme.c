/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>

#include "gpio.h"
#include "panel.h"

static struct panel_description padme_panels[] = {
	[0x22] = {
		.configure_backlight = panel_configure_backlight,
		.power_on = mipi_panel_power_on,
		.name = "TM_TL121BVMS07_00C",
		.disp_path = DISP_PATH_MIPI,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
};

struct panel_description *get_panel_description(void)
{
	uint8_t id = panel_id();
	if (id >= ARRAY_SIZE(padme_panels))
		return NULL;

	return &padme_panels[id];
}
