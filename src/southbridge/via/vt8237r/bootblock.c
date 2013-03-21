/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/pci_ids.h>

static void bootblock_southbridge_init(void)
{
	device_t dev;
	/* don't walk other busses, HT is not enabled */

	/* ROM decode last 8MB FF800000 - FFFFFFFF on VT8237S/VT8237A */
	/* ROM decode last 4MB FFC00000 - FFFFFFFF on VT8237R */

	/* Power management controller */
	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
		    PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);

	if (dev != PCI_DEV_INVALID)
		goto found;

	/* Power management controller */
	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
		PCI_DEVICE_ID_VIA_VT8237S_LPC), 0);

	if (dev != PCI_DEV_INVALID)
		goto found;

	/* Power management controller */
	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
		PCI_DEVICE_ID_VIA_VT8237A_LPC), 0);

	if (dev == PCI_DEV_INVALID)
		return;

found:
	pci_write_config8(dev, 0x41, 0x7f);
}
