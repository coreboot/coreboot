/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_SRAM_H
#define SOC_INTEL_COMMON_BLOCK_SRAM_H

#include <device/device.h>

/* This function is specific to soc and defined as common weak function */
void soc_sram_init(struct device *dev);

#endif  /* SOC_INTEL_COMMON_BLOCK_SRAM_H */
