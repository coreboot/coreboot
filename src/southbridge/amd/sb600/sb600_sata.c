/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "sb600.h"

static void sata_init(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;
	u8 *sata_bar5;
	u16 sata_bar0, sata_bar1, sata_bar2, sata_bar3, sata_bar4;

	struct southbridge_ati_sb600_config *conf;
	conf = dev->chip_info;

	device_t sm_dev;
	/* SATA SMBus Disable */
	/* sm_dev = pci_locate_device(PCI_ID(0x1002, 0x4385), 0); */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	/* Disable SATA SMBUS */
	byte = pci_read_config8(sm_dev, 0xad);
	byte |= (1 << 1);
	/* Enable SATA and power saving */
	byte = pci_read_config8(sm_dev, 0xad);
	byte |= (1 << 0);
	byte |= (1 << 5);
	pci_write_config8(sm_dev, 0xad, byte);
	/* Set the interrupt Mapping to INTG# */
	byte = pci_read_config8(sm_dev, 0xaf);
	byte = 0x6 << 2;
	pci_write_config8(sm_dev, 0xaf, byte);

	/* get base addresss */
	sata_bar5 = (u8 *) (pci_read_config32(dev, 0x24) & ~0x3FF);
	sata_bar0 = pci_read_config16(dev, 0x10) & ~0x7;
	sata_bar1 = pci_read_config16(dev, 0x14) & ~0x7;
	sata_bar2 = pci_read_config16(dev, 0x18) & ~0x7;
	sata_bar3 = pci_read_config16(dev, 0x1C) & ~0x7;
	sata_bar4 = pci_read_config16(dev, 0x20) & ~0x7;

	printk_debug("sata_bar0=%x\n", sata_bar0);	/* 3030 */
	printk_debug("sata_bar1=%x\n", sata_bar1);	/* 3070 */
	printk_debug("sata_bar2=%x\n", sata_bar2);	/* 3040 */
	printk_debug("sata_bar3=%x\n", sata_bar3);	/* 3080 */
	printk_debug("sata_bar4=%x\n", sata_bar4);	/* 3000 */
	printk_debug("sata_bar5=%x\n", sata_bar5);	/* e0309000 */

	/* Program the 2C to 0x43801002 */
	dword = 0x43801002;
	pci_write_config32(dev, 0x2c, dword);

	/* SERR-Enable */
	word = pci_read_config16(dev, 0x04);
	word |= (1 << 8);
	pci_write_config16(dev, 0x04, word);

	/* Dynamic power saving */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x40, byte);

	/* Set SATA Operation Mode, Set to IDE mode */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 0);
	byte |= (1 << 4);
	pci_write_config8(dev, 0x40, byte);

	dword = 0x01018f00;
	pci_write_config32(dev, 0x8, dword);

	byte = pci_read_config8(dev, 0x40);
	byte &= ~(1 << 0);
	pci_write_config8(dev, 0x40, byte);

	/* Enable the SATA watchdog counter */
	byte = pci_read_config8(dev, 0x44);
	byte |= (1 << 0);
	pci_write_config8(dev, 0x44, byte);

	/* Program the watchdog counter to 0x10 */
	byte = 0x10;
	pci_write_config8(dev, 0x46, byte);

	/* RPR6.5 Program the PHY Global Control to 0x2C00 for A13 */
	word = 0x2c00;
	pci_write_config16(dev, 0x86, word);

	/* RPR6.5 Program the Phy Tuning4Ports */
	dword = 0x00B401D6;
	pci_write_config32(dev, 0x88, dword);
	pci_write_config32(dev, 0x8c, dword);
	pci_write_config32(dev, 0x90, dword);
	pci_write_config32(dev, 0x94, dword);

	byte = 0xB8;
	pci_write_config8(dev, 0xA5, byte);
	pci_write_config8(dev, 0xAD, byte);
	pci_write_config8(dev, 0xB5, byte);
	pci_write_config8(dev, 0xBD, byte);

	/* RPR 6.8  */
	word = pci_read_config16(dev, 0x42);
	word |= 1 << 7;
	pci_write_config16(dev, 0x42, word);
	/* RPR 6.9  */
	dword = pci_read_config32(dev, 0x40);
	dword |= 1 << 25;
	pci_write_config32(dev, 0x40, dword);

	/* Enable the I/O ,MM ,BusMaster access for SATA */
	byte = pci_read_config8(dev, 0x4);
	byte |= 7 << 0;
	pci_write_config8(dev, 0x4, byte);

	/* RPR6.6 SATA drive detection. Currently we detect Primary Master Device only */
	/* Use BAR5+0x1A8,BAR0+0x6 for Primary Slave */
	/* Use BAR5+0x228,BAR0+0x6 for Secondary Master */
	/* Use BAR5+0x2A8,BAR0+0x6 for Secondary Slave */

	byte = readb(sata_bar5 + 0x128);
	printk_debug("byte=%x\n", byte);
	byte &= 0xF;
	if (byte == 0x3) {
		outb(0xA0, sata_bar0 + 0x6);
		while ((inb(sata_bar0 + 0x6) != 0xA0)
		       || ((inb(sata_bar0 + 0x7) & 0x88) != 0)) {
			mdelay(10);
			printk_debug("0x6=%x,0x7=%x\n", inb(sata_bar0 + 0x6),
				     inb(sata_bar0 + 0x7));
			printk_debug("drive detection fail,trying...\n");
		}
		printk_debug("Primary master device is ready\n");
	} else {
		printk_debug("No Primary master SATA drive on Slot0\n");
	}

	/* Below is CIM InitSataLateFar */
	/* Enable interrupts from the HBA  */
	byte = readb(sata_bar5 + 0x4);
	byte |= 1 << 1;
	writeb(byte, (sata_bar5 + 0x4));

	/* Clear error status */
	writel(0xFFFFFFFF, (sata_bar5 + 0x130));
	writel(0xFFFFFFFF, (sata_bar5 + 0x1b0));
	writel(0xFFFFFFFF, (sata_bar5 + 0x230));
	writel(0xFFFFFFFF, (sata_bar5 + 0x2b0));

	/* Clear SATA status,Firstly we get the AcpiGpe0BlkAddr */
	/* ????? why CIM does not set the AcpiGpe0BlkAddr , but use it??? */

	/* word = 0x0000; */
	/* word = pm_ioread(0x28); */
	/* byte = pm_ioread(0x29); */
	/* word |= byte<<8; */
	/* printk_debug("AcpiGpe0Blk addr = %x\n", word); */
	/* writel(0x80000000 , word); */
}

static struct pci_operations lops_pci = {
	/* .set_subsystem = pci_dev_set_subsystem, */
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	/* .enable           = sb600_enable, */
	.init = sata_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB600_SATA,
};
