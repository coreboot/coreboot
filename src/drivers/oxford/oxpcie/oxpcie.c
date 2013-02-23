/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc
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

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <arch/io.h>

static void oxford_oxpcie_enable(device_t dev)
{
	printk(BIOS_DEBUG, "Initializing Oxford OXPCIe952\n");

	struct resource *res = find_resource(dev, 0x10);
	if (!res) {
		printk(BIOS_WARNING, "OXPCIe952: No UART resource found.\n");
		return;
	}

	printk(BIOS_DEBUG, "OXPCIe952: Class=%x Revision ID=%x\n",
			(read32(res->base) >> 8), (read32(res->base) & 0xff));
	printk(BIOS_DEBUG, "OXPCIe952: %d UARTs detected.\n",
			(read32(res->base + 4) & 3));
	printk(BIOS_DEBUG, "OXPCIe952: UART BAR: 0x%x\n", (u32)res->base);
}


static void oxford_oxpcie_set_resources(struct device *dev)
{
	pci_dev_set_resources(dev);

#if CONFIG_CONSOLE_SERIAL8250MEM
	/* Re-initialize OXPCIe base address after set_resources */
	uartmem_init();
#endif
}

static struct device_operations oxford_oxpcie_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = oxford_oxpcie_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = oxford_oxpcie_enable,
	.scan_bus         = 0,
};

static const struct pci_driver oxford_oxpcie_driver __pci_driver = {
	.ops    = &oxford_oxpcie_ops,
	.vendor = 0x1415,
	.device = 0xc158,
};

static const struct pci_driver oxford_oxpcie_driver_2 __pci_driver = {
	.ops    = &oxford_oxpcie_ops,
	.vendor = 0x1415,
	.device = 0xc11b,
};
