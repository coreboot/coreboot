/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <cf9_reset.h>
#include <device/pci_ops.h>

#include "gm45.h"

void gm45_early_reset(void/*const timings_t *const timings*/)
{
	int ch, r;

	/* Reset DRAM power-up settings in CLKCFG (they are not
	   affected by system reset but may disrupt raminit). */
	mchbar_clrsetbits32(CLKCFG_MCHBAR, 3 << 21, 1 << 3);

	/*\ Next settings are the real purpose of this function:
	    If these steps are not performed, reset results in power off. \*/

	/* Initialize some DRAM settings to 1 populated rank of 128MB. */
	FOR_EACH_CHANNEL(ch) {
		/* Configure DRAM control mode. */
		mchbar_clrsetbits32(CxDRC0_MCHBAR(ch), CxDRC0_RANKEN_MASK,
			(ch ? 0 : CxDRC0_RANKEN(0)));
		mchbar_write32(CxDRC1_MCHBAR(ch),
			(mchbar_read32(CxDRC1_MCHBAR(ch)) | CxDRC1_NOTPOP_MASK) &
			~(ch ? 0 : CxDRC1_NOTPOP(0)));
		mchbar_write32(CxDRC2_MCHBAR(ch),
			(mchbar_read32(CxDRC2_MCHBAR(ch)) | CxDRC2_NOTPOP_MASK) &
			~(ch ? 0 : CxDRC2_NOTPOP(0)));
		/*if (timings && (timings->mem_clock == MEM_CLOCK_1067MT))
			mchbar_setbits32(CxDRC2_MCHBAR(ch), CxDRC2_CLK1067MT);*/

		/* Program rank boundaries (CxDRBy). */
		for (r = 0; r < RANKS_PER_CHANNEL; r += 2)
			mchbar_write32(CxDRBy_MCHBAR(ch, r),
				CxDRBy_BOUND_MB(r, 128) | CxDRBy_BOUND_MB(r + 1, 128));
	}
	/* Set DCC mode to no operation and do magic 0xf0 thing. */
	mchbar_clrsetbits32(DCC_MCHBAR, DCC_CMD_MASK, DCC_CMD_NOP);

	pci_and_config8(PCI_DEV(0, 0, 0), 0xf0, ~(1 << 2));

	pci_or_config8(PCI_DEV(0, 0, 0), 0xf0, (1 << 2));

	/* Normally, we would set this after successful raminit. */
	mchbar_setbits32(DCC_MCHBAR, 1 << 19);

	system_reset();
}
