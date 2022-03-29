/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <string.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/intel/smm_reloc.h>
#include <console/console.h>
#include <smp/node.h>
#include <soc/msr.h>
#include <soc/smmrelocate.h>

static void fill_in_relocation_params(struct smm_relocation_params *params)
{
	uintptr_t tseg_base;
	size_t tseg_size;

	smm_region(&tseg_base, &tseg_size);

	if (!IS_ALIGNED(tseg_base, tseg_size)) {
		/*
		 * Note SMRR2 is supported which might support base/size combinations.
		 * For now it looks like FSP-M always uses aligned base/size, so let's
		 * not care about that.
		 */
		printk(BIOS_WARNING,
		       "TSEG base not aligned with TSEG SIZE! Not setting SMRR\n");
		return;
	}

	/* SMRR has 32-bits of valid address aligned to 4KiB. */
	if (!IS_ALIGNED(tseg_size, 4 * KiB)) {
		printk(BIOS_WARNING,
		       "TSEG size not aligned to the minimum 4KiB! Not setting SMRR\n");
		return;
	}

	smm_subregion(SMM_SUBREGION_CHIPSET, &params->ied_base, &params->ied_size);

	params->smrr_base.lo = tseg_base | MTRR_TYPE_WRBACK;
	params->smrr_base.hi = 0;
	params->smrr_mask.lo = ~(tseg_size - 1) | MTRR_PHYS_MASK_VALID;
	params->smrr_mask.hi = 0;
}

static void setup_ied_area(struct smm_relocation_params *params)
{
	char *ied_base;

	const struct ied_header ied = {
		.signature = "INTEL RSVD",
		.size = params->ied_size,
		.reserved = {0},
	};

	ied_base = (void *)params->ied_base;

	printk(BIOS_DEBUG, "IED base = 0x%08x\n", (u32)params->ied_base);
	printk(BIOS_DEBUG, "IED size = 0x%08x\n", (u32)params->ied_size);

	/* Place IED header at IEDBASE. */
	memcpy(ied_base, &ied, sizeof(ied));

	assert(params->ied_size > 1 * MiB + 32 * KiB);

	/* Zero out 32KiB at IEDBASE + 1MiB */
	memset(ied_base + 1 * MiB, 0, 32 * KiB);
}

void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
		  size_t *smm_save_state_size)
{
	fill_in_relocation_params(&smm_reloc_params);

	smm_subregion(SMM_SUBREGION_HANDLER, perm_smbase, perm_smsize);

	if (smm_reloc_params.ied_size)
		setup_ied_area(&smm_reloc_params);

	*smm_save_state_size = sizeof(em64t101_smm_state_save_area_t);
}

static void update_save_state(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase,
				struct smm_relocation_params *relo_params)
{
	u32 smbase;
	u32 iedbase;
	em64t101_smm_state_save_area_t *save_state;
	/*
	 * The relocated handler runs with all CPUs concurrently. Therefore
	 * stagger the entry points adjusting SMBASE downwards by save state
	 * size * CPU num.
	 */
	smbase = staggered_smbase;
	iedbase = relo_params->ied_base;

	printk(BIOS_DEBUG, "New SMBASE=0x%08x IEDBASE=0x%08x\n apic_id=0x%x\n",
		smbase, iedbase, initial_lapicid());

	save_state = (void *)(curr_smbase + SMM_DEFAULT_SIZE - sizeof(*save_state));

	save_state->smbase = smbase;
	save_state->iedbase = iedbase;
}

/*
 * The relocation work is actually performed in SMM context, but the code
 * resides in the ramstage module. This occurs by trampolining from the default
 * SMRAM entry point to here.
 */
void smm_relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase)
{
	msr_t mtrr_cap, msr;
	struct smm_relocation_params *relo_params = &smm_reloc_params;

	printk(BIOS_DEBUG, "%s : CPU %d\n", __func__, cpu);

	/* Make appropriate changes to the save state map. */
	update_save_state(cpu, curr_smbase, staggered_smbase, relo_params);

	/* Write SMRR MSRs based on indicated support. */
	mtrr_cap = rdmsr(MTRR_CAP_MSR);

	/* Set Lock bit if supported */
	if (mtrr_cap.lo & SMRR_LOCK_SUPPORTED) {
		msr = rdmsr(IA32_SMRR_PHYS_MASK);
		/* Don't write the same core scope MSR if another thread has locked it,
		   otherwise system would hang. */
		if (msr.lo & SMRR_PHYS_MASK_LOCK)
			return;
		relo_params->smrr_mask.lo |= SMRR_PHYS_MASK_LOCK;
	}

	if (mtrr_cap.lo & SMRR_SUPPORTED)
		write_smrr(relo_params);
}
