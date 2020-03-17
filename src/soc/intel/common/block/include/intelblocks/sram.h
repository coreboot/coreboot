/*
 * This file is part of the coreboot project.
 *
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

#ifndef SOC_INTEL_COMMON_BLOCK_SRAM_H
#define SOC_INTEL_COMMON_BLOCK_SRAM_H

#include <device/device.h>

/* This function is specific to soc and defined as common weak function */
void soc_sram_init(struct device *dev);

#endif  /* SOC_INTEL_COMMON_BLOCK_SRAM_H */
