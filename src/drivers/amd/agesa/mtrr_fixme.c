/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <romstage_handoff.h>

static void set_range_uc(u32 base, u32 size)
{
	int i, max_var_mtrrs;
	msr_t msr;
	max_var_mtrrs = get_var_mtrr_count();

	for (i = 0; i < max_var_mtrrs; i++) {
		msr = rdmsr(MTRR_PHYS_MASK(i));
		if (!(msr.lo & MTRR_PHYS_MASK_VALID))
			break;
	}
	if (i == max_var_mtrrs)
		die("Run out of unused MTRRs\n");

	msr.hi = 0;
	msr.lo = base | MTRR_TYPE_UNCACHEABLE;
	wrmsr(MTRR_PHYS_BASE(i), msr);

	msr.hi = (1 << (cpu_phys_address_size() - 32)) - 1;
	msr.lo = ~(size - 1) | MTRR_PHYS_MASK_VALID;
	wrmsr(MTRR_PHYS_MASK(i), msr);
}

void fixup_cbmem_to_UC(int s3resume)
{
	if (s3resume)
		return;

	/* For normal path, INIT_POST has returned with all
	 * memory set WB cacheable. But we need CBMEM as UC
	 * to make CAR teardown with invalidation without
	 * writeback possible.
	 */

	uintptr_t top_of_ram = (uintptr_t)cbmem_top();
	top_of_ram = ALIGN_UP(top_of_ram, 4 * MiB);

	set_range_uc(top_of_ram - 4 * MiB, 4 * MiB);
	set_range_uc(top_of_ram - 8 * MiB, 4 * MiB);
}

static void recover_postcar_frame(struct postcar_frame *pcf)
{
	msr_t base, mask;
	int i;
	int s3resume = romstage_handoff_is_resume();

	/* Replicate non-UC MTRRs as left behind by AGESA.
	 */
	for (i = 0; i < pcf->mtrr->max_var_mtrrs; i++) {
		mask = rdmsr(MTRR_PHYS_MASK(i));
		base = rdmsr(MTRR_PHYS_BASE(i));
		u32 size = ~(mask.lo & ~0xfff) + 1;
		u8 type = base.lo & 0x7;
		base.lo &= ~0xfff;

		if (!(mask.lo & MTRR_PHYS_MASK_VALID) ||
			(type == MTRR_TYPE_UNCACHEABLE))
			continue;

		postcar_frame_add_mtrr(pcf, base.lo, size, type);
	}

	/* For S3 resume path, INIT_RESUME does not return with
	 * memory covering CBMEM set as WB cacheable. For better
	 * speed make them WB after CAR teardown.
	 */
	if (s3resume) {
		uintptr_t top_of_ram = (uintptr_t)cbmem_top();
		top_of_ram = ALIGN_DOWN(top_of_ram, 4 * MiB);

		postcar_frame_add_mtrr(pcf, top_of_ram - 4 * MiB, 4 * MiB,
			MTRR_TYPE_WRBACK);
		postcar_frame_add_mtrr(pcf, top_of_ram - 8 * MiB, 4 * MiB,
			MTRR_TYPE_WRBACK);
	}
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	pcf->skip_common_mtrr = 1;
	recover_postcar_frame(pcf);
}
