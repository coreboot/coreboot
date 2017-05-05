/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SOC_SMM_H_
#define _SOC_SMM_H_

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <fsp/memmap.h>
#include <soc/gpio.h>

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
	/*
	 * The smm_save_state_in_msrs field indicates if SMM save state
	 * locations live in MSRs. This indicates to the CPUs how to adjust
	 * the SMMBASE and IEDBASE
	 */
	int smm_save_state_in_msrs;
};

/* Mainboard handler for GPI SMIs*/
void mainboard_smi_gpi_handler(const struct gpi_status *sts);

/* Mainboard handler for eSPI SMIs */
void mainboard_smi_espi_handler(void);

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
void smm_relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase);
void smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
		size_t *smm_save_state_size);
void smm_initialize(void);
void smm_relocate(void);

/*
 * The initialization of the southbridge is split into 2 compoments. One is
 * for clearing the state in the SMM registers. The other is for enabling
 * SMIs.
 */
void southbridge_smm_clear_state(void);
void southbridge_smm_enable_smi(void);
#else	/* CONFIG_HAVE_SMI_HANDLER */
static inline void smm_relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase) {}
static inline void smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
		size_t *smm_save_state_size) {}
static inline void smm_initialize(void) {}

static inline void smm_relocate(void) {}
static inline void southbridge_smm_clear_state(void) {}
static inline void southbridge_smm_enable_smi(void) {}
#endif	/* CONFIG_HAVE_SMI_HANDLER */

#endif
