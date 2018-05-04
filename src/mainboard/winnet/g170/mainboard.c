/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
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
#include <arch/acpi.h>

static void mainboard_final(void *chip_info)
{
	                                       /*  A  B   C  D */
	static const unsigned char irq_map[4] = { 11, 5, 10, 0 };
	struct device *lpc_dev;

	printk(BIOS_INFO, "Setting up G170 IRQ routing...\n");

	lpc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
	                          PCI_DEVICE_ID_VIA_VT8237R_LPC, 0);

	/* Disable APIC */
	pci_write_config8(lpc_dev, 0x58, pci_read_config8(lpc_dev, 0x58) & ~0x40);

	/* Share INTE-INTH with INTA-INTD */
	pci_write_config8(lpc_dev, 0x46, 0x00);

	/* Route INTA-INTD */
	pci_write_config8(lpc_dev, 0x55, irq_map[0] << 4);
	pci_write_config8(lpc_dev, 0x56, irq_map[1] << 4 | irq_map[2]);
	pci_write_config8(lpc_dev, 0x57, irq_map[3]);

	/* Assign IRQ numbers to known devices for non-PnP OSes */
	pci_assign_irqs(0x00, 0x10, irq_map);
	pci_assign_irqs(0x00, 0x11, irq_map);
	pci_assign_irqs(0x00, 0x12, irq_map);
	pci_assign_irqs(0x01, 0x00, irq_map);
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("WinNET G170")
	.final = mainboard_final,
};
