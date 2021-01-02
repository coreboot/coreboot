/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _GMA_H_
#define _GMA_H_

#include <stdint.h>

struct i915_gpu_controller_info {
	int use_spread_spectrum_clock;
	int ndid;
	u32 did[5];
};

/* Devicetree panel configuration */
struct i915_gpu_panel_config {
	unsigned int up_delay_ms;
	unsigned int down_delay_ms;
	unsigned int cycle_delay_ms;
	unsigned int backlight_on_delay_ms;
	unsigned int backlight_off_delay_ms;
	unsigned int backlight_pwm_hz;
	enum {
		GPU_BACKLIGHT_POLARITY_HIGH = 0,
		GPU_BACKLIGHT_POLARITY_LOW,
	} backlight_polarity;
};

#define GMA_STATIC_DISPLAYS(ssc) {			\
	.use_spread_spectrum_clock = (ssc),		\
	.ndid = 3, .did = { 0x0100, 0x0240, 0x0410, }	\
}

/* Shortcut for one internal panel at port A */
#define GMA_DEFAULT_PANEL(ssc) {		\
	.use_spread_spectrum_clock = (ssc),	\
	.ndid = 1, .did = { 0x0400 }		\
}

void drivers_intel_gma_displays_ssdt_generate(const struct i915_gpu_controller_info *conf);

#endif
