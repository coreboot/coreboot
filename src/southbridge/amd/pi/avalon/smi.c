/*
 * Utilities for SMM setup
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "smi.h"

#include <console/console.h>
#include <cpu/cpu.h>

void smm_setup_structures(void *gnvs, void *tcg, void *smi1)
{
	printk(BIOS_DEBUG, "smm_setup_structures STUB!!!\n");
}

/** Set the EOS bit and enable SMI generation from southbridge */
void hudson_enable_smi_generation(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg &= ~SMITRG0_SMIENB;	/* Enable SMI generation */
	reg |= SMITRG0_EOS;	/* Set EOS bit */
	smi_write32(SMI_REG_SMITRIG0, reg);
}
