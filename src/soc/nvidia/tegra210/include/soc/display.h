/*
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_DISPLAY_H__
#define __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_DISPLAY_H__

#define COLOR_WHITE	0xFFFFFF
#define COLOR_BLACK	0x000000

#define hsync_start(mode)	\
	(mode->xres + mode->hfront_porch)

#define hsync_end(mode)	\
	(mode->xres + mode->hfront_porch + mode->hsync_width)

#define htotal(mode)	\
	(mode->xres + mode->hfront_porch + \
	mode->hsync_width + mode->hback_porch)

#define vtotal(mode)	\
	(mode->yres + mode->vfront_porch + \
	mode->vsync_width + mode->vback_porch)

enum {
	/* norrin64 */
	TEGRA_EDID_I2C_ADDRESS = 0x50,
};

/* refresh rate = 60/s */
#define FRAME_IN_MS 17

/* forward declaration */
struct soc_nvidia_tegra210_config;
struct display_controller;

void dsi_display_startup(struct device *dev);
void dp_display_startup(struct device *dev);

int tegra_dc_init(struct display_controller *disp_ctrl);
int update_display_mode(struct display_controller *disp_ctrl,
			struct soc_nvidia_tegra210_config *config);
void update_window(const struct soc_nvidia_tegra210_config *config);
void update_display_shift_clock_divider(struct display_controller *disp_ctrl,
			u32 shift_clock_div);
void pass_mode_info_to_payload(
			struct soc_nvidia_tegra210_config *config);
#endif /* __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_DISPLAY_H__ */
