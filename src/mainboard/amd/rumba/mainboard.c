/*
 * This file is part of the coreboot project.
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
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>

static void init(struct device *dev)
{
	device_t nic = NULL;
	unsigned bus = 0;
	unsigned devfn = PCI_DEVFN(0xd, 0);
	int nicirq = 1;

	printk(BIOS_DEBUG, "AMD RUMBA ENTER %s\n", __func__);

	if (nicirq) {
		printk(BIOS_DEBUG, "%s (%x,%x)SET PCI interrupt line to %d\n",
			__func__, bus, devfn, nicirq);
		nic = dev_find_slot(bus, devfn);
		if (! nic){
			printk(BIOS_ERR, "Could not find NIC\n");
		} else {
			pci_write_config8(nic, PCI_INTERRUPT_LINE, nicirq);
		}
	}
	printk(BIOS_DEBUG, "AMD RUMBA EXIT %s\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
