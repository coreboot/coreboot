/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA210_CHIP_H__
#define __SOC_NVIDIA_TEGRA210_CHIP_H__
#include <soc/addressmap.h>
#include <stdint.h>
#include <soc/nvidia/tegra/dc.h>

struct soc_nvidia_tegra210_config {
	/*
	 * panel resolution
	 *  The two parameters below provides dc about panel spec.
	 */
	u32 xres;		/* the width of H display active area */
	u32 yres;		/* the height of V display active area */
	u32 framebuffer_bits_per_pixel;
	u32 color_depth;	/* color format */

	u64 display_controller;	/* dc block base address */
	u32 framebuffer_base;

	/*
	 * Technically, we can compute this. At the same time, some platforms
	 * might want to specify a specific size for their own reasons. If it
	 * is zero the soc code will compute it as
	 * xres*yres*framebuffer_bits_per_pixel/8
	 */
	u32 framebuffer_size;

	/*
	 * Framebuffer resolution
	 *  The two parameters below provides dc about framebuffer's sdram size.
	 *  When they are not the same as panel resolution, we need to program
	 *  dc's DDA_INCREMENT and some other registers to resize dc output.
	 */
	u32 display_xres;
	u32 display_yres;

	int href_to_sync;	/* HSYNC position with respect to line start */
	int hsync_width;	/* the width of HSYNC pulses */
	int hback_porch;	/* the distance between HSYNC trailing edge to
				   beginning of H display active area */
	int hfront_porch;	/* the distance between end of H display active
				   area to the leading edge of HSYNC */
	int vref_to_sync;
	int vsync_width;
	int vback_porch;
	int vfront_porch;
	int refresh;		/* display refresh rate */

	int pixel_clock;	/* dc pixel clock source rate */

	u32 panel_bits_per_pixel;

	/* dp specific fields */
	struct {
		/* pwm to use to set display contrast */
		int pwm;

		/* HPD related timing */
		int vdd_to_hpd_delay_ms;
		int hpd_unplug_min_us;
		int hpd_plug_min_us;
		int hpd_irq_min_us;

		/* The minimum link configuration settings */
		u32 lane_count;
		u32 enhanced_framing;
		u32 link_bw;
		u32 drive_current;
		u32 preemphasis;
		u32 postcursor;
	} dp;

	int win_opt;
	void *dc_data;
};

#endif /* __SOC_NVIDIA_TEGRA210_CHIP_H__ */
