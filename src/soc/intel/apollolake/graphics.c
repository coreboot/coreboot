/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2017 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpigen.h>
#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/graphics.h>
#include <soc/intel/common/opregion.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

uintptr_t graphics_soc_write_acpi_opregion(struct device *device,
		uintptr_t current, struct acpi_rsdp *rsdp)
{
	igd_opregion_t *opregion;
	uint16_t reg16;

	printk(BIOS_DEBUG, "ACPI:    * IGD OpRegion\n");
	opregion = (igd_opregion_t *)current;

	if (init_igd_opregion(opregion) != CB_SUCCESS)
		return current;

	current += sizeof(igd_opregion_t);

	/* TODO Initialize Mailbox 3 */
	opregion->mailbox3.bclp = IGD_BACKLIGHT_BRIGHTNESS;
	opregion->mailbox3.pfit = IGD_FIELD_VALID | IGD_PFIT_STRETCH;
	opregion->mailbox3.pcft = 0; /* should be (IMON << 1) & 0x3e */
	opregion->mailbox3.cblv = IGD_FIELD_VALID | IGD_INITIAL_BRIGHTNESS;
	opregion->mailbox3.bclm[0] = IGD_WORD_FIELD_VALID + 0x0000;
	opregion->mailbox3.bclm[1] = IGD_WORD_FIELD_VALID + 0x0a19;
	opregion->mailbox3.bclm[2] = IGD_WORD_FIELD_VALID + 0x1433;
	opregion->mailbox3.bclm[3] = IGD_WORD_FIELD_VALID + 0x1e4c;
	opregion->mailbox3.bclm[4] = IGD_WORD_FIELD_VALID + 0x2866;
	opregion->mailbox3.bclm[5] = IGD_WORD_FIELD_VALID + 0x327f;
	opregion->mailbox3.bclm[6] = IGD_WORD_FIELD_VALID + 0x3c99;
	opregion->mailbox3.bclm[7] = IGD_WORD_FIELD_VALID + 0x46b2;
	opregion->mailbox3.bclm[8] = IGD_WORD_FIELD_VALID + 0x50cc;
	opregion->mailbox3.bclm[9] = IGD_WORD_FIELD_VALID + 0x5ae5;
	opregion->mailbox3.bclm[10] = IGD_WORD_FIELD_VALID + 0x64ff;

	/*
	* TODO This needs to happen in S3 resume, too.
	* Maybe it should move to the finalize handler.
	*/

	pci_write_config32(device, ASLS, (uintptr_t)opregion);
	reg16 = pci_read_config16(device, SWSCI);
	reg16 &= ~(1 << 0);
	reg16 |= (1 << 15);
	pci_write_config16(device, SWSCI, reg16);

	return acpi_align_current(current);
}
