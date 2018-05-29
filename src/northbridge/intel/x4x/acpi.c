/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cbmem.h>
#include <arch/acpigen.h>
#include <cpu/cpu.h>
#include "x4x.h"

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct device *dev;
	u32 pciexbar = 0;
	u32 length = 0;

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!decode_pciebar(&pciexbar, &length))
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
			pciexbar, 0x0, 0x0, (length >> 20) - 1);

	return current;
}

unsigned long northbridge_write_acpi_tables(struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	unsigned long current;
	current = acpi_align_current(start);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
