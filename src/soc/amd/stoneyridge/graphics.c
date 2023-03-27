/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_rom.h>
#include <stdint.h>

/* Change the vendor / device IDs to match the generic VBIOS header. */
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev;

	if ((vendev >= 0x100298e0) && (vendev <= 0x100298ef))
		new_vendev = 0x100298e0;
	else if ((vendev >= 0x10029870) && (vendev <= 0x1002987f))
		new_vendev = 0x10029870;
	else
		new_vendev = vendev;

	if (vendev != new_vendev)
		printk(BIOS_NOTICE, "Mapping PCI device %8x to %8x\n", vendev, new_vendev);

	return new_vendev;
}
