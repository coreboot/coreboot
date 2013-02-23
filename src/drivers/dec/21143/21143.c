/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Marc Bertens <mbertens@xs4all.nl>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>

static void dec_21143_enable(device_t dev)
{
	printk(BIOS_DEBUG, "Initializing DECchip 21143\n");

	// The resource allocator should do this. If not, it needs to be fixed
	// differently.
#if 0
	/* Command and status configuration (offset 0x04) */
	pci_write_config32(dev, 0x04, 0x02800107);
	printk(BIOS_DEBUG, "0x04 = %08x (07 01 80 02)\n",
	       pci_read_config32(dev, 0x04));

	/* Cache line size (offset 0x0C) */
	pci_write_config8(dev, 0x0C, 0x00);
	printk(BIOS_DEBUG, "0x0c = %08x (00)\n",
	       pci_read_config8(dev, 0x0C));
#endif
}

static struct device_operations dec_21143_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = dec_21143_enable,
	.scan_bus         = 0,
};

static const struct pci_driver dec_21143_driver __pci_driver = {
	.ops    = &dec_21143_ops,
	.vendor = PCI_VENDOR_ID_DEC,
	.device = PCI_DEVICE_ID_DEC_21142, // FIXME wrong ID?
};
