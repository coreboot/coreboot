/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "../panel.h"

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
	.orientation = LB_FB_ORIENTATION_LEFT_UP,
	.init = {
		INIT_DELAY_CMD(10),
		INIT_DCS_CMD(0x11),
		INIT_DELAY_CMD(120),
		INIT_DCS_CMD(0x29),
		INIT_DELAY_CMD(20),
		INIT_END_CMD,
	},
};
