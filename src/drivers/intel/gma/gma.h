/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _GMA_H_
#define _GMA_H_

#include <stdint.h>

struct i915_gpu_controller_info {
	int use_spread_spectrum_clock;
	int ndid;
	u32 did[5];
};

#define GMA_STATIC_DISPLAYS(ssc) {			\
	.use_spread_spectrum_clock = (ssc),		\
	.ndid = 3, .did = { 0x0100, 0x0240, 0x0410, }	\
}

void drivers_intel_gma_displays_ssdt_generate(const struct i915_gpu_controller_info *conf);

#endif
