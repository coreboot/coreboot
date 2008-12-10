/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Corey Osgood <corey.osgood@gmail.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <io.h>
#include <device/pci_ids.h>
#include <spd.h>
#include "vt8237.h"
#include <config.h>

/* TODO List:
 * * Merge the rest of the functions from v2, except smbus_fixup which doesn't
 *   seem to be necessary any more (?)
 * * Clean up vt8237_early_network_init. 
 *     Comments in code indicate that it's broken?
 * * Figure out if the smbus actually needs to be reset after every transaction.
 */

/**
 * Print an error, should it occur. If no error, just exit.
 *
 * @param host_status The data returned on the host status register after
 *		      a transaction is processed.
 * @param loops The number of times a transaction was attempted.
 */
static void smbus_print_error(u8 host_status, int loops)
{
	/* Check if there actually was an error. */
	if ((host_status == 0x00 || host_status == 0x40 ||
		host_status == 0x42) && (loops < SMBUS_TIMEOUT))
	{
		printk(BIOS_SPEW, "SMBus Ready/Completed Successfully\n");
		return;
	}
	if (loops >= SMBUS_TIMEOUT)
		printk(BIOS_ERR, "SMBus Timed out\n");
	if (host_status & (1 << 4))
		printk(BIOS_ERR, "Interrupt/SMI# was Failed Bus Transaction\n");
	if (host_status & (1 << 3))
		printk(BIOS_ERR, "Bus error\n");
	if (host_status & (1 << 2))
		printk(BIOS_ERR, "Device error\n");
	if (host_status & (1 << 1))
		printk(BIOS_SPEW, "Interrupt/SMI# completed successfully\n");
	if (host_status & (1 << 0))
		printk(BIOS_ERR, "Host busy\n");
}

/**
 * Reset and take ownership of the SMBus.
 */
static void smbus_reset(u16 smbus_io_base)
{
	outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);

	/* Datasheet says we have to read it to take ownership of SMBus. */
	smbus_print_error(inb(smbus_io_base + SMBHSTSTAT), 0);
}

/**
 * Wait for the SMBus to become ready to process the next transaction.
 */
static void smbus_wait_until_ready(u16 smbus_io_base)
{
	int loops;

	printk(BIOS_SPEW, "Waiting until SMBus ready\n");

	loops = 0;
	while ((inb(smbus_io_base + SMBHSTSTAT) & 1) == 1 && loops <= SMBUS_TIMEOUT)
		++loops;

	smbus_print_error(inb(smbus_io_base + SMBHSTSTAT), loops);
}

/**
 * Read a byte from the SMBus.
 *
 * @param dimm The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 * @param smbus_io_base The SMBus I/O base, usually 0x400.
 */
u8 smbus_read_byte(u16 dimm, u8 offset, u16 smbus_io_base)
{
	u8 val;

	printk(BIOS_SPEW, "SMBus Read from DIMM %d at address 0x%x\n", 
				(int)dimm, offset);

	smbus_reset(smbus_io_base);

	/* Clear host data port. */
	outb(0x00, smbus_io_base + SMBHSTDAT0);
	/* Doesn't seem to be necessary...*/
	/* udelay(1); */
	smbus_wait_until_ready(smbus_io_base);

	/* With this, addresses are 0x50, 0x51, etc. Without it,
	 * addresses would be 0xa1, 0xa3, etc */
	dimm = (dimm << 1) | 1;

	outb(dimm, smbus_io_base + SMBXMITADD);
	outb(offset, smbus_io_base + SMBHSTCMD);

	/* Start transaction, byte data read. */
	outb(0x48, smbus_io_base + SMBHSTCTL);
	/* udelay(1); */
	smbus_wait_until_ready(smbus_io_base);

	val = inb(smbus_io_base + SMBHSTDAT0);
	printk(BIOS_SPEW, "Read: 0x%x\n", val);
	
	/* TODO: Is this necessary? */
	smbus_reset(smbus_io_base);

	return val;
}

/**
 * Enable the smbus on vt8237-based systems
 *
 * @param smbus_io_base The SMBus I/O base, usually 0x400.
 */
void enable_smbus(u16 smbus_io_base)
{
	u32 dev;

	/* Power management controller */
	pci_conf1_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VT8237R_LPC, 
				&dev);

	if (!dev) {
		/* Power management controller */
		pci_conf1_find_device(PCI_VENDOR_ID_VIA,
					PCI_DEVICE_ID_VIA_VT8237S_LPC, &dev);
		if (!dev)
		{
			printk(BIOS_ERR, "Power management controller not "
					"found! Using hardcoded default.\n");
			dev = PCI_BDF(0, 17, 0);
		} else {
			printk(BIOS_DEBUG, "VT8237S Power management "
					"controller found at 0x%x\n", dev);
		}
	} else {
		printk(BIOS_DEBUG, "VT8237R Power management controller found "
						"at 0x%x\n", dev);
	}

	/* 7 = SMBus Clock from RTC 32.768KHz
	 * 5 = Internal PLL reset from susp
	 */
	pci_conf1_write_config8(dev, VT8237_POWER_WELL, 0xa0);

	/* Enable SMBus. */
	pci_conf1_write_config16(dev, VT8237_SMBUS_IO_BASE_REG,
			   smbus_io_base | 0x1);

	/* SMBus Host Configuration, enable. */
	pci_conf1_write_config8(dev, VT8237_SMBUS_HOST_CONF, 0x01);

	/* Make it work for I/O. */
	pci_conf1_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	/* reset smbus */
	smbus_reset(smbus_io_base);

	/* Reset the internal pointer. */
	inb(smbus_io_base + SMBHSTCTL);
}

/* The change from RAID to SATA in phase6 causes coreboot to lock up, so do it
 * as early as possible. Move back to stage2 later */ 
static void sata_stage1(void)
{
	u32 dev;
	u8 reg;

	pci_conf1_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VT8237R_SATA, &dev);

	printk(BIOS_DEBUG, "Configuring VIA SATA controller\n");

	/* Class IDE Disk */
	reg = pci_conf1_read_config8(dev, SATA_MISC_CTRL);
	reg &= 0x7f;		/* Sub Class Write Protect off */
	pci_conf1_write_config8(dev, SATA_MISC_CTRL, reg);

	/* Change the device class to SATA from RAID. */
	pci_conf1_write_config8(dev, PCI_CLASS_DEVICE, 0x1);
	reg |= 0x80;		/* Sub Class Write Protect on */
	pci_conf1_write_config8(dev, SATA_MISC_CTRL, reg);
}

void vt8237_stage1(u16 smbus_io_base)
{
	u32 dev;
	u32 ide_dev;
	
	printk(BIOS_DEBUG, "Doing vt8237r/s stage1 init\n");

	pci_conf1_find_device(0x1106, 0x3227, &dev);
	pci_conf1_find_device(0x1106, 0x0571, &ide_dev);
	
	/* Disable GP3 timer, or else the system reboots when it runs out */
	pci_conf1_write_config8(dev, 0x98, 0x00);
	
	/* Change the ROM size mapping based on where CAR is located */
#if (CONFIG_CARBASE + CONFIG_CARSIZE) < 0xffc00000
	pci_conf1_write_config8(dev, 0x41, 0x7f);
#elif (CONFIG_CARBASE + CONFIG_CARSIZE) < 0xffe00000
	pci_conf1_write_config8(dev, 0x41, 0x70);
#elif (CONFIG_CARBASE + CONFIG_CARSIZE) < 0xfff00000
	pci_conf1_write_config8(dev, 0x41, 0x40);
#endif

	pci_conf1_write_config8(dev, 0x50, 0x80);//disable mc97
	pci_conf1_write_config8(dev, 0x51, 0x1f);
	pci_conf1_write_config8(dev, 0x58, 0x60);
	pci_conf1_write_config8(dev, 0x59, 0x80);
	pci_conf1_write_config8(dev, 0x5b, 0x08);

	/* Make it respond to IO space */
	pci_conf1_write_config8(ide_dev, 0x04, 0x07);

	/* Compatibility mode addresses */
	//pci_conf1_write_config32(ide_dev, 0x10, 0);
	//pci_conf1_write_config32(ide_dev, 0x14, 0);
	//pci_conf1_write_config32(ide_dev, 0x18, 0);
	//pci_conf1_write_config32(ide_dev, 0x1b, 0);

	/* Native mode base address */
	//pci_conf1_write_config32(ide_dev, 0x20, BUS_MASTER_ADDR | 1);

	pci_conf1_write_config8(ide_dev, 0x40, 0x3);//was 0x3
	pci_conf1_write_config8(ide_dev, 0x41, 0xf2);
	pci_conf1_write_config8(ide_dev, 0x42, 0x09);

	sata_stage1();
	enable_smbus(smbus_io_base);
}

/* TODO:
 * Magic numbers -> #defines
 * fix?
 * clean up
 */
 
/* offset 0x58
 * 31:20 	reserved
 * 19:16	4 bit position in shadow EEPROM
 * 15:0		data to write
 *
 * offset 0x5c
 * 31:28	reserved
 * 27 		ERDBG - enable read from 0x5c
 * 26		reserved
 * 25		SEELD
 * 24		SEEPR - write 1 when done updating, wait until SEELD is set to 1, sticky
 *		cleared by reset, if it is 1 writing is disabled
 * 19:16	4 bit position in shadow EEPROM
 * 15:0		data from shadow EEPROM
 *
 * after PCIRESET SEELD and SEEPR must be 1 and 1
*/

/* 1 = needs PCI reset, 0 don't reset, network initialized */

/* fixme maybe close the debug register after use? */

#define LAN_TIMEOUT 0x7FFFFFFF

int vt8237_early_network_init(struct vt8237_network_rom *rom) {
	struct vt8237_network_rom n;
	int loops = 0;
	u32 dev;
	u32 tmp;
	u8 status;
	u16 *rom_write;
	unsigned int checksum;
	int i;

	/* Network adapter */
	pci_conf1_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VT8237_LAN,
				&dev);

	if (!dev) {
	    printk(BIOS_ERR, "Network is disabled, please enable\n");
	    return 0;
	}

	tmp = pci_conf1_read_config32(dev, 0x5c);
	/* enable ERDBG */
	tmp |= 0x08000000;
	pci_conf1_write_config32(dev, 0x5c, tmp);
	
	status = ((pci_conf1_read_config32(dev, 0x5c) >> 24) & 0x3);
	
	if (status == 3) {
	    /* network controller OK, EEPROM loaded */
	    return 0;
	}
	
	if (rom == NULL) {
	    printk(BIOS_ERR, "No configuration data specified, using default MAC!\n");
		n.mac_address[0] = 0x0;
		n.mac_address[1] = 0x0;
		n.mac_address[2] = 0xde;
		n.mac_address[3] = 0xad;
		n.mac_address[4] = 0xbe;
		n.mac_address[5] = 0xef;
		n.phy_addr = 0x1;
		n.res1 = 0x0;
		n.sub_sid = 0x102;
		n.sub_vid = 0x1106;
		n.pid = 0x3065;
		n.vid = 0x1106;
		n.pmcc = 0x1f;
		n.data_sel = 0x10;
		n.pmu_data_reg = 0x0;
		n.aux_curr = 0x0;
		n.reserved = 0x0;
		n.min_gnt = 0x3;
		n.max_lat = 0x8;
		n.bcr0 = 0x9;
		n.bcr1 = 0xe;
		n.cfg_a = 0x3;
		n.cfg_b = 0x0;
		n.cfg_c = 0x40;
		n.cfg_d = 0x82;
		n.checksum = 0x0;
		rom = &n;
	}
	
	rom_write = (u16 *) rom;
	checksum = 0;
	/* write all data except checksum and second to last byte */
	tmp &= 0xff000000; /* leave reserved bits in */
	for (i = 0; i < 15; i++) {
		pci_conf1_write_config32(dev, 0x58, tmp | (i << 16) | rom_write[i]);
		/* lame code fixme */
		checksum += rom_write[i] & 0xff;
		//checksum %= 256;
		checksum += (rom_write[i] >> 8) & 0xff;
		//checksum %= 256;
	}
	
	checksum += (rom_write[15] & 0xff);
	checksum = ~(checksum & 0xff);
	tmp |= (((checksum & 0xff) << 8) | rom_write[15]);

	/* write last byte and checksum */
	pci_conf1_write_config32(dev, 0x58, (15 << 16) |  tmp);
	
	tmp = pci_conf1_read_config32(dev, 0x5c);
	pci_conf1_write_config32(dev, 0x5c, tmp | 0x01000000); /* toggle SEEPR */
	
	/* Yes, this is a mess, but it's the easiest way to do it. */
	while ( (((pci_conf1_read_config32(dev, 0x5c) >> 25) & 1) == 0)
 			&& (loops < LAN_TIMEOUT))
		++loops;

	if (loops >= LAN_TIMEOUT) {
	    printk(BIOS_ERR, "Timout - LAN controller did not accept configuration\n");
	    return 0;
	}
	
	/* we are done, config will be used after PCIRST# */
	return 1;
}
