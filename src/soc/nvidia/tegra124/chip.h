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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_NVIDIA_TEGRA124_CHIP_H__
#define __SOC_NVIDIA_TEGRA124_CHIP_H__
#include <arch/cache.h>
#include <soc/addressmap.h>
#include "gpio.h"

/* this is a misuse of the device tree. We're going to let it go for now but
 * we should at minimum have a struct for the display controller, since
 * the chip supports two.
 */
struct soc_nvidia_tegra124_config {
	int xres;
	int yres;
	int framebuffer_bits_per_pixel;
	int cache_policy;
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
	 * do the control operations -- meaming, for *coreboot*
	 * we probably only need the vdd_delay, but payloads may
	 * need the other info.
	 */
	/* Delay before from power on asserting vdd */
	int vdd_delay;
        /* delay between panel_vdd-rise and data-rise*/
	int vdd_data_delay;
	/* delay between data-rise and backlight_vdd-rise */
	int data_backlight_delay;
	/* delay between backlight_vdd and pwm-rise */
	int backlight_pwm_delay;
	/* delay between pwm-rise and backlight_en-rise */
	int pwm_backlight_en_delay;
	/* display timing.
	 * we have not found a dts in which these are set */
	int href_to_sync; /* u-boot code says 'set to 1' */
	int hsync_width;
	int hback_porch;
	int hfront_porch;
	int vref_to_sync; /* u-boot code says 'set to 1' */
	int vsync_width;
	int vback_porch;
	int vfront_porch;

	int pixel_clock;
};

#endif /* __SOC_NVIDIA_TEGRA124_CHIP_H__ */
