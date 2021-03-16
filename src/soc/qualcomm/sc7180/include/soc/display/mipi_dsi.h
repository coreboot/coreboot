/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DISPLAY_MIPI_DSI_H_
#define _SOC_DISPLAY_MIPI_DSI_H_

#include <soc/display/panel.h>
/**********************************************************
  DSI register configuration options
 **********************************************************/
#define MIPI_DSI_MRPS			0x04	/* Maximum Return Packet Size */
#define MIPI_DSI_REG_LEN		16	/* 4 x 4 bytes register */

#define DSI_VIDEO_DST_FORMAT_RGB565		0
#define DSI_VIDEO_DST_FORMAT_RGB666		1
#define DSI_VIDEO_DST_FORMAT_RGB666_LOOSE	2
#define DSI_VIDEO_DST_FORMAT_RGB888		3

struct mipi_dsi_cmd {
	char payload[4];
	uint32_t size;
	int delay_us;
};

enum {
	DSI_VIDEO_MODE,
	DSI_CMD_MODE,
};

enum cb_err mdss_dsi_config(struct edid *edid, uint32_t num_of_lanes, uint32_t bpp);
void mdss_dsi_clock_config(void);
void mdss_dsi_video_mode_config(struct edid *edid, uint32_t bpp);
int mdss_dsi_panel_initialize(const struct panel_data *pinfo);

#endif
