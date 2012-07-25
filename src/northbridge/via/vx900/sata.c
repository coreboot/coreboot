/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "vx900.h"


static void dump_pci_device(device_t dev)
{
	int i;
	for (i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		if((i & 7) == 0) print_debug(" |");
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static void vx900_native_sata_mode(device_t dev)
{
	/* Disable subclass write protect */
	pci_mod_config8(dev, 0x45, 1<<7, 0);
	/* Change the device class to SATA */
	pci_write_config8(dev, PCI_CLASS_DEVICE, 0x01);
	/* Re-enable subclass write protect */
	pci_mod_config8(dev, 0x45, 0, 1<<7);
	/* */
	pci_write_config8(dev, PCI_CLASS_PROG, 0x8f);
}

static void vx900_sata_init(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== SATA init \n");
	print_debug("======================================================\n");

	/* Enable SATA primary channel IO access */
	//pci_mod_config8(dev, 0x40, 0, 1<<1);
	/* Just SATA, so it makes sense to be in native SATA mode */
	vx900_native_sata_mode(dev);

	/* Fix "PMP Device Can’t Detect HDD Normally" (VIA Porting Guide)
	 * SATA device detection will not work unless we clear these bits.
	 * Without doing this, SeaBIOS (and potentially other payloads) will
	 * timeout when detecting SATA devices */
	pci_mod_config8(dev, 0x89, (1<<3) | (1<<6), 0);

	/* 12.7 Two Software Resets May Affect the System
	 * When the software does the second reset before the first reset
	 * finishes, it may cause the system hang. It would do one software
	 * reset and check the BSY bit of one port only, and the BSY bit of
	 * other port would be 1, then it does another software reset
	 * immediately and causes the system hang.
	 * This is because the first software reset doesn’t finish, and the
	 * state machine of the host controller conflicts, it can’t finish the
	 * second one anymore. The BSY bit of slave port would be always 1 after
	 * the second software reset issues. BIOS should set the following
	 * bit to avoid this issue. */
	pci_mod_config8(dev, 0x80, 0, 1<<6);

	/* Spin-up SATA devices
	 * TODO: Move to early ramstage to save time? */
	pci_mod_config8(dev, 0xb9, 0, 3<<4);

	print_debug("And finally, the dump you've all been waiting for\n");
	dump_pci_device(dev);
}

static struct device_operations vga_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vx900_sata_init,
};

static const struct pci_driver vga_driver_900 __pci_driver = {
	.ops = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_SATA,
};