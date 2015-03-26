/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *               2012 secunet Security Networks AG
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
 * Foundation, Inc.
 */

#ifndef NORTHBRIDGE_INTEL_GM45_CHIP_H
#define NORTHBRIDGE_INTEL_GM45_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_gm45_config {
	struct i915_gpu_controller_info gfx;
};

#endif				/* NORTHBRIDGE_INTEL_GM45_CHIP_H */
