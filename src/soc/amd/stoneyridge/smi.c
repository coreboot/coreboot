/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Utilities for SMM setup
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <soc/southbridge.h>
#include <soc/smi.h>

void smm_setup_structures(void *gnvs, void *tcg, void *smi1)
{
	printk(BIOS_DEBUG, "smm_setup_structures STUB!!!\n");
}

/** Set the EOS bit and enable SMI generation from southbridge */
void enable_smi_generation(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg &= ~SMITRG0_SMIENB;	/* Enable SMI generation */
	reg |= SMITRG0_EOS;	/* Set EOS bit */
	smi_write32(SMI_REG_SMITRIG0, reg);
}
