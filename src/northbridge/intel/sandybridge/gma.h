/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef NORTHBRIDGE_INTEL_SANDYBRIDGE_GMA_H
#define NORTHBRIDGE_INTEL_SANDYBRIDGE_GMA_H

struct i915_gpu_controller_info;

int i915lightup_sandy(const struct i915_gpu_controller_info *info, u32 physbase, u16 pio,
			u8 *mmio, u32 lfb);

int i915lightup_ivy(const struct i915_gpu_controller_info *info, u32 physbase, u16 pio,
			u8 *mmio, u32 lfb);

#endif /* NORTHBRIDGE_INTEL_SANDYBRIDGE_GMA_H */
