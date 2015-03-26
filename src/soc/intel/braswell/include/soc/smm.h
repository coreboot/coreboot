/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _BAYTRAIL_SMM_H_
#define _BAYTRAIL_SMM_H_

/* There is a bug in the order of Kconfig includes in that arch/x86/Kconfig
 * is included after chipset code. This causes the chipset's Kconfig to be
 * clobbered by the arch/x86/Kconfig if they have the same name. */
static inline int smm_region_size(void)
{
	/* Make it 8MiB by default. */
	if (CONFIG_SMM_TSEG_SIZE == 0)
		return (8 << 20);
	return CONFIG_SMM_TSEG_SIZE;
}

uintptr_t smm_region_start(void);

#if !defined(__PRE_RAM__) && !defined(__SMM___)
#include <stdint.h>
void southcluster_smm_clear_state(void);
void southcluster_smm_enable_smi(void);
void southcluster_smm_save_param(int param, uint32_t data);
#endif

enum {
	SMM_SAVE_PARAM_GPIO_ROUTE = 0,
	SMM_SAVE_PARAM_PCIE_WAKE_ENABLE,
	SMM_SAVE_PARAM_COUNT
};

#endif /* _BAYTRAIL_SMM_H_ */
