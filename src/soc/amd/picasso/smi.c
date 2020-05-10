/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Utilities for SMM setup
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <amdblocks/acpimmio.h>
#include <soc/southbridge.h>
#include <soc/smi.h>

void smm_setup_structures(void *gnvs, void *tcg, void *smi1)
{
	printk(BIOS_DEBUG, "%s STUB!!!\n", __func__);
}

/** Set the EOS bit and enable SMI generation from southbridge */
void enable_smi_generation(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg &= ~SMITRG0_SMIENB;	/* Enable SMI generation */
	reg |= SMITRG0_EOS;	/* Set EOS bit */
	smi_write32(SMI_REG_SMITRIG0, reg);

	outb(APM_CNT_SMMINFO, APM_CNT);
}
