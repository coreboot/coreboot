/* SPDX-License-Identifier: GPL-2.0-only */

#include <mipi/panel.h>

struct panel_serializable_data SIM_DUALMIPI = {
	.edid = {
		.ascii_string = "SIM_DUAL",
		.manufacturer_name = "QCM",
		.panel_bits_per_color = 8,
		.panel_bits_per_pixel = 24,
		.mode = {
			.pixel_clock = 186811,
			.lvds_dual_channel = 0,
			.refresh = 144,
			.ha = 2880, .hbl = 24, .hso = 8, .hspw = 8,
			.va = 2560, .vbl = 14, .vso = 6, .vspw = 2,
			.phsync = 0, .pvsync = 0,
		},
	},
	.init = {
		PANEL_GENERIC(0x00, 0x00),
		PANEL_END,
	},
	.dsc_config = {
		.dsc_version_major = 0x1,
		.dsc_version_minor = 0x1,
		.bits_per_component = 8,
		.line_buf_depth = 9,
		.block_pred_enable = true,
		.convert_rgb = 1,
		.simple_422 = false,
		.bits_per_pixel = 8 << 4,
		.slice_height = 40,
		.slice_width = 720,
		.slice_chunk_size = 720,
		.initial_dec_delay = 616,
	},
};
