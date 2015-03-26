/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _BROADWELL_SMM_H_
#define _BROADWELL_SMM_H_

#include <stdint.h>
#include <cpu/x86/msr.h>

struct ied_header {
	char signature[10];
	u32 size;
	u8 reserved[34];
} __attribute__ ((packed));

struct smm_relocation_params {
	u32 smram_base;
	u32 smram_size;
	u32 ied_base;
	u32 ied_size;
	msr_t smrr_base;
	msr_t smrr_mask;
	msr_t emrr_base;
	msr_t emrr_mask;
	msr_t uncore_emrr_base;
	msr_t uncore_emrr_mask;
	/* The smm_save_state_in_msrs field indicates if SMM save state
	 * locations live in MSRs. This indicates to the CPUs how to adjust
	 * the SMMBASE and IEDBASE */
	int smm_save_state_in_msrs;
};

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

int smm_initialize(void);
void smm_relocate(void);

/* These helpers are for performing SMM relocation. */
void southbridge_trigger_smi(void);
void southbridge_clear_smi_status(void);

/* The initialization of the southbridge is split into 2 components. One is
 * for clearing the state in the SMM registers. The other is for enabling
 * SMIs. They are split so that other work between the 2 actions. */
void southbridge_smm_clear_state(void);
void southbridge_smm_enable_smi(void);

#endif
