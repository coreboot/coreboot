/*
 * This file is part of the coreboot project.
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

#ifndef NORTHBRIDGE_INTEL_PINEVIEW_CHIP_H
#define NORTHBRIDGE_INTEL_PINEVIEW_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_pineview_config {
	/* GMA */
	bool use_crt;
	bool use_lvds;
	int gpu_lvds_use_spread_spectrum_clock;
	struct i915_gpu_controller_info gfx;
};

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_CHIP_H */
