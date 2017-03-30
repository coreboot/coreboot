/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Chromium OS Authors
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

#ifndef NORTHBRIDGE_INTEL_SANDYBRIDGE_GMA_H
#define NORTHBRIDGE_INTEL_SANDYBRIDGE_GMA_H

struct i915_gpu_controller_info;

int i915lightup_sandy(const struct i915_gpu_controller_info *info,
		u32 physbase, u16 pio, u8 *mmio, u32 lfb);
int i915lightup_ivy(const struct i915_gpu_controller_info *info,
		u32 physbase, u16 pio, u8 *mmio, u32 lfb);

#endif /* NORTHBRIDGE_INTEL_SANDYBRIDGE_GMA_H */
