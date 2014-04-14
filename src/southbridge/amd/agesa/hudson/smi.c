/*
 * Utilities for SMM setup
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "smi.h"

#include <console/console.h>
#include <cpu/cpu.h>

#define HUDSON_SMI_ACPI_COMMAND		75

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

static void enable_smi(uint8_t smi_num)
{
	uint8_t reg32_offset, bit_offset;
	uint32_t reg32;

	/* SMI sources range from [0:149] */
	if (smi_num > 149) {
		printk(BIOS_WARNING, "BUG: Invalid SMI: %u\n", smi_num);
		return;
	}

	/* 16 sources per register, 2 bits per source; registers are 4 bytes */
	reg32_offset = (smi_num / 16) * 4;
	bit_offset = (smi_num % 16) * 2;

	reg32 = smi_read32(SMI_REG_CONTROL0 + reg32_offset);
	reg32 &= ~(3 << (bit_offset));
	reg32 |= (SMI_SRC_MODE_SMI << (bit_offset));
	smi_write32(SMI_REG_CONTROL0 + reg32_offset, reg32);

}

/** Enable generation of SMIs for given GPE */
void hudson_enable_gevent_smi(uint8_t gevent)
{
	/* GEVENT pins range from [0:23] */
	if (gevent > 23) {
		printk(BIOS_WARNING, "BUG: Invalid GEVENT: %u\n", gevent);
		return;
	}

	/* SMI0 source is GEVENT0 and so on */
	enable_smi(gevent);
}

/** Enable SMIs on writes to ACPI SMI command port */
void hudson_enable_acpi_cmd_smi(void)
{
	enable_smi(HUDSON_SMI_ACPI_COMMAND);
}
