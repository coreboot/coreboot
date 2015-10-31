/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc
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

#include <arch/x86/failover.ld>
#include <cpu/x86/16bit/entry16.ld>
#include <cpu/x86/16bit/reset16.ld>
#include <arch/x86/id.ld>
#if IS_ENABLED(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE)
#include <cpu/intel/fit/fit.ld>
#endif

/* Include generated .ld files. */
#include <generated/bootblock.ld>
