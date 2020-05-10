/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef NORTHBRIDGE_INTEL_X4X_CHIP_H
#define NORTHBRIDGE_INTEL_X4X_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_x4x_config {
	struct i915_gpu_controller_info gfx;
};

#endif
