/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/intel/common/opregion.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return find_resource(SA_DEV_IGD, PCI_BASE_ADDRESS_2)->base;
}

static void igd_set_resources(struct device *dev)
{
	pci_dev_set_resources(dev);
}

static unsigned long igd_write_opregion(device_t dev, unsigned long current,
				struct acpi_rsdp *rsdp)
{
	igd_opregion_t *opregion;
	uint16_t reg16;

	printk(BIOS_DEBUG, "ACPI:    * IGD OpRegion\n");
	opregion = (igd_opregion_t *)current;

	if (!init_igd_opregion(opregion))
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

	pci_write_config32(dev, ASLS, (uintptr_t)opregion);
	reg16 = pci_read_config16(dev, SWSCI);
	reg16 &= ~(1 << 0);
	reg16 |= (1 << 15);
	pci_write_config16(dev, SWSCI, reg16);

	return acpi_align_current(current);
}

static const struct device_operations igd_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = igd_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.write_acpi_tables = igd_write_opregion,
	.enable           = DEVICE_NOOP
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_IGD_HD_505,
	PCI_DEVICE_ID_INTEL_APL_IGD_HD_500,
	PCI_DEVICE_ID_INTEL_GLK_IGD,
	0,
};

static const struct pci_driver integrated_graphics_driver __pci_driver = {
	.ops		= &igd_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
