/* SPDX-License-Identifier: GPL-2.0-only */

#include <edid.h>
#include <string.h>
#include <types.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/panel.h>

struct mipi_dsi_cmd visionox_init_cmds[] = {
	{{0xFE, 0x00, 0x15, 0x80}, 0x4, 0},
	{{0xc2, 0x08, 0x15, 0x80}, 0x4, 0},
	{{0x35, 0x00, 0x15, 0x80}, 0x4, 0},
	{{0x51, 0xff, 0x15, 0x80}, 0x4, 0},
	{{0x11, 0x00, 0x05, 0x80}, 0x4, 150000},
	{{0x29, 0x00, 0x05, 0x80}, 0x4, 50000},
};

static const struct edid visionox_edid = {
	.ascii_string = "RM69299",
	.manufacturer_name = "RM",
	.panel_bits_per_color = 8,
	.panel_bits_per_pixel = 24,
	.mode = {
		.pixel_clock = 158695,
		.lvds_dual_channel = 0,
		.refresh = 60,
		.ha = 1080, .hbl = 64, .hso = 26, .hspw = 2,
		.va = 2248, .vbl = 64, .vso = 56, .vspw = 4,
		.phsync = '-', .pvsync = '-',
		.x_mm = 74, .y_mm = 131,
	},
};

const struct panel_data panel_info = {
	.lanes = 4,
	.init_cmd = visionox_init_cmds,
	.init_cmd_count = 6,
};

const struct panel_data *get_panel_config(struct edid *edid)
{
	memcpy(edid, &visionox_edid, sizeof(struct edid));
	edid_set_framebuffer_bits_per_pixel(edid, 32, 0);
	return &panel_info;
}
