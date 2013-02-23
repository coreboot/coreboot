/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include "i3100.h"

void i3100_enable(device_t dev)
{
	device_t lpc_dev;
	u8 func;
	volatile u32 *disable;

	if (dev->enabled)
		return;

	/*
	 * To disable an integrated southbridge device, set the corresponding
	 * flag in the Function Disable register.
	 */

	/* Temporarily enable the root complex register block at 0xa0000000. */
	lpc_dev = dev_find_slot(0x0, PCI_DEVFN(0x1f, 0x0));
	pci_write_config32(lpc_dev, 0xf0, 0xa0000000 | (1 << 0));
	disable = (volatile u32 *) 0xa0003418;
	func = PCI_FUNC(dev->path.pci.devfn);
	switch (PCI_SLOT(dev->path.pci.devfn)) {
	case 0x1f: /* LPC (fn0), SATA (fn2), SMBus (fn3) */
		*disable |= (1 << (func == 0x0 ? 14 : func));
		break;
	case 0x1d: /* UHCI (fn0, fn1), EHCI (fn7) */
		*disable |= (1 << (func + 8));
		break;
	case 0x1c: /* PCIe ports B0-B3 (fn0-fn3) */
		*disable |= (1 << (func + 16));
		break;
	}
	/* Disable the root complex register block. */
	pci_write_config32(lpc_dev, 0xf0, 0);
}

struct chip_operations southbridge_intel_i3100_ops = {
	CHIP_NAME("Intel 3100 Southbridge")
};
