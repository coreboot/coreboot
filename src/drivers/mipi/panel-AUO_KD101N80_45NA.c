/* SPDX-License-Identifier: GPL-2.0-only */

#include <mipi/panel.h>

struct panel_serializable_data AUO_KD101N80_45NA = {
	.edid = {
		.ascii_string = "KD101N80-45NA",
		.manufacturer_name = "AUO",
		.panel_bits_per_color = 8,
		.panel_bits_per_pixel = 24,
		.mode = {
			.pixel_clock = 157000,
			.lvds_dual_channel = 0,
			.refresh = 60,
			.ha = 1200, .hbl = 140, .hso = 80, .hspw = 24,
			.va = 1920, .vbl = 36, .vso = 16, .vspw = 4,
			.phsync = '-', .pvsync = '-',
			.x_mm = 135, .y_mm = 216,
		},
	},
	.init = {
		PANEL_DELAY(10),
		PANEL_DCS(0x11),
		PANEL_DELAY(120),
		PANEL_DCS(0x29),
		PANEL_DELAY(20),
		PANEL_END,
	},
};
