/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Corey Osgood <corey.osgood@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <console.h>
#include <device/pci.h>
#include <config.h>
#include "cn700.h"

/**
 * Configure the bus between the cpu and the northbridge. This might be able to 
 * be moved to post-ram code in the future. For the most part, these registers
 * should not be messed around with. These are too complex to explain short of
 * copying the datasheets into the comments, but most of these values are from
 * the BIOS Porting Guide, so they should work on any board. If they don't,
 * try the values from your factory BIOS.
 *
 * TODO: Changing the DRAM frequency doesn't work (hard lockup)
 *
 * @param dev The northbridge's CPU Host Interface (D0F2)
 */
static void c7_cpu_setup(void)
{
	u32 dev = PCI_BDF(0, 0, 2);
	u8 reg8;

	/* Host bus interface registers (D0F2 0x50-0x67) */
	/* Request phase control */
	pci_conf1_write_config8(dev, 0x50, 0x88);
	/* CPU Interface Control */
	pci_conf1_write_config8(dev, 0x51, 0x7a);
	pci_conf1_write_config8(dev, 0x52, 0x6f);
	/* Arbitration */
	pci_conf1_write_config8(dev, 0x53, 0x88);
	/* Miscellaneous Control */
	pci_conf1_write_config8(dev, 0x54, 0x1e);
	pci_conf1_write_config8(dev, 0x55, 0x16);
	/* Write Policy */
	pci_conf1_write_config8(dev, 0x56, 0x01);
	/* Miscellaneous Control */
	/* DRAM Operating Frequency (bits 7:5 Rx57)
	 *      000 : 100MHz    001 : 133MHz
	 *      010 : 166MHz    011 : 200MHz
	 *      100 : 266MHz    101 : 333MHz
	 *      110/111 : Reserved
	 * bits 4:0: Reserved
	 */

	reg8 = pci_conf1_read_config8(dev, 0x57);
	reg8 &= (0x7 << 5);
	//reg8 |= (0x4 << 5);
	reg8 |= (0x3 << 5);
	pci_conf1_write_config8(dev, 0x57, reg8);
	
	/* CPU Miscellaneous Control */
	pci_conf1_write_config8(dev, 0x59, 0x44);
	/* Write Policy */
	pci_conf1_write_config8(dev, 0x5d, 0xb2);
	/* Bandwidth Timer */
	pci_conf1_write_config8(dev, 0x5e, 0x88);
	/* CPU Miscellaneous Control */
	pci_conf1_write_config8(dev, 0x5f, 0xc7);

	/* Line DRDY# Timing Control */
	pci_conf1_write_config8(dev, 0x60, 0xff);
	pci_conf1_write_config8(dev, 0x61, 0xff);
	pci_conf1_write_config8(dev, 0x62, 0x0f);
	/* QW DRDY# Timing Control */
	pci_conf1_write_config8(dev, 0x63, 0xff);
	pci_conf1_write_config8(dev, 0x64, 0xff);
	pci_conf1_write_config8(dev, 0x65, 0x0f);
	/* Read Line Burst DRDY# Timing Control */
	pci_conf1_write_config8(dev, 0x66, 0xff);
	pci_conf1_write_config8(dev, 0x67, 0x30);

	/* Host Bus I/O Circuit (see datasheet) */
	/* Host Address Pullup/down Driving */
	pci_conf1_write_config8(dev, 0x70, 0x11);
	pci_conf1_write_config8(dev, 0x71, 0x11);
	pci_conf1_write_config8(dev, 0x72, 0x11);
	pci_conf1_write_config8(dev, 0x73, 0x11);
	/* Miscellaneous Control */
	pci_conf1_write_config8(dev, 0x74, 0x35);
	/* AGTL+ I/O Circuit */
	pci_conf1_write_config8(dev, 0x75, 0x28);
	/* AGTL+ Compensation Status */
	pci_conf1_write_config8(dev, 0x76, 0x74);
	/* AGTL+ Auto Compensation Offest */
	pci_conf1_write_config8(dev, 0x77, 0x00);
	/* Host FSB CKG Control */
	pci_conf1_write_config8(dev, 0x78, 0x0a);
	/* Address/Address Clock Output Delay Control */
	pci_conf1_write_config8(dev, 0x79, 0xaa);
	/* Address Strobe Input Delay Control */
	pci_conf1_write_config8(dev, 0x7a, 0x24);
	/* Address CKG Rising/Falling Time Control */
	pci_conf1_write_config8(dev, 0x7b, 0xaa);
	/* Address CKG Clock Rising/Falling Time Control */
	pci_conf1_write_config8(dev, 0x7c, 0x00);
	/* Undefined (can't remember why I did this) */
	pci_conf1_write_config8(dev, 0x7d, 0x6d);

	pci_conf1_write_config8(dev, 0x7e, 0x00);
	pci_conf1_write_config8(dev, 0x7f, 0x00);
	pci_conf1_write_config8(dev, 0x80, 0x1b);
	pci_conf1_write_config8(dev, 0x81, 0x0a);
	pci_conf1_write_config8(dev, 0x82, 0x0a);
	pci_conf1_write_config8(dev, 0x83, 0x0a);
}

void cn700_stage1(void)
{
	/* Enable multifunction for northbridge. */
	pci_conf1_write_config8(0x00, 0x4f, 0x01);

	/* Put Bus 1 in its proper place */
	pci_conf1_write_config8(PCI_BDF(0, 1, 0), 0x19, 0x1);
	pci_conf1_write_config8(PCI_BDF(0, 1, 0), 0x1a, 0x1);

	c7_cpu_setup();
}
