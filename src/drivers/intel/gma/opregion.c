/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2, or (at your option)
 * any later verion of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "opregion.h"

/* Write ASLS PCI register and prepare SWSCI register. */
void intel_gma_opregion_register(uintptr_t opregion)
{
	device_t igd;
	u16 reg16;

	igd = dev_find_slot(0, PCI_DEVFN(0x2, 0));
	if (!igd || !igd->enabled)
		return;

	/*
	 * Intel BIOS Specification
	 * Chapter 5.3.7 "Initialize Hardware State"
	 */
	pci_write_config32(igd, ASLS, opregion);

	/*
	 * Intel's Windows driver relies on this:
	 * Intel BIOS Specification
	 * Chapter 5.4 "ASL Software SCI Handler"
	 */
	reg16 = pci_read_config16(igd, SWSCI);
	reg16 &= ~GSSCIE;
	reg16 |= SMISCISEL;
	pci_write_config16(igd, SWSCI, reg16);
}
