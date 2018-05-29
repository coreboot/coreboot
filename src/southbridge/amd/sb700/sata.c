/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#include <compiler.h>
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <option.h>
#include "sb700.h"

static int sata_drive_detect(int portnum, uint16_t iobar)
{
	u8 byte, byte2;
	u8 byte_prev, byte2_prev;
	int i = 0;
	byte_prev = byte2_prev = 0;
	outb(0xa0 + 0x10 * (portnum % 2), iobar + 0x6);
	while (byte = inb(iobar + 0x6), byte2 = inb(iobar + 0x7),
		(byte != (0xa0 + 0x10 * (portnum % 2))) ||
		((byte2 & 0x88) != 0)) {
		if ((byte != byte_prev) || (byte2 != byte2_prev))
			printk(BIOS_SPEW, "0x6=%x, 0x7=%x\n", byte, byte2);
		if (byte != (0xa0 + 0x10 * (portnum % 2))) {
			/* This will happen at the first iteration of this loop
			 * if the first SATA port is unpopulated and the
			 * second SATA port is populated.
			 */
			printk(BIOS_DEBUG, "drive no longer selected after %i ms, "
				"retrying init\n", i * 10);
			return 1;
		} else {
			if (i == 0)
				printk(BIOS_SPEW, "drive detection not yet completed, "
					"waiting...\n");
		}
		mdelay(10);
		i++;
		byte_prev = byte;
		byte2_prev = byte2;

		/* Detect stuck SATA controller and attempt reset */
		if (i > 1024) {
			printk(BIOS_DEBUG, "drive detection not done after %i ms, "
				"resetting HBA and retrying init\n", i * 10);
			return 2;
		}
	}
	printk(BIOS_SPEW, "drive detection done after %i ms\n", i * 10);
	return 0;
}

/* This function can be overloaded in mainboard.c */
void __weak sb7xx_51xx_setup_sata_phys(struct device *dev)
{
	/* RPR7.6.1 Program the PHY Global Control to 0x2C00 */
	pci_write_config16(dev, 0x86, 0x2c00);

	/* RPR7.6.2 SATA GENI PHY ports setting */
	pci_write_config32(dev, 0x88, 0x01B48017);
	pci_write_config32(dev, 0x8c, 0x01B48019);
	pci_write_config32(dev, 0x90, 0x01B48016);
	pci_write_config32(dev, 0x94, 0x01B48016);
	pci_write_config32(dev, 0x98, 0x01B48016);
	pci_write_config32(dev, 0x9c, 0x01B48016);

	/* RPR7.6.3 SATA GEN II PHY port setting for port [0~5]. */
	pci_write_config16(dev, 0xa0, 0xA09A);
	pci_write_config16(dev, 0xa2, 0xA09F);
	pci_write_config16(dev, 0xa4, 0xA07A);
	pci_write_config16(dev, 0xa6, 0xA07A);
	pci_write_config16(dev, 0xa8, 0xA07A);
	pci_write_config16(dev, 0xaa, 0xA07A);
}

/* This function can be overloaded in mainboard.c */
void __weak sb7xx_51xx_setup_sata_port_indication(void *sata_bar5)
{
	uint32_t dword;

	/* RPR7.9 Program Port Indication Registers */
	dword = read32(sata_bar5 + 0xf8);
	dword &= ~(0x3f << 12);	/* Ports 0 and 1 are eSATA */
	dword |= (0x3 << 12);
	dword &= ~0x3f;
	write32(sata_bar5 + 0xf8, dword);

	dword = read32(sata_bar5 + 0xfc);
	dword |= 0x1 << 20;	/* At least one eSATA port is present */
	write32(sata_bar5 + 0xfc, dword);
}

static void sata_init(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;
	u8 rev_id;
	void *sata_bar5;
	uint16_t sata_bar0, sata_bar1, sata_bar2, sata_bar3, sata_bar4;
	uint16_t ide_bar0, ide_bar1, ide_bar2, ide_bar3;
	uint16_t current_bar;
	int i, j, ret;
	uint8_t nvram;
	uint8_t sata_ahci_mode;
	uint8_t sata_alpm_enable;
	uint8_t port_count;
	uint8_t max_port_count;
	uint8_t ide_io_enabled;
	uint8_t ide_legacy_io_enabled;

	sata_ahci_mode = 0;
	if (get_option(&nvram, "sata_ahci_mode") == CB_SUCCESS)
		sata_ahci_mode = !!nvram;

	sata_alpm_enable = 0;
	if (get_option(&nvram, "sata_alpm") == CB_SUCCESS)
		sata_alpm_enable = !!nvram;

	struct device *sm_dev;
	/* SATA SMBus Disable */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	/* WARNING
	 * Enabling the SATA link latency enhancement (SMBUS 0xAD bit 5)
	 * causes random persistent drive detection failures until it is cleared,
	 * with the probabability of detection failure rising exponentially with
	 * the number of drives attached to the controller!
	 * This happens on Rev15 H/W.
	 * Do NOT follow the RPR advice; leave this bit set at all times...
	 */
	byte = pci_read_config8(sm_dev, 0xad);
	/* Disable SATA SMBUS */
	byte |= (1 << 1);
	/* Enable SATA and power saving */
	byte |= (1 << 0);
	/* Disable link latency enhancement */
	byte |= (1 << 5);
	pci_write_config8(sm_dev, 0xad, byte);

	/* Take the PHY logic out of reset */
	word = pci_read_config16(dev, 0x84);
	word |= 0x1 << 2;
	word &= ~0x1f8;
	pci_write_config16(dev, 0x84, word);

	/* get rev_id */
	rev_id = pci_read_config8(sm_dev, 0x08) - 0x28;

	printk(BIOS_SPEW, "rev_id=%x\n", rev_id);

	/* Enable combined mode */
	byte = pci_read_config8(sm_dev, 0xad);
	byte |= (1 << 3);
	pci_write_config8(sm_dev, 0xad, byte);

	struct device *ide_dev;
	/* IDE Device */
	ide_dev = dev_find_slot(0, PCI_DEVFN(0x14, 1));

	/* Disable legacy IDE mode (enable PATA_BAR0/2) */
	byte = pci_read_config8(ide_dev, 0x09);
	ide_legacy_io_enabled = !(byte & 0x1);
	byte |= 0x1;
	pci_write_config8(ide_dev, 0x09, byte);

	/* Enable IDE I/O access (enable PATA_BAR0/2) */
	byte = pci_read_config8(ide_dev, 0x04);
	ide_io_enabled = byte & 0x1;
	byte |= 0x1;
	pci_write_config8(ide_dev, 0x04, byte);

	/* RPR 7.2 SATA Initialization */
	/* Set the interrupt Mapping to INTG# */
	byte = pci_read_config8(sm_dev, 0xaf);
	byte = 0x6 << 2;
	pci_write_config8(sm_dev, 0xaf, byte);

	/* get base address */
	sata_bar5 = (void *)(pci_read_config32(dev, 0x24) & ~0x3FF);
	sata_bar0 = pci_read_config16(dev, 0x10) & ~0x7;
	sata_bar1 = pci_read_config16(dev, 0x14) & ~0x3;
	sata_bar2 = pci_read_config16(dev, 0x18) & ~0x7;
	sata_bar3 = pci_read_config16(dev, 0x1c) & ~0x3;
	sata_bar4 = pci_read_config16(dev, 0x20) & ~0xf;

	printk(BIOS_SPEW, "sata_bar0=%x\n", sata_bar0);	/* 3030 */
	printk(BIOS_SPEW, "sata_bar1=%x\n", sata_bar1);	/* 3070 */
	printk(BIOS_SPEW, "sata_bar2=%x\n", sata_bar2);	/* 3040 */
	printk(BIOS_SPEW, "sata_bar3=%x\n", sata_bar3);	/* 3080 */
	printk(BIOS_SPEW, "sata_bar4=%x\n", sata_bar4);	/* 3000 */
	printk(BIOS_SPEW, "sata_bar5=%p\n", sata_bar5);	/* e0309000 */

	ide_bar0 = pci_read_config16(ide_dev, 0x10) & ~0x7;
	ide_bar1 = pci_read_config16(ide_dev, 0x14) & ~0x3;
	ide_bar2 = pci_read_config16(ide_dev, 0x18) & ~0x7;
	ide_bar3 = pci_read_config16(ide_dev, 0x1c) & ~0x3;
	printk(BIOS_SPEW, "ide_bar0=%x\n", ide_bar0);
	printk(BIOS_SPEW, "ide_bar1=%x\n", ide_bar1);
	printk(BIOS_SPEW, "ide_bar2=%x\n", ide_bar2);
	printk(BIOS_SPEW, "ide_bar3=%x\n", ide_bar3);

	/* Program the Subsystem ID/VID to 0x43801002 */
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

	/* Unlock subclass and certain BAR R/O registers */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 0);
	pci_write_config8(dev, 0x40, byte);

	/* Disable AHCI enhancement (AMD SP5100 RPR page 54) */
	dword = pci_read_config32(dev, 0x40);
	dword |= (1 << 23);
	pci_write_config32(dev, 0x40, dword);

	if (sata_ahci_mode) {
		/* Force number of ports to 6
		 * NOTE: This is not documented in the register
		 * reference guide, but CIMX needs to do this
		 * to activate all 6 ports when IDE is disabled.
		 */
		dword = read32(sata_bar5 + 0x00);
		dword &= ~0x7;
		dword |= 0x5;
		write32(sata_bar5 + 0x00, dword);
	} else {
		/* Set SATA Operation Mode, Set to IDE mode */
		byte = pci_read_config8(dev, 0x40);
		byte |= (1 << 4);
		pci_write_config8(dev, 0x40, byte);

		dword = 0x01018f00;
		pci_write_config32(dev, 0x8, dword);
	}

	/* Get maximum number of ports */
	max_port_count = read32(sata_bar5 + 0x00) & 0x1f;
	max_port_count++;
	printk(BIOS_SPEW, "Maximum SATA port count supported by silicon: %d\n", max_port_count);

	/* Set number of ports */
	dword = CONFIG_SOUTHBRIDGE_AMD_SB700_SATA_PORT_COUNT_BITFIELD;
	for (i = max_port_count; i < 32; i++)
		dword &= ~(0x1 << i);
	write32(sata_bar5 + 0x0c, dword);

	/* Disable ALPM if ALPM support not requested */
	if (!sata_alpm_enable) {
		dword = read32(sata_bar5 + 0xfc);
		dword &= ~(0x1 << 11);	/* Disable ALPM */
		write32(sata_bar5 + 0xfc, dword);
	}

	/* Enable SATA ports */
	byte = pci_read_config8(dev, 0x42);
	if (max_port_count <= 6) {
		byte |= 0x3f;
		for (i = 0; i < max_port_count; i++)
			byte &= ~(0x1 << i);
	} else {
		byte &= ~0x3f;
	}
	pci_write_config8(dev, 0x42, byte);

	if (sata_ahci_mode) {
		/* FIXME
		* SeaBIOS does not know how to spin
		* up the drives and therefore hangs
		* in AHCI init if this is enabled...
		*/
		/* Enable staggered spin-up */
		dword = read32(sata_bar5 + 0x00);
#if 0
		dword |= 0x1 << 27;
#else
		dword &= ~(0x1 << 27);
#endif
		write32(sata_bar5 + 0x00, dword);

		/* Reset the HBA to avoid stuck drives in SeaBIOS */
		dword = read32(sata_bar5 + 0x04);
		dword |= 0x1;
		write32(sata_bar5 + 0x04, dword);
	}

	sb7xx_51xx_setup_sata_phys(dev);
	sb7xx_51xx_setup_sata_port_indication(sata_bar5);

	/* Write protect Sub-Class Code */
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
		dword |= 1 << 15 | 0x7F << 7 | 1 << 6;
	}
	pci_write_config32(dev, 0x48, dword);

	/* Program the watchdog counter to 0x10 */
	byte = 0x10;
	pci_write_config8(dev, 0x46, byte);

	/* Enable the I/O, MM, BusMaster access for SATA */
	byte = pci_read_config8(dev, 0x4);
	byte |= 7 << 0;
	pci_write_config8(dev, 0x4, byte);

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SUBTYPE_SP5100)
	/* Master Latency Timer */
	pci_write_config32(dev, 0xC, 0x00004000);
#endif

	/* Determine port count */
	port_count = 0;
	for (i = 0; i < 32; i++) {
		if (CONFIG_SOUTHBRIDGE_AMD_SB700_SATA_PORT_COUNT_BITFIELD & (0x1 << i))
			port_count = i;
	}
	port_count++;
	if (port_count > max_port_count)
		port_count = max_port_count;

	/* Send COMRESET to all ports */
	for (i = 0; i < port_count; i++) {
		/* Read in Port-N Serial ATA Control Register */
		byte = read8(sata_bar5 + 0x12C + 0x80 * i);

		/* Set Reset Bit */
		byte |= 0x1;
		write8((sata_bar5 + 0x12C + 0x80 * i), byte);

		/* Wait 1ms */
		mdelay(1);

		/* Clear Reset Bit */
		byte &= ~0x01;
		write8((sata_bar5 + 0x12C + 0x80 * i), byte);

		/* Wait 1ms */
		mdelay(1);
	}

	/* RPR7.7 SATA drive detection. */
	/* Use BAR5+0x128,BAR0 for Primary Slave */
	/* Use BAR5+0x1A8,BAR0 for Primary Slave */
	/* Use BAR5+0x228,BAR2 for Secondary Master */
	/* Use BAR5+0x2A8,BAR2 for Secondary Slave */
	/* Use BAR5+0x328,PATA_BAR0/2 for Primary/Secondary Master emulation */
	/* Use BAR5+0x3A8,PATA_BAR0/2 for Primary/Secondary Slave emulation */
	for (i = 0; i < port_count; i++) {
		byte = read8(sata_bar5 + 0x128 + 0x80 * i);
		printk(BIOS_SPEW, "SATA port %i status = %x\n", i, byte);
		byte &= 0xF;
		if (byte == 0x1) {
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
				if (i < 4)
					current_bar = ((i / 2) == 0) ? sata_bar0 : sata_bar2;
				else
					current_bar = (pci_read_config8(sm_dev, 0xad) & (0x1 << 4))
						? ide_bar2 : ide_bar0;
				ret = sata_drive_detect(i, current_bar);
				if (ret == 0) {
					break;
				} else if (ret == 2) {
					/* Read in Port-N Serial ATA Control Register */
					byte = read8(sata_bar5 + 0x12C + 0x80 * i);

					/* Set Reset Bit */
					byte |= 0x1;
					write8((sata_bar5 + 0x12C + 0x80 * i), byte);

					/* Wait 1000ms */
					mdelay(1000);

					/* Clear Reset Bit */
					byte &= ~0x01;
					write8((sata_bar5 + 0x12C + 0x80 * i), byte);

					/* Wait 1ms */
					mdelay(1);
				}
			}
			if (sata_ahci_mode)
				printk(BIOS_DEBUG, "AHCI device %d is %sready after %i tries\n",
						i,
						(j == 10) ? "not " : "",
						(j == 10) ? j : j + 1);
			else
				printk(BIOS_DEBUG, "%s %s device is %sready after %i tries\n",
						(i / 2) ? "Secondary" : "Primary",
						(i % 2 ) ? "Slave" : "Master",
						(j == 10) ? "not " : "",
						(j == 10) ? j : j + 1);
		} else {
			if (sata_ahci_mode)
				printk(BIOS_DEBUG, "No AHCI SATA drive on Slot%i\n", i);
			else
				printk(BIOS_DEBUG, "No %s %s SATA drive on Slot%i\n",
						(i / 2) ? "Secondary" : "Primary",
						(i % 2 ) ? "Slave" : "Master", i);
		}
	}

	/* Restore IDE I/O access */
	if (!ide_io_enabled) {
		byte = pci_read_config8(ide_dev, 0x04);
		byte &= ~0x1;
		pci_write_config8(ide_dev, 0x04, byte);
	}

	/* Re-enable legacy IDE mode */
	if (ide_legacy_io_enabled) {
		byte = pci_read_config8(ide_dev, 0x09);
		byte &= ~0x1;
		pci_write_config8(ide_dev, 0x09, byte);
	}

	/* Below is CIM InitSataLateFar */
	if (sata_ahci_mode) {
		/* Disable combined mode */
		byte = pci_read_config8(sm_dev, 0xad);
		byte &= ~(1 << 3);
		pci_write_config8(sm_dev, 0xad, byte);
	} else {
		/* Enable interrupts from the HBA  */
		byte = read8(sata_bar5 + 0x4);
		byte |= 1 << 1;
		write8((sata_bar5 + 0x4), byte);
	}

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

static const struct pci_driver sata1_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_SATA_AHCI,
};

static const struct pci_driver sata2_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_SATA_AHCI_AMD,
};
