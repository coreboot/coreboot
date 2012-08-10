/*
 * This file is part of the vx900_ephy utlity.
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

#include <pci/pci.h>
#include <stddef.h>
#include <stdio.h>

#define VX900_SATA_DOM          0
#define VX900_SATA_BUS          0
#define VX900_SATA_DEV          0x0f
#define VX900_SATA_FUNC         0

#define VX900_SATA_DEV_ID       0x9001
#define VX900_SATA_VEND_ID      0x1106

typedef struct pci_dev *device_t;

typedef u8 sata_phy_config[64];

static u32 sata_phy_read32(device_t dev, u8 index)
{
	/* The SATA PHY control registers are accessed by a funny index/value
	 * scheme. Each byte (0,1,2,3) has its own 4-bit index */
	index = (index >> 2) & 0xf;
	u16 i16 = index | (index<<4) | (index<<8)| (index<<12);
	/* The index */
	pci_write_word(dev, 0x68, i16);
	/* The value */
	return pci_read_long(dev, 0x64);

}

static void vx900_sata_read_phy_config(device_t dev, sata_phy_config cfg)
{
	size_t i;
	u32* data = (u32*)cfg;
	for(i = 0; i < (sizeof(sata_phy_config) ) >> 2; i++) {
		data[i] = sata_phy_read32(dev, i<<2);
	}
}

int main()
{
	sata_phy_config ephy;
	struct pci_access *pci_lord;
	device_t dev;

	printf(" vx900_ephy tool:\n");
	printf(" Copyright (C) 2012  Alexandru Gagniuc\n");
	printf("======================================\n");

	/* libpci mambo jumbo we need to do, but don't care about */
	pci_lord = pci_alloc();
	pci_init(pci_lord);
	pci_scan_bus(pci_lord);

	/* We write to the PCI config, so stop here if it's the wrong device */
	dev = pci_get_dev(pci_lord, VX900_SATA_DOM, VX900_SATA_BUS,
				    VX900_SATA_DEV, VX900_SATA_FUNC);

	pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if( (VX900_SATA_VEND_ID != dev->vendor_id) ||
		(VX900_SATA_DEV_ID != dev->device_id) )
	{
		printf("VX900 SATA controller not found\n");
		return -1;

	}
	/* Get all the info in one pass */
	vx900_sata_read_phy_config(dev, ephy);

	/* Put it on the terminal for the user to read and be done with it */
	printf("SATA PHY config:\n");
	int i;
	for (i = 0; i < sizeof(sata_phy_config); i++) {
		if ((i & 0x0f) == 0) {
			printf("%.2x :", i);
		}
		if( (i & 0x0f) == 0x08 )
			printf("| ");
		printf("%.2x ", ephy[i]);
		if ((i & 0x0f) == 0x0f) {
			printf("\n");
		}
	}

	printf("Compare that with the coreboot debug output, and see if the"
	       " EPHY values need adjustment for your mainboard.\n");
}