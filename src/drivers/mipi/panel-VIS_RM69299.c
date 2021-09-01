/* SPDX-License-Identifier: GPL-2.0-only */

#include <mipi/panel.h>

struct panel_serializable_data VIS_RM69299 = {
	.edid = {
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
	},
	.init = {
		PANEL_DCS(0xFE, 0x00, 0x15, 0x80),
		PANEL_DCS(0xc2, 0x08, 0x15, 0x80),
		PANEL_DCS(0x35, 0x00, 0x15, 0x80),
		PANEL_DCS(0x51, 0xff, 0x15, 0x80),
		PANEL_DCS(0x11, 0x00, 0x05, 0x80),
		PANEL_DELAY(150),
		PANEL_DCS(0x29, 0x00, 0x05, 0x80),
		PANEL_DELAY(50),
		PANEL_END,
	},
};
