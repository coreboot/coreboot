/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA124_CHIP_H__
#define __SOC_NVIDIA_TEGRA124_CHIP_H__
#include <arch/cache.h>
#include <gpio.h>
#include <soc/addressmap.h>

#define EFAULT  1
#define EINVAL  2

/* this is a misuse of the device tree. We're going to let it go for now but
 * we should at minimum have a struct for the display controller, since
 * the chip supports two.
 */
struct soc_nvidia_tegra124_config {
	u32 xres;
	u32 yres;
	u32 framebuffer_bits_per_pixel;
	u32 color_depth;
	u32 panel_bits_per_pixel;
	/* there are two. It's not unimaginable that we might someday
	 * have two of these structs in a single mainboard.
	 */
	u32 display_controller;
	u32 framebuffer_base;
	/* Technically, we can compute this. At the same time, some platforms
	 * might want to specify a specific size for their own reasons. If it is
	 * zero the soc code will compute it as xres*yres*framebuffer_bits_per_pixel/4
	 */
	u32 framebuffer_size;
	/* GPIOs -- all, some, or none are used. Unused ones can be ignored
	 * in devicetree.cb since if they are not set there they default to 0,
	 * and 0 for a gpio means 'unused GPIO'.
	 */
	gpio_t backlight_en_gpio;
	gpio_t lvds_shutdown_gpio;
	gpio_t backlight_vdd_gpio;
	gpio_t panel_vdd_gpio;

	/* required info. */
	/* pwm to use to set display contrast */
	int pwm;
	/* timings -- five numbers, all relative to the previous
	 * event, not to absolute time.  e.g., vdd_data_delay is the
	 * delay from vdd on to data, not from power on to data.
	 * This is stated to be four timings in the
	 * u-boot docs. In any event, in coreboot, we generally
	 * only delay long enough to let the panel wake up and then
	 * do the control operations -- meaning, for *coreboot*
	 * we probably only need the vdd_delay, but payloads may
	 * need the other info.
	 */
	/* Delay before from power on asserting vdd */
	int vdd_delay_ms;

	/* Delay beween pwm and backlight_en_gpio is asserted */
	int pwm_to_bl_delay_ms;

	/* Delay before HPD high */
	int vdd_to_hpd_delay_ms;

	int hpd_unplug_min_us;
	int hpd_plug_min_us;
        int hpd_irq_min_us;

	int href_to_sync;
	int hsync_width;
	int hback_porch;
	int hfront_porch;
	int vref_to_sync;
	int vsync_width;
	int vback_porch;
	int vfront_porch;

	int pixel_clock;

	/* The minimum link configuration settings */
	u32 lane_count;
	u32 enhanced_framing;
	u32 link_bw;
	u32 drive_current;
	u32 preemphasis;
	u32 postcursor;

	void *dc_data;
};

#endif /* __SOC_NVIDIA_TEGRA124_CHIP_H__ */
