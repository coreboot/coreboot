/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_PINEVIEW_CHIP_H
#define NORTHBRIDGE_INTEL_PINEVIEW_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_pineview_config {
	/* GMA */
	bool use_crt;
	bool use_lvds;
	struct i915_gpu_controller_info gfx;
};

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_CHIP_H */
