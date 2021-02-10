/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/smm.h>
#include <console/console.h>
#include <cpu/amd/amd64_save_state.h>
#include <cpu/amd/msr.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <types.h>

static struct smm_relocation_params smm_reloc_params;

static void fill_in_relocation_params(struct smm_relocation_params *params)
{
	uintptr_t tseg_base;
	size_t tseg_size;

	smm_region(&tseg_base, &tseg_size);

	params->tseg_base.lo = ALIGN_DOWN(tseg_base, 128 * KiB);
	params->tseg_base.hi = 0;
	params->tseg_mask.lo = ALIGN_DOWN(~(tseg_size - 1), 128 * KiB);
	params->tseg_mask.hi = ((1 << (cpu_phys_address_size() - 32)) - 1);

	params->tseg_mask.lo |= SMM_TSEG_WB;
}

void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize, size_t *smm_save_state_size)
{
	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	fill_in_relocation_params(&smm_reloc_params);

	smm_subregion(SMM_SUBREGION_HANDLER, perm_smbase, perm_smsize);
	*smm_save_state_size = sizeof(amd64_smm_state_save_area_t);
}

void smm_relocation_handler(int cpu, uintptr_t curr_smbase, uintptr_t staggered_smbase)
{
	struct smm_relocation_params *relo_params = &smm_reloc_params;
	amd64_smm_state_save_area_t *smm_state;

	wrmsr(SMM_ADDR_MSR, relo_params->tseg_base);
	wrmsr(SMM_MASK_MSR, relo_params->tseg_mask);

	smm_state = (void *)(SMM_AMD64_SAVE_STATE_OFFSET + curr_smbase);
	smm_state->smbase = staggered_smbase;
}
