/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Utilities for SMM setup
 */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <amdblocks/acpimmio.h>
#include <soc/southbridge.h>
#include <soc/smi.h>

/** Set the EOS bit and enable SMI generation from southbridge */
void global_smi_enable(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg &= ~SMITRG0_SMIENB;	/* Enable SMI generation */
	reg |= SMITRG0_EOS;	/* Set EOS bit */
	smi_write32(SMI_REG_SMITRIG0, reg);
}
