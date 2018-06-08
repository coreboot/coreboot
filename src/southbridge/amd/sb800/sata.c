/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "sb800.h"

static int sata_drive_detect(int portnum, u16 iobar)
{
	u8 byte, byte2;
	int i = 0;
	outb(0xA0 + 0x10 * (portnum % 2), iobar + 0x6);
	while (byte = inb(iobar + 0x6), byte2 = inb(iobar + 0x7),
		(byte != (0xA0 + 0x10 * (portnum % 2))) ||
		((byte2 & 0x88) != 0)) {
		printk(BIOS_SPEW, "0x6=%x, 0x7=%x\n", byte, byte2);
		if (byte != (0xA0 + 0x10 * (portnum % 2))) {
			/* This will happen at the first iteration of this loop
			 * if the first SATA port is unpopulated and the
			 * second SATA port is populated.
			 */
			printk(BIOS_DEBUG, "drive no longer selected after %i ms, "
				"retrying init\n", i * 10);
			return 1;
		} else
			printk(BIOS_SPEW, "drive detection not yet completed, "
				"waiting...\n");
		mdelay(10);
		i++;
	}
	printk(BIOS_SPEW, "drive detection done after %i ms\n", i * 10);
	return 0;
}

static void sb800_setup_sata_phys(struct device *dev)
{
	int i;
	static const u32 sata_phy[][3] = {
		{0x0056A607, 0x00061400, 0x00061302}, /* port 0 */
		{0x0056A607, 0x00061400, 0x00061302}, /* port 1 */
		{0x0056A607, 0x00061402, 0x00064300}, /* port 2 */
		{0x0056A607, 0x00061402, 0x00064300}, /* port 3 */
		{0x0056A700, 0x00061502, 0x00064302}, /* port 4 */
		{0x0056A700, 0x00061502, 0x00064302}  /* port 5 */
	};
	/* RPR8.4 */
	/* Port 0 - 5 */
	for (i = 0; i < 6; i++) {
		pci_write_config16(dev, 0x84, 0x3006 | i << 9);
		pci_write_config32(dev, 0x94, sata_phy[i][0]); /* Gen 3 */
		pci_write_config16(dev, 0x84, 0x2006 | i << 9);
		pci_write_config32(dev, 0x94, sata_phy[i][1]); /* Gen 2 */
		pci_write_config16(dev, 0x84, 0x1006 | i << 9);
		pci_write_config32(dev, 0x94, sata_phy[i][2]); /* Gen 1 */
	}

}
static void sata_init(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;
	u8 rev_id;
	void *sata_bar5;
	u16 sata_bar0, sata_bar1, sata_bar2, sata_bar3, sata_bar4;
	int i, j;

	struct southbridge_ati_sb800_config *conf;
	conf = dev->chip_info;

	struct device *sm_dev;
	/* SATA SMBus Disable */
	/* sm_dev = pci_locate_device(PCI_ID(0x1002, 0x4385), 0); */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	/* get rev_id */
	rev_id = pci_read_config8(sm_dev, 0x08) - 0x2F;

	/* get base address */
	sata_bar5 = (void *)(pci_read_config32(dev, 0x24) & ~0x3FF);
	sata_bar0 = pci_read_config16(dev, 0x10) & ~0x7;
	sata_bar1 = pci_read_config16(dev, 0x14) & ~0x3;
	sata_bar2 = pci_read_config16(dev, 0x18) & ~0x7;
	sata_bar3 = pci_read_config16(dev, 0x1C) & ~0x3;
	sata_bar4 = pci_read_config16(dev, 0x20) & ~0xf;

	printk(BIOS_SPEW, "sata_bar0=%x\n", sata_bar0);	/* 3030 */
	printk(BIOS_SPEW, "sata_bar1=%x\n", sata_bar1);	/* 3070 */
	printk(BIOS_SPEW, "sata_bar2=%x\n", sata_bar2);	/* 3040 */
	printk(BIOS_SPEW, "sata_bar3=%x\n", sata_bar3);	/* 3080 */
	printk(BIOS_SPEW, "sata_bar4=%x\n", sata_bar4);	/* 3000 */
	printk(BIOS_SPEW, "sata_bar5=%p\n", sata_bar5);	/* e0309000 */

	/* SERR-Enable */
	word = pci_read_config16(dev, 0x04);
	word |= (1 << 8);
	pci_write_config16(dev, 0x04, word);

	/* Set SATA Operation Mode, Set to IDE mode */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 0);
	//byte |= (1 << 4);
	pci_write_config8(dev, 0x40, byte);

	dword = 0x01018f00;
	pci_write_config32(dev, 0x8, dword);

	/* Program the 2C to 0x43801002 */
	dword = 0x43801002;
	pci_write_config32(dev, 0x2c, dword);

	pci_write_config8(dev, 0x34, 0x70); /* 8.11 SATA MSI and D3 Power State Capability */

	dword = read32(sata_bar5 + 0xFC);
	dword &= ~(1 << 11);	/* rpr 8.8. Disabling Aggressive Link Power Management */
	dword &= ~(1 << 12);	/* rpr 8.9.1 Disabling Port Multiplier support. */
	dword &= ~(1 << 10);	/* rpr 8.9.2 disabling FIS-based Switching support */
	dword &= ~(1 << 19);	/* rpr 8.10. Disabling CCC (Command Completion Coalescing) Support */
	write32((sata_bar5 + 0xFC), dword);

	dword = read32(sata_bar5 + 0xF8);
	dword &= ~(0x3F << 22);	/* rpr 8.9.2 disabling FIS-based Switching support */
	write32(sata_bar5 + 0xF8, dword);

	byte = pci_read_config8(dev, 0x40);
	byte &= ~(1 << 0);
	pci_write_config8(dev, 0x40, byte);

	/* rpr 8.3 */
	printk(BIOS_SPEW, "rev_id=%x\n", rev_id);
	dword = pci_read_config32(dev, 0x84);
	if (rev_id == 0x11)	/* A11 */
		dword |= 1 << 22;
	pci_write_config32(dev, 0x84, dword);

	/* rpr8.12 Program the watchdog counter to 0x20 */
	byte = pci_read_config8(dev, 0x44);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x44, byte);

	pci_write_config8(dev, 0x46, 0x20);

	sb800_setup_sata_phys(dev);
	/* Enable the I/O, MM, BusMaster access for SATA */
	byte = pci_read_config8(dev, 0x4);
	byte |= 7 << 0;
	pci_write_config8(dev, 0x4, byte);

	/* RPR7.7 SATA drive detection. */
	/* Use BAR5+0x128,BAR0 for Primary Slave */
	/* Use BAR5+0x1A8,BAR0 for Primary Slave */
	/* Use BAR5+0x228,BAR2 for Secondary Master */
	/* Use BAR5+0x2A8,BAR2 for Secondary Slave */
	/* Use BAR5+0x328,PATA_BAR0/2 for Primary/Secondary master emulation */
	/* Use BAR5+0x3A8,PATA_BAR0/2 for Primary/Secondary Slave emulation */

	/* TODO: port 4,5, which are PATA emulations. What are PATA_BARs? */

	for (i = 0; i < 4; i++) {
		byte = read8(sata_bar5 + 0x128 + 0x80 * i);
		printk(BIOS_SPEW, "SATA port %i status = %x\n", i, byte);
		byte &= 0xF;
		if ( byte == 0x1 ) {
			/* If the drive status is 0x1 then we see it but we aren't talking to it. */
			/* Try to do something about it. */
			printk(BIOS_SPEW, "SATA device detected but not talking. Trying lower speed.\n");

			/* Read in Port-N Serial ATA Control Register */
			byte = read8(sata_bar5 + 0x12C + 0x80 * i);

			/* Set Reset Bit and 1.5g bit */
			byte |= 0x11;
			write8((sata_bar5 + 0x12C + 0x80 * i), byte);

			/* Wait 1ms */
			mdelay(1);

			/* Clear Reset Bit */
			byte &= ~0x01;
			write8((sata_bar5 + 0x12C + 0x80 * i), byte);

			/* Wait 1ms */
			mdelay(1);

			/* Reread status */
			byte = read8(sata_bar5 + 0x128 + 0x80 * i);
			printk(BIOS_SPEW, "SATA port %i status = %x\n", i, byte);
			byte &= 0xF;
		}

		if (byte == 0x3) {
			for (j = 0; j < 10; j++) {
				if (!sata_drive_detect(i, ((i / 2) == 0) ? sata_bar0 : sata_bar2))
					break;
			}
			printk(BIOS_DEBUG, "%s %s device is %sready after %i tries\n",
					(i / 2) ? "Secondary" : "Primary",
					(i % 2 ) ? "Slave" : "Master",
					(j == 10) ? "not " : "",
					(j == 10) ? j : j + 1);
		} else {
			printk(BIOS_DEBUG, "No %s %s SATA drive on Slot%i\n",
					(i / 2) ? "Secondary" : "Primary",
					(i % 2 ) ? "Slave" : "Master", i);
		}
	}

	/* Below is CIM InitSataLateFar */
	/* Enable interrupts from the HBA  */
	byte = read8(sata_bar5 + 0x4);
	byte |= 1 << 1;
	write8((sata_bar5 + 0x4), byte);

	/* Clear error status */
	write32((sata_bar5 + 0x130), 0xFFFFFFFF);
	write32((sata_bar5 + 0x1b0), 0xFFFFFFFF);
	write32((sata_bar5 + 0x230), 0xFFFFFFFF);
	write32((sata_bar5 + 0x2b0), 0xFFFFFFFF);
	write32((sata_bar5 + 0x330), 0xFFFFFFFF);
	write32((sata_bar5 + 0x3b0), 0xFFFFFFFF);

	/* Clear SATA status,Firstly we get the AcpiGpe0BlkAddr */
	/* ????? why CIM does not set the AcpiGpe0BlkAddr , but use it??? */

	/* word = 0x0000; */
	/* word = pm_ioread(0x28); */
	/* byte = pm_ioread(0x29); */
	/* word |= byte<<8; */
	/* printk(BIOS_DEBUG, "AcpiGpe0Blk addr = %x\n", word); */
	/* write32(word, 0x80000000); */
}

static struct pci_operations lops_pci = {
	/* .set_subsystem = pci_dev_set_subsystem, */
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA,
};
