/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_rom.h>
#include <stdint.h>

#define ST_GPU_FIRST_VID_DID	0x100298e0
#define ST_GPU_LAST_VID_DID	0x100298ef
#define ST_VBIOS_VID_DID	0x100298e0
#define CZ_GPU_FIRST_VID_DID	0x10029870
#define CZ_GPU_LAST_VID_DID	0x1002987f
#define CZ_VBIOS_VID_DID	0x10029870

/* Change the vendor / device IDs to match the generic VBIOS header. */
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev;

	if ((vendev >= ST_GPU_FIRST_VID_DID) && (vendev <= ST_GPU_LAST_VID_DID))
		new_vendev = ST_VBIOS_VID_DID;
	else if ((vendev >= CZ_GPU_FIRST_VID_DID) && (vendev <= CZ_GPU_LAST_VID_DID))
		new_vendev = CZ_VBIOS_VID_DID;
	else
		new_vendev = vendev;

	if (vendev != new_vendev)
		printk(BIOS_NOTICE, "Mapping PCI device %8x to %8x\n", vendev, new_vendev);

	return new_vendev;
}
