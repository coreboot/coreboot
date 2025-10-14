/* SPDX-License-Identifier: GPL-2.0-only */

#include <mipi/panel.h>

struct panel_serializable_data TM_TL121BVMS07_00C = {
	.edid = {
		.ascii_string = "TL121BVMS07",
		.manufacturer_name = "TM",
		.panel_bits_per_color = 8,
		.panel_bits_per_pixel = 24,
		.mode = {
			.pixel_clock = 264355,
			.lvds_dual_channel = 0,
			.refresh = 60,
			.ha = 1600, .hbl = 44, .hso = 20, .hspw = 4,
			.va = 2560, .vbl = 120, .vso = 82, .vspw = 2,
			.phsync = '-', .pvsync = '-',
			.x_mm = 163, .y_mm = 261,
		},
	},
	.init = {
		PANEL_DELAY(24),
		PANEL_DCS(0xFF, 0x5A, 0xA5, 0x06),
		PANEL_DCS(0x3E, 0x62),
		PANEL_DCS(0xFF, 0x5A, 0xA5, 0x02),
		PANEL_DCS(0x1B, 0x20),
		PANEL_DCS(0x5D, 0x00),
		PANEL_DCS(0x5E, 0x40),
		PANEL_DCS(0xFF, 0x5A, 0xA5, 0x07),
		PANEL_DCS(0X29, 0x00),
		PANEL_DCS(0xFF, 0x5A, 0xA5, 0x00),
		PANEL_DCS(0x11),
		PANEL_DELAY(120),
		PANEL_DCS(0x29),
		PANEL_DELAY(20),
		PANEL_END,
	},
	.flags = PANEL_FLAG_CPHY,
};
