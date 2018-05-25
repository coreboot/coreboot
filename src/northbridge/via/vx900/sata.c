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
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "vx900.h"

/**
 * @file vx900/sata.c
 *
 * STATUS: Pretty good
 * The only issue is the SATA EPHY configuration. We do not know if it is board
 * specific or not. Otherwise, the SATA controller works without issues.
 */

static void vx900_print_sata_errors(u32 flags)
{
	/* Status flags */
	printk(BIOS_DEBUG, "\tPhyRdy %s\n",
	       (flags & (1 << 16)) ? "changed" : "not changed");
	printk(BIOS_DEBUG, "\tCOMWAKE %s\n",
	       (flags & (1 << 16)) ? "detected" : "not detected");
	printk(BIOS_DEBUG, "\tExchange as determined by COMINIT %s\n",
	       (flags & (1 << 26)) ? "occurred" : "not occurred");
	printk(BIOS_DEBUG, "\tPort selector presence %s\n",
	       (flags & (1 << 27)) ? "detected" : "not detected");
	/* Errors */
	if (flags & (1 << 0))
		printk(BIOS_DEBUG, "\tRecovered data integrity ERROR\n");
	if (flags & (1 << 1))
		printk(BIOS_DEBUG, "\tRecovered data communication ERROR\n");
	if (flags & (1 << 8))
		printk(BIOS_DEBUG, "\tNon-recovered Transient Data Integrity ERROR\n");
	if (flags & (1 << 9))
		printk(BIOS_DEBUG, "\tNon-recovered Persistent Communication or"
			    "\tData Integrity ERROR\n");
	if (flags & (1 << 10))
		printk(BIOS_DEBUG, "\tProtocol ERROR\n");
	if (flags & (1 << 11))
		printk(BIOS_DEBUG, "\tInternal ERROR\n");
	if (flags & (1 << 17))
		printk(BIOS_DEBUG, "\tPHY Internal ERROR\n");
	if (flags & (1 << 19))
		printk(BIOS_DEBUG, "\t10B to 8B Decode ERROR\n");
	if (flags & (1 << 20))
		printk(BIOS_DEBUG, "\tDisparity ERROR\n");
	if (flags & (1 << 21))
		printk(BIOS_DEBUG, "\tCRC ERROR\n");
	if (flags & (1 << 22))
		printk(BIOS_DEBUG, "\tHandshake ERROR\n");
	if (flags & (1 << 23))
		printk(BIOS_DEBUG, "\tLink Sequence ERROR\n");
	if (flags & (1 << 24))
		printk(BIOS_DEBUG, "\tTransport State Transition ERROR\n");
	if (flags & (1 << 25))
		printk(BIOS_DEBUG, "\tUNRECOGNIZED FIS type\n");
}

static void vx900_dbg_sata_errors(struct device *dev)
{
	/* Port 0 */
	if (pci_read_config8(dev, 0xa0) & (1 << 0)) {
		printk(BIOS_DEBUG, "Device detected in SATA port 0.\n");
		u32 flags = pci_read_config32(dev, 0xa8);
		vx900_print_sata_errors(flags);
	};
	/* Port 1 */
	if (pci_read_config8(dev, 0xa1) & (1 << 0)) {
		printk(BIOS_DEBUG, "Device detected in SATA port 1.\n");
		u32 flags = pci_read_config32(dev, 0xac);
		vx900_print_sata_errors(flags);
	};
}

typedef u8 sata_phy_config[64];

static sata_phy_config reference_ephy = {
	0x80, 0xb8, 0xf0, 0xfe, 0x40, 0x7e, 0xf6, 0xdd,
	0x1a, 0x22, 0xa0, 0x10, 0x02, 0xa9, 0x7c, 0x7e,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x30, 0x84, 0x8c,
	0x75, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x40, 0xd0, 0x41, 0x40, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x40, 0x50, 0x41, 0x40, 0x00, 0x00, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static u32 sata_phy_read32(struct device *dev, u8 index)
{
	/* The SATA PHY control registers are accessed by a funny index/value
	 * scheme. Each byte (0,1,2,3) has its own 4-bit index */
	index = (index >> 2) & 0xf;
	u16 i16 = index | (index << 4) | (index << 8) | (index << 12);
	/* The index */
	pci_write_config16(dev, 0x68, i16);
	/* The value */
	return pci_read_config32(dev, 0x64);
}

static void sata_phy_write32(struct device *dev, u8 index, u32 val)
{
	/* The SATA PHY control registers are accessed by a funny index/value
	 * scheme. Each byte (0,1,2,3) has its own 4-bit index */
	index = (index >> 2) & 0xf;
	u16 i16 = index | (index << 4) | (index << 8) | (index << 12);
	/* The index */
	pci_write_config16(dev, 0x68, i16);
	/* The value */
	pci_write_config32(dev, 0x64, val);
}

static void vx900_sata_read_phy_config(struct device *dev, sata_phy_config cfg)
{
	size_t i;
	u32 *data = (u32 *) cfg;
	for (i = 0; i < (sizeof(sata_phy_config)) >> 2; i++) {
		data[i] = sata_phy_read32(dev, i << 2);
	}
}

static void vx900_sata_write_phy_config(struct device *dev, sata_phy_config cfg)
{
	size_t i;
	u32 *data = (u32 *) cfg;
	for (i = 0; i < (sizeof(sata_phy_config)) >> 2; i++) {
		sata_phy_write32(dev, i << 2, data[i]);
	}
}

static void vx900_sata_dump_phy_config(sata_phy_config cfg)
{
	printk(BIOS_DEBUG, "SATA PHY config:\n");
	int i;
	for (i = 0; i < sizeof(sata_phy_config); i++) {
		unsigned char val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%02x:", i);
		val = cfg[i];
		if ((i & 7) == 0)
			printk(BIOS_DEBUG, " |");
		printk(BIOS_DEBUG, " %02x", val);
		if ((i & 0x0f) == 0x0f) {
			printk(BIOS_DEBUG, "\n");
		}
	}
}

/**
 * \brief VX900: Place the onboard SATA controller in Native IDE mode
 *
 * AHCI mode requires a sub-class of 0x06, and Interface of 0x0
 * SATA mode requires a sub-class of 0x06, and Interface of 0x00
 * Unfortunately, setting the class to SATA, will prevent us from modyfing the
 * interface register to an AHCI/SATA compliant value. Thus, payloads or OS may
 * not properly identify this as a SATA controller.
 * We could set the class code to 0x04, which would cause the interface register
 * to become 0x00, which represents a RAID controller. Unfortunately, when we do
 * this, SeaBIOS will skip this as a storage device, and we will not be able to
 * boot.
 * Our only option is to operate in IDE mode. We choose native IDE so that we
 * can freely assign an IRQ, and are not forced to use IRQ14
 */
static void vx900_native_ide_mode(struct device *dev)
{
	/* Disable subclass write protect */
	pci_mod_config8(dev, 0x45, 1 << 7, 0);
	/* Change the device class to IDE */
	pci_write_config16(dev, PCI_CLASS_DEVICE, PCI_CLASS_STORAGE_IDE);
	/* Re-enable subclass write protect */
	pci_mod_config8(dev, 0x45, 0, 1 << 7);
	/* Put it in native IDE mode */
	pci_write_config8(dev, PCI_CLASS_PROG, 0x8f);
}

static void vx900_sata_init(struct device *dev)
{
	/* Enable SATA primary channel IO access */
	pci_mod_config8(dev, 0x40, 0, 1 << 1);
	/* Just SATA, so it makes sense to be in native SATA mode */
	vx900_native_ide_mode(dev);

	/* TP Layer Idle at least 20us before the Following Command */
	pci_mod_config8(dev, 0x53, 0, 1 << 7);
	/* Resend COMRESET When Recovering SATA Gen2 Device Error */
	pci_mod_config8(dev, 0x62, 1 << 1, 1 << 7);

	/* Fix "PMP Device Can't Detect HDD Normally" (VIA Porting Guide)
	 * SATA device detection will not work unless we clear these bits.
	 * Without doing this, SeaBIOS (and potentially other payloads) will
	 * timeout when detecting SATA devices */
	pci_mod_config8(dev, 0x89, (1 << 3) | (1 << 6), 0);

	/* 12.7 Two Software Resets May Affect the System
	 * When the software does the second reset before the first reset
	 * finishes, it may cause the system hang. It would do one software
	 * reset and check the BSY bit of one port only, and the BSY bit of
	 * other port would be 1, then it does another software reset
	 * immediately and causes the system hang.
	 * This is because the first software reset doesn't finish, and the
	 * state machine of the host controller conflicts, it can't finish the
	 * second one anymore. The BSY bit of slave port would be always 1 after
	 * the second software reset issues. BIOS should set the following
	 * bit to avoid this issue. */
	pci_mod_config8(dev, 0x80, 0, 1 << 6);

	/* We need to set the EPHY values before doing anything with the link */
	sata_phy_config ephy;
	vx900_sata_read_phy_config(dev, ephy);
	if (1) {
		vx900_sata_dump_phy_config(ephy);
		vx900_sata_write_phy_config(dev, reference_ephy);
	} else {
		/* Enable TX and RX driving resistance */
		/* TX - 50 Ohm */
		ephy[1] &= ~(0x1f << 3);
		ephy[1] |= (1 << 7) | (8 << 3);
		/* RX - 50 Ohm */
		ephy[2] &= ~(0x1f << 3);
		ephy[2] |= (1 << 7) | (8 << 3);
		vx900_sata_write_phy_config(dev, ephy);
	}

	vx900_sata_read_phy_config(dev, ephy);
	vx900_sata_dump_phy_config(ephy);

	/* Clear error flags */
	pci_write_config32(dev, 0xa8, 0xffffffff);
	pci_write_config32(dev, 0xac, 0xffffffff);

	/* Start OOB link negotiation sequence */
	pci_mod_config8(dev, 0xb9, 0, 3 << 4);

	/* FIXME: From now on, we are just doing DEBUG stuff
	 * Wait until PHY communication is enabled */
	u32 wloops = 0;
	while (!(pci_read_config8(dev, 0xa0) & (1 << 1)))
		wloops++;
	printk(BIOS_SPEW, "SATA wait loops: %u\n", wloops);

	vx900_dbg_sata_errors(dev);
}

static void vx900_sata_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);
}

static struct device_operations vga_operations = {
	.read_resources = vx900_sata_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vx900_sata_init,
};

static const struct pci_driver chrome9hd_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_SATA,
};
