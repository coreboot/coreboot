/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#ifndef SOC_INTEL_COMMON_BLOCK_LPSS_H
#define SOC_INTEL_COMMON_BLOCK_LPSS_H

#include <stdint.h>

/* Gets controller out of reset */
void lpss_reset_release(uintptr_t base);

/*
 * Update clock divider parameters. Clock frequency is
 * configured as SOC_INTEL_COMMON_LPSS_CLOCK_MHZ * (M / N)
 */
void lpss_clk_update(uintptr_t base, uint32_t clk_m_val, uint32_t clk_n_val);

#endif	/* SOC_INTEL_COMMON_BLOCK_LPSS_H */
