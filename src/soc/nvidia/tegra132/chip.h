/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_NVIDIA_TEGRA132_CHIP_H__
#define __SOC_NVIDIA_TEGRA132_CHIP_H__
#include <soc/addressmap.h>
#include <stdint.h>

struct soc_nvidia_tegra132_config {
	/* Address to monitor if spintable employed. */
	uintptr_t spintable_addr;

	/*
	 * panel default specification
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
};

#endif /* __SOC_NVIDIA_TEGRA132_CHIP_H__ */
