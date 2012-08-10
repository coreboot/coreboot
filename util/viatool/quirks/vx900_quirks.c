/*
 * viatool - dump all registers on a VIA CPU + chipset based system.
 *
 * Copyright (C) 2013 Alexandru Gagniuc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * a long with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quirks.h"

#include <stdio.h>

typedef u8 sata_phy_config[64];

static u32 sata_phy_read32(struct pci_dev *dev, u8 index)
{
	/* The SATA PHY control registers are accessed by a funny index/value
	 * scheme. Each byte (0,1,2,3) has its own 4-bit index */
	index = (index >> 2) & 0xf;
	u16 i16 = index | (index << 4) | (index << 8)| (index << 12);
	/* The index */
	pci_write_word(dev, 0x68, i16);
	/* The value */
	return pci_read_long(dev, 0x64);
}

static void vx900_sata_read_phy_config(struct pci_dev *dev, sata_phy_config cfg)
{
	size_t i;
	u32* data = (u32*)cfg;
	for (i = 0; i < ( sizeof(sata_phy_config) ) >> 2; i++) {
		data[i] = sata_phy_read32(dev, i<<2);
	}
}

static int quirk_vx900_sata(struct pci_dev *dev)
{
	sata_phy_config ephy;

	/* Get all the info in one pass */
	vx900_sata_read_phy_config(dev, ephy);

	/* Put it on the terminal for the user to read and be done with it */
	printf("SATA PHY config:\n");
	unsigned int i;
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
	return 0;
}




static struct quirk vx900_sb_quirks[] = {
	{0, 0, 0x0f, 0, PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX900_SATA,
		quirk_vx900_sata },
	{0, 0, 0, 0, 0, 0, 0},
};

struct quirk_list vx900_sb_quirk_list = {
	.pci_vendor_id = PCI_VENDOR_ID_VIA,
	.pci_device_id = PCI_DEVICE_ID_VIA_VX900_LPC,
	.dev_quirks = vx900_sb_quirks
};