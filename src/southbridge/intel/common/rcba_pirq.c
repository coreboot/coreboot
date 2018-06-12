/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba_pirq.h>

#define MAX_SLOT 31
#define MIN_SLOT 19

static const u32 pirq_dir_route_reg[MAX_SLOT - MIN_SLOT + 1] = {
	D19IR, D20IR, D21IR, D22IR, D23IR, 0, D25IR,
	D26IR, D27IR, D28IR, D29IR, D30IR, D31IR,
};

enum pirq intel_common_map_pirq(const struct device *dev,
				const enum pci_pin pci_pin)
{
	u8 slot = PCI_SLOT(dev->path.pci.devfn);
	u8 shift = 4 * (pci_pin - PCI_INT_A);
	u8 pirq;
	u16 reg;

	if (pci_pin < PCI_INT_A || pci_pin > PCI_INT_D) {
		printk(BIOS_ERR,
			"ACPI_PIRQ_GEN: Slot %d PCI pin %d out of bounds\n",
			slot, pci_pin);
		return PIRQ_NONE;
	}

	/* Slot 24 should not exist and has no D24IR but better be safe here */
	if (slot < MIN_SLOT || slot > MAX_SLOT || slot == 24) {
		/* non-PCH devices use 1:1 mapping. */
		return (enum pirq)pci_pin;
	}

	reg = pirq_dir_route_reg[slot - MIN_SLOT];

	pirq = (RCBA16(reg) >> shift) & 0x7;

	return (enum pirq)(PIRQ_A + pirq);
}
