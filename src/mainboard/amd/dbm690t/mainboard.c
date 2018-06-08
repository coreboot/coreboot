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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb600/sb600.h>

#define ADT7461_ADDRESS 0x4C
#define ARA_ADDRESS     0x0C /* Alert Response Address */
#define SMBUS_IO_BASE 0x1000

extern int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
extern int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address,
			       u8 val);
#define ADT7461_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address)
#define ARA_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ARA_ADDRESS, address)
#define ADT7461_write_byte(address, val) \
	do_smbus_write_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address, val)


/********************************************************
* dbm690t uses a BCM5789 as on-board NIC.
* It has a pin named LOW_POWER to enable it into LOW POWER state.
* In order to run NIC, we should let it out of Low power state. This pin is
* controlled by sb600 GPM3.
* RRG4.2.3 GPM as GPIO
* GPM pins can be used as GPIO. The GPM I/O functions is controlled by three registers:
* I/O C50, C51, C52, PM I/O94, 95, 96.
* RRG4.2.3.1 GPM pins as Input
* RRG4.2.3.2 GPM pins as Output
********************************************************/
static void enable_onboard_nic(void)
{
	u8 byte;

	printk(BIOS_INFO, "%s.\n", __func__);

	/* set index register 0C50h to 13h (miscellaneous control) */
	outb(0x13, 0xC50);	/* CMIndex */

	/* set CM data register 0C51h bits [7:6] to 01b to set Input/Out control */
	byte = inb(0xC51);
	byte &= 0x3F;
	byte |= 0x40;
	outb(byte, 0xC51);

	/* set GPM port 0C52h bit 3 to 0 to enable output for GPM3 */
	byte = inb(0xC52);
	byte &= ~0x8;
	outb(byte, 0xC52);

	/* set CM data register 0C51h bits [7:6] to 10b to set Output state control */
	byte = inb(0xC51);
	byte &= 0x3F;
	byte |= 0x80;		/* 7:6=10 */
	outb(byte, 0xC51);

	/* set GPM port 0C52h bit 3 to 0 to output 0 on GPM3 */
	byte = inb(0xC52);
	byte &= ~0x8;
	outb(byte, 0xC52);
}

/********************************************************
* dbm690t uses SB600 GPIO9 to detect IDE_DMA66.
* IDE_DMA66 is routed to GPIO 9. So we read Gpio 9 to
* get the cable type, 40 pin or 80 pin?
********************************************************/
static void get_ide_dma66(void)
{
	u8 byte;
	struct device *sm_dev;
	struct device *ide_dev;

	printk(BIOS_INFO, "%s.\n", __func__);
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	byte = pci_read_config8(sm_dev, 0xA9);
	byte |= (1 << 5);	/* Set Gpio9 as input */
	pci_write_config8(sm_dev, 0xA9, byte);

	ide_dev = dev_find_slot(0, PCI_DEVFN(0x14, 1));
	byte = pci_read_config8(ide_dev, 0x56);
	byte &= ~(7 << 0);
	if ((1 << 5) & pci_read_config8(sm_dev, 0xAA))
		byte |= 2 << 0;	/* mode 2 */
	else
		byte |= 5 << 0;	/* mode 5 */
	pci_write_config8(ide_dev, 0x56, byte);
}

/*
 * set thermal config
 */
static void set_thermal_config(void)
{
	u8 byte;
	u16 word;
	struct device *sm_dev;

	/* set ADT 7461 */
	ADT7461_write_byte(0x0B, 0x50);	/* Local Temperature Hight limit */
	ADT7461_write_byte(0x0C, 0x00);	/* Local Temperature Low limit */
	ADT7461_write_byte(0x0D, 0x50);	/* External Temperature Hight limit  High Byte */
	ADT7461_write_byte(0x0E, 0x00);	/* External Temperature Low limit High Byte */

	ADT7461_write_byte(0x19, 0x55);	/* External THERM limit */
	ADT7461_write_byte(0x20, 0x55);	/* Local THERM limit */

	byte = ADT7461_read_byte(0x02);	/* read status register to clear it */
	ARA_read_byte(0x05); /* A hardware alert can only be cleared by the master sending an ARA as a read command */
	printk(BIOS_INFO, "Init adt7461 end , status 0x02 %02x\n", byte);

	/* sb600 settings for thermal config */
	/* set SB600 GPIO 64 to GPIO with pull-up */
	byte = pm2_ioread(0x42);
	byte &= 0x3f;
	pm2_iowrite(0x42, byte);

	/* set GPIO 64 to input */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	word = pci_read_config16(sm_dev, 0x56);
	word |= 1 << 7;
	pci_write_config16(sm_dev, 0x56, word);

	/* set GPIO 64 internal pull-up */
	byte = pm2_ioread(0xf0);
	byte &= 0xee;
	pm2_iowrite(0xf0, byte);

	/* set Talert to be active low */
	byte = pm_ioread(0x67);
	byte &= ~(1 << 5);
	pm_iowrite(0x67, byte);

	/* set Talert to generate ACPI event */
	byte = pm_ioread(0x3c);
	byte &= 0xf3;
	pm_iowrite(0x3c, byte);

	/* THERMTRIP pin */
	/* byte = pm_ioread(0x68);
	 * byte |= 1 << 3;
	 * pm_iowrite(0x68, byte);
	 *
	 * byte = pm_ioread(0x55);
	 * byte |= 1 << 0;
	 * pm_iowrite(0x55, byte);
	 *
	 * byte = pm_ioread(0x67);
	 * byte &= ~( 1 << 6);
	 * pm_iowrite(0x67, byte);
	 */
}

/*************************************************
* enable the dedicated function in dbm690t board.
* This function called early than rs690_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard DBM690T Enable. dev=0x%p\n", dev);

	enable_onboard_nic();
	get_ide_dma66();
	set_thermal_config();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
