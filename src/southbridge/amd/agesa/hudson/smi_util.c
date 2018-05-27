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
 * SMM utilities used in both SMM and normal mode
 */

#include "smi.h"

#include <console/console.h>

#define HUDSON_SMI_ACPI_COMMAND		75

static void configure_smi(uint8_t smi_num, uint8_t mode)
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
	reg32 &= ~(0x3 << (bit_offset));
	reg32 |= (mode & 0x3) << bit_offset;
	smi_write32(SMI_REG_CONTROL0 + reg32_offset, reg32);
}

/**
 * Configure generation of interrupts for given GEVENT pin
 *
 * @param gevent The GEVENT pin number. Valid values are 0 thru 23
 * @param mode The type of event this pin should generate. Note that only
 *	       SMI_MODE_SMI generates an SMI. SMI_MODE_DISABLE disables events.
 * @param level SMI_LVL_LOW or SMI_LVL_HIGH
 */
void hudson_configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level)
{
	uint32_t reg32;
	/* GEVENT pins range from [0:23] */
	if (gevent > 23) {
		printk(BIOS_WARNING, "BUG: Invalid GEVENT: %u\n", gevent);
		return;
	}

	/* SMI0 source is GEVENT0 and so on */
	configure_smi(gevent, mode);

	/* And set set the trigger level */
	reg32 = smi_read32(SMI_REG_SMITRIG0);
	reg32 &= ~(1 << gevent);
	reg32 |= (level & 0x1) << gevent;
	smi_write32(SMI_REG_SMITRIG0, reg32);
}

/** Disable events from given GEVENT pin */
void hudson_disable_gevent_smi(uint8_t gevent)
{
	/* GEVENT pins range from [0:23] */
	if (gevent > 23) {
		printk(BIOS_WARNING, "BUG: Invalid GEVENT: %u\n", gevent);
		return;
	}

	/* SMI0 source is GEVENT0 and so on */
	configure_smi(gevent, SMI_MODE_DISABLE);
}

/** Enable SMIs on writes to ACPI SMI command port */
void hudson_enable_acpi_cmd_smi(void)
{
	configure_smi(HUDSON_SMI_ACPI_COMMAND, SMI_MODE_SMI);
}
