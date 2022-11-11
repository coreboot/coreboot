/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INTEL_SMM_RELOC_H__
#define __INTEL_SMM_RELOC_H__

#include <console/console.h>
#include <types.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>

struct smm_relocation_params {
	uintptr_t ied_base;
	size_t ied_size;
	msr_t smrr_base;
	msr_t smrr_mask;
	msr_t prmrr_base;
	msr_t prmrr_mask;
	msr_t uncore_prmrr_base;
	msr_t uncore_prmrr_mask;
	/*
	 * The smm_save_state_in_msrs field indicates if SMM save state
	 * locations live in MSRs. This indicates to the CPUs how to adjust
	 * the SMMBASE and IEDBASE
	 */
	int smm_save_state_in_msrs;
};

extern struct smm_relocation_params smm_reloc_params;

struct ied_header {
	char signature[10];
	u32 size;
	u8 reserved[34];
} __packed;

/* These helpers are for performing SMM relocation. */
void northbridge_write_smram(u8 smram);

void smm_close(void);
void smm_open(void);
void smm_lock(void);
void smm_relocate(void);

/* The initialization of the southbridge is split into 2 components. One is
 * for clearing the state in the SMM registers. The other is for enabling
 * SMIs. They are split so that other work between the 2 actions. */
void smm_southbridge_clear_state(void);

/* To be removed. */
void smm_initialize(void);
void smm_info(uintptr_t *perm_smbase, size_t *perm_smsize, size_t *smm_save_state_size);
void smm_relocation_handler(int cpu, uintptr_t curr_smbase, uintptr_t staggered_smbase);

bool cpu_has_alternative_smrr(void);

#define MSR_PRMRR_PHYS_BASE 0x1f4
#define MSR_PRMRR_PHYS_MASK 0x1f5
#define MSR_UNCORE_PRMRR_PHYS_BASE 0x2f4
#define MSR_UNCORE_PRMRR_PHYS_MASK 0x2f5

static inline void write_smrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG, "Writing SMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->smrr_base.lo, relo_params->smrr_mask.lo);
	wrmsr(IA32_SMRR_PHYS_BASE, relo_params->smrr_base);
	wrmsr(IA32_SMRR_PHYS_MASK, relo_params->smrr_mask);
}

static inline void write_prmrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG, "Writing PRMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->prmrr_base.lo, relo_params->prmrr_mask.lo);
	wrmsr(MSR_PRMRR_PHYS_BASE, relo_params->prmrr_base);
	wrmsr(MSR_PRMRR_PHYS_MASK, relo_params->prmrr_mask);
}

static inline void write_uncore_prmrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG,
	       "Writing UNCORE_PRMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->uncore_prmrr_base.lo,
	       relo_params->uncore_prmrr_mask.lo);
	wrmsr(MSR_UNCORE_PRMRR_PHYS_BASE, relo_params->uncore_prmrr_base);
	wrmsr(MSR_UNCORE_PRMRR_PHYS_MASK, relo_params->uncore_prmrr_mask);
}

#endif
