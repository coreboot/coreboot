/* SPDX-License-Identifier: GPL-2.0-only */

#include <mipi/panel.h>

struct panel_serializable_data TIANMA_ILITEK79900A = {
	.flags = PANEL_FLAG_CPHY,
	.edid = {
		.ascii_string = "ILITEK79900A",
		.manufacturer_name = "TM",
		.panel_bits_per_color = 8,
		.panel_bits_per_pixel = 24,
		.mode = {
			.pixel_clock = 551117,
			.lvds_dual_channel = 0,
			.refresh = 120,
			.ha = 1600, .hbl = 64, .hso = 24, .hspw = 20,
			.va = 2560, .vbl = 200, .vso = 62, .vspw = 2,
			.phsync = 0, .pvsync = 0,
			.x_mm = 261, .y_mm = 163,
		},
	},
	.poweroff = {
		PANEL_DCS(MIPI_DCS_SET_DISPLAY_OFF),
		PANEL_DELAY(20),
		PANEL_DCS(MIPI_DCS_ENTER_SLEEP_MODE),
		PANEL_DELAY(80),
		PANEL_END,
	},
	.init = {
		PANEL_DCS(0xff, 0x5a, 0xa5, 0x06),
		PANEL_DCS(0x3e, 0x62),
		PANEL_DCS(0x8f, 0xf0),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x08),
		PANEL_DCS(0x7d, 0xaf),
		PANEL_DCS(0x7f, 0xda),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x05),
		PANEL_DCS(0x3f, 0x47),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x01),
		PANEL_DCS(0xb0, 0x00),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x16),
		PANEL_DCS(0x00, 0x01),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x02),
		PANEL_DCS(0x1b, 0x00),
		PANEL_DCS(0x5d, 0x00),
		PANEL_DCS(0x5e, 0x40),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x05),
		PANEL_DCS(0x9e, 0xe9),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x07),
		PANEL_DCS(0x29, 0x01),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x06),
		PANEL_DCS(0x91, 0x45),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x16),
		PANEL_DCS(0x03, 0x4b),
		PANEL_DCS(0x04, 0x73),
		PANEL_DCS(0x05, 0xdf),
		PANEL_DCS(0x00, 0x01),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x10),
		PANEL_DCS(0x12, 0x8c),
		PANEL_DCS(0x14, 0x3c),
		PANEL_DCS(0x15, 0x3d),
		PANEL_DCS(0x1d, 0xfc),
		PANEL_DCS(0x25, 0x9d),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x0e),
		PANEL_DCS(0xc0, 0x18),
		PANEL_DCS(0x2a, 0x0e),
		PANEL_DCS(0x38, 0xcd),
		PANEL_DCS(0x80, 0x53),
		PANEL_DCS(0x81, 0x0e),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x1e),
		PANEL_DCS(0x61, 0x5c),

		PANEL_DCS(0xff, 0x5a, 0xa5, 0x00),
		PANEL_DCS(0x35, 0x00),
		PANEL_DCS(MIPI_DCS_EXIT_SLEEP_MODE),
		PANEL_DELAY(120),
		PANEL_DCS(MIPI_DCS_SET_DISPLAY_ON),
		PANEL_END,
	},
	.dsc_config = {
		.dsc_version_major = 0x1,
		.dsc_version_minor = 0x2,
		.bits_per_component = 8,
		.line_buf_depth = 9,
		.block_pred_enable = true,
		.convert_rgb = 1,
		.simple_422 = false,
		.bits_per_pixel = 8 << 4,
		.slice_height = 8,
		.slice_width = 800,
		.slice_chunk_size = 800,
	},
};
