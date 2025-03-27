/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <fw_config.h>
#include <soc/display.h>
#include <variants.h>

void fw_config_panel_override(struct panel_description *panel)
{
	if (fw_config_probe(FW_CONFIG(OLED_WQXGA_PLUS, PRESENT)))
		panel->quirks |= PANEL_QUIRK_FORCE_MAX_SWING;
}
