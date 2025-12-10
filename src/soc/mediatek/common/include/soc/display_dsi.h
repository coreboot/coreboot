/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_DISPLAY_DSI_H
#define SOC_MEDIATEK_DISPLAY_DSI_H

#include <edid.h>
#include <types.h>

enum mipi_dsi_pixel_format {
	MIPI_DSI_FMT_RGB888,
	MIPI_DSI_FMT_RGB666,
	MIPI_DSI_FMT_RGB666_PACKED,
	MIPI_DSI_FMT_RGB565
};

/* video mode */
enum {
	MIPI_DSI_MODE_VIDEO = BIT(0),
	/* video burst mode */
	MIPI_DSI_MODE_VIDEO_BURST = BIT(1),
	/* video pulse mode */
	MIPI_DSI_MODE_VIDEO_SYNC_PULSE = BIT(2),
	/* enable auto vertical count mode */
	MIPI_DSI_MODE_VIDEO_AUTO_VERT = BIT(3),
	/* enable hsync-end packets in vsync-pulse and v-porch area */
	MIPI_DSI_MODE_VIDEO_HSE = BIT(4),
	/* disable hfront-porch area */
	MIPI_DSI_MODE_VIDEO_HFP = BIT(5),
	/* disable hback-porch area */
	MIPI_DSI_MODE_VIDEO_HBP = BIT(6),
	/* disable hsync-active area */
	MIPI_DSI_MODE_VIDEO_HSA = BIT(7),
	/* flush display FIFO on vsync pulse */
	MIPI_DSI_MODE_VSYNC_FLUSH = BIT(8),
	/* disable EoT packets in HS mode */
	MIPI_DSI_MODE_EOT_PACKET = BIT(9),
	/* device supports non-continuous clock behavior (DSI spec 5.6.1) */
	MIPI_DSI_CLOCK_NON_CONTINUOUS = BIT(10),
	/* transmit data in low power */
	MIPI_DSI_MODE_LPM = BIT(11),
	/* dsi per line's data end same time on all lanes */
	MIPI_DSI_MODE_LINE_END = BIT(12),
	/* mipi is in CPHY mode */
	MIPI_DSI_MODE_CPHY = BIT(13),
	/* mipi is DSI Dual Channel mode */
	MIPI_DSI_DUAL_CHANNEL = BIT(14),
	/* mipi is DSC compression mode */
	MIPI_DSI_DSC_MODE = BIT(15),
};

/* Public API for common display code (display.c). */
int mtk_dsi_init(u32 mode_flags, u32 format, u32 lanes, const struct edid *edid,
		 const u8 *init_commands);

#endif /* SOC_MEDIATEK_DISPLAY_DSI_H */
