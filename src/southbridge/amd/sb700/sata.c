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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "sb700.h"

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
			 * second SATA port is poulated.
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

/* This function can be overloaded in mainboard.c */
void __attribute__((weak)) sb7xx_51xx_setup_sata_phys(struct device *dev)
{
	/* RPR7.6.1 Program the PHY Global Control to 0x2C00 */
	pci_write_config16(dev, 0x86, 0x2c00);

	/* RPR7.6.2 SATA GENI PHY ports setting */
	pci_write_config32(dev, 0x88, 0x01B48017);
	pci_write_config32(dev, 0x8c, 0x01B48019);
	pci_write_config32(dev, 0x90, 0x01B48016);
	pci_write_config32(dev, 0x94, 0x01B48016);
	pci_write_config32(dev, 0x98, 0x01B48016);
	pci_write_config32(dev, 0x9C, 0x01B48016);

	/* RPR7.6.3 SATA GEN II PHY port setting for port [0~5]. */
	pci_write_config16(dev, 0xA0, 0xA09A);
	pci_write_config16(dev, 0xA2, 0xA09F);
	pci_write_config16(dev, 0xA4, 0xA07A);
	pci_write_config16(dev, 0xA6, 0xA07A);
	pci_write_config16(dev, 0xA8, 0xA07A);
	pci_write_config16(dev, 0xAA, 0xA07A);
}

static void sata_init(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;
	u8 rev_id;
	u32 sata_bar5;
	u16 sata_bar0, sata_bar1, sata_bar2, sata_bar3, sata_bar4;
	int i, j;

	device_t sm_dev;
	/* SATA SMBus Disable */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	byte = pci_read_config8(sm_dev, 0xad);
	/* Disable SATA SMBUS */
	byte |= (1 << 0);
	/* Enable SATA and power saving */
	byte |= (1 << 1);
	byte |= (1 << 5);
	pci_write_config8(sm_dev, 0xad, byte);

	/* RPR 7.2 SATA Initialization */
	/* Set the interrupt Mapping to INTG# */
	byte = pci_read_config8(sm_dev, 0xaf);
	byte = 0x6 << 2;
	pci_write_config8(sm_dev, 0xaf, byte);

	/* get rev_id */
	rev_id = pci_read_config8(sm_dev, 0x08) - 0x28;

	/* get base address */
	sata_bar5 = pci_read_config32(dev, 0x24) & ~0x3FF;
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
	printk(BIOS_SPEW, "sata_bar5=%x\n", sata_bar5);	/* e0309000 */

	/* disable combined mode */
	byte = pci_read_config8(sm_dev, 0xAD);
	byte &= ~(1 << 3);
	pci_write_config8(sm_dev, 0xAD, byte);
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

	/* Set bit 29 and 24 for A12 */
	dword = pci_read_config32(dev, 0x40);
	if (rev_id < 0x14)	/* before A12 */
		dword |= (1 << 29);
	else
		dword &= ~(1 << 29); /* A14 and above */
	pci_write_config32(dev, 0x40, dword);

	/* set bit 21 for A12 */
	dword = pci_read_config32(dev, 0x48);
	if (rev_id < 0x14)	/* before A12 */
		dword |= 1 << 24 | 1 << 21;
	else {
		dword &= ~(1 << 24 | 1 << 21); /* A14 and above */
		dword &= ~0xFF80; /* 15:7 */
		dword |= 1 << 15 | 0x7F << 7;
	}
	pci_write_config32(dev, 0x48, dword);

	/* Program the watchdog counter to 0x10 */
	byte = 0x10;
	pci_write_config8(dev, 0x46, byte);
	sb7xx_51xx_setup_sata_phys(dev);
	/* Enable the I/O, MM, BusMaster access for SATA */
	byte = pci_read_config8(dev, 0x4);
	byte |= 7 << 0;
	pci_write_config8(dev, 0x4, byte);

#if CONFIG_SOUTHBRIDGE_AMD_SUBTYPE_SP5100
	/* Master Latency Timer */
	pci_write_config32(dev, 0xC, 0x00004000);
#endif

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
		if( byte == 0x1 ) {
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
	.set_subsystem = pci_dev_set_subsystem,
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
	.device = PCI_DEVICE_ID_ATI_SB700_SATA,
};
