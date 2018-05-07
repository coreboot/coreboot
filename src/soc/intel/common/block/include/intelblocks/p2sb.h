/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
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

#ifndef SOC_INTEL_COMMON_BLOCK_P2SB_H
#define SOC_INTEL_COMMON_BLOCK_P2SB_H

#include <stddef.h>
#include <stdint.h>

#define PCH_P2SB_E0 0xe0
#define   P2SB_E0_MASKLOCK (1 << 1)

enum {
	P2SB_EP_MASK_0_REG,
	P2SB_EP_MASK_1_REG,
	P2SB_EP_MASK_2_REG,
	P2SB_EP_MASK_3_REG,
	P2SB_EP_MASK_4_REG,
	P2SB_EP_MASK_5_REG,
	P2SB_EP_MASK_6_REG,
	P2SB_EP_MASK_7_REG,
	P2SB_EP_MASK_MAX_REG,
};

void p2sb_unhide(void);
void p2sb_hide(void);
void p2sb_disable_sideband_access(void);
void p2sb_enable_bar(void);
void p2sb_configure_hpet(void);

/* SOC overrides */
/*
 * Each SoC should implement EP Mask register to disable SB access
 * Input:
 * ep_mask: An array to be filled by SoC code with EP mask register.
 * count: number of element in EP mask array.
 */
void p2sb_soc_get_sb_mask(uint32_t *ep_mask, size_t count);

#endif	/* SOC_INTEL_COMMON_BLOCK_P2SB_H */
