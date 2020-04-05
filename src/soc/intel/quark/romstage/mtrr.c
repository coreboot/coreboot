/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <soc/pci_devs.h>
#include <soc/reg_access.h>

asmlinkage void *soc_set_mtrrs(void *top_of_stack)
{
	union {
		uint32_t u32[2];
		uint64_t u64;
		msr_t msr;
	} data;
	uint32_t mtrr_count;
	uint32_t *mtrr_data;
	uint32_t mtrr_reg;

	/*
	 * The stack contents are initialized in src/soc/intel/common/stack.c
	 * to be the following:
	 *
	 *				*
	 *				*
	 *				*
	 *		   +36: MTRR mask 1 63:32
	 *		   +32: MTRR mask 1 31:0
	 *		   +28: MTRR base 1 63:32
	 *		   +24: MTRR base 1 31:0
	 *		   +20: MTRR mask 0 63:32
	 *		   +16: MTRR mask 0 31:0
	 *		   +12: MTRR base 0 63:32
	 *		    +8: MTRR base 0 31:0
	 *		    +4: Number of MTRRs to setup (described above)
	 * top_of_stack --> +0: Number of variable MTRRs to clear
	 *
	 * This routine:
	 *	* Clears all of the variable MTRRs
	 *	* Initializes the variable MTRRs with the data passed in
	 *	* Returns the new top of stack after removing all of the
	 *	  data passed in.
	 */

	/* Clear all of the variable MTRRs (base and mask). */
	mtrr_reg = MTRR_PHYS_BASE(0);
	mtrr_data = top_of_stack;
	mtrr_count = (*mtrr_data++) * 2;
	data.u64 = 0;
	while (mtrr_count-- > 0)
		soc_msr_write(mtrr_reg++, data.msr);

	/* Setup the specified variable MTRRs */
	mtrr_reg = MTRR_PHYS_BASE(0);
	mtrr_count = *mtrr_data++;
	while (mtrr_count-- > 0) {
		data.u32[0] = *mtrr_data++;
		data.u32[1] = *mtrr_data++;
		soc_msr_write(mtrr_reg++, data.msr); /* Base */
		data.u32[0] = *mtrr_data++;
		data.u32[1] = *mtrr_data++;
		soc_msr_write(mtrr_reg++, data.msr); /* Mask */
	}

	/* Remove setup_stack_and_mtrrs data and return the new top_of_stack */
	top_of_stack = mtrr_data;
	return top_of_stack;
}

asmlinkage void soc_enable_mtrrs(void)
{
	union {
		uint32_t u32[2];
		uint64_t u64;
		msr_t msr;
	} data;

	/* Enable MTRR. */
	data.msr = soc_msr_read(MTRR_DEF_TYPE_MSR);
	data.u32[0] |= MTRR_DEF_TYPE_EN;
	soc_msr_write(MTRR_DEF_TYPE_MSR, data.msr);
}
