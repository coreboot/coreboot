/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <cf9_reset.h>
#include <device/pci_ops.h>

#include "gm45.h"

void gm45_early_reset(void/*const timings_t *const timings*/)
{
	int ch, r;

	/* Reset DRAM power-up settings in CLKCFG (they are not
	   affected by system reset but may disrupt raminit). */
	MCHBAR32(CLKCFG_MCHBAR) =
		(MCHBAR32(CLKCFG_MCHBAR) & ~(3 << 21)) | (1 << 3);

	/*\ Next settings are the real purpose of this function:
	    If these steps are not performed, reset results in power off. \*/

	/* Initialize some DRAM settings to 1 populated rank of 128MB. */
	FOR_EACH_CHANNEL(ch) {
		/* Configure DRAM control mode. */
		MCHBAR32(CxDRC0_MCHBAR(ch)) =
			(MCHBAR32(CxDRC0_MCHBAR(ch)) & ~CxDRC0_RANKEN_MASK) |
			(ch ? 0 : CxDRC0_RANKEN(0));
		MCHBAR32(CxDRC1_MCHBAR(ch)) =
			(MCHBAR32(CxDRC1_MCHBAR(ch)) | CxDRC1_NOTPOP_MASK) &
			~(ch ? 0 : CxDRC1_NOTPOP(0));
		MCHBAR32(CxDRC2_MCHBAR(ch)) =
			(MCHBAR32(CxDRC2_MCHBAR(ch)) | CxDRC2_NOTPOP_MASK) &
			~(ch ? 0 : CxDRC2_NOTPOP(0));
		/*if (timings && (timings->mem_clock == MEM_CLOCK_1067MT))
			MCHBAR32(CxDRC2_MCHBAR(ch)) |= CxDRC2_CLK1067MT;*/

		/* Program rank boundaries (CxDRBy). */
		for (r = 0; r < RANKS_PER_CHANNEL; r += 2)
			MCHBAR32(CxDRBy_MCHBAR(ch, r)) =
				CxDRBy_BOUND_MB(r, 128) |
				CxDRBy_BOUND_MB(r+1, 128);
	}
	/* Set DCC mode to no operation and do magic 0xf0 thing. */
	MCHBAR32(DCC_MCHBAR) =
		(MCHBAR32(DCC_MCHBAR) & ~DCC_CMD_MASK) | DCC_CMD_NOP;
	u8 reg8 = pci_read_config8(PCI_DEV(0, 0, 0), 0xf0);
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf0, reg8 & ~(1 << 2));
	reg8 = pci_read_config8(PCI_DEV(0, 0, 0), 0xf0);
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf0, reg8 |  (1 << 2));
	/* Normally, we would set this after successful raminit. */
	MCHBAR32(DCC_MCHBAR) |= (1 << 19);

	system_reset();
}
