/* SPDX-License-Identifier: GPL-2.0-only */

#include "../panel.h"

struct panel_serializable_data AUO_NT51021D8P = {
	.edid = {
		.ascii_string = "NT51021D8P",
		.manufacturer_name = "AUO",
		.panel_bits_per_color = 8,
		.panel_bits_per_pixel = 24,
		.mode = {
			.pixel_clock = 159420,
			.lvds_dual_channel = 0,
			.refresh = 60,
			.ha = 1200, .hbl = 141, .hso = 80, .hspw = 1,
			.va = 1920, .vbl = 61, .vso = 35, .vspw = 1,
			.phsync = '-', .pvsync = '-',
			.x_mm = 107, .y_mm = 132,
		},
	},
	.init = {
		INIT_DCS_CMD(0x11),
		INIT_DELAY_CMD(0x78),
		INIT_DCS_CMD(0x29),
		INIT_DELAY_CMD(0x14),
		INIT_END_CMD,
	},
};
