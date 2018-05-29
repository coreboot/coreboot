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

#define ADT7475_ADDRESS 0x2E
#define SMBUS_IO_BASE 0x1000

extern int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
extern int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address,
			       u8 val);
#define ADT7475_read_byte(address) \
	 do_smbus_read_byte(SMBUS_IO_BASE, ADT7475_ADDRESS, address)
#define ADT7475_write_byte(address, val) \
	do_smbus_write_byte(SMBUS_IO_BASE, ADT7475_ADDRESS, address, val)


/********************************************************
* pistachio uses a BCM5787 as on-board NIC.
* It has a pin named LOW_POWER to enable it into LOW POWER state.
* In order to run NIC, we should let it out of Low power state. This pin is
* controlled by GPM8.
* RRG4.2.3 GPM as GPIO
* GPM pins can be used as GPIO. The GPM I/O functions is controlled by three registers:
* I/O C50, C51, C52, PM I/O94, 95, 96.
* RRG4.2.3.1 GPM pins as Input
* RRG4.2.3.2 GPM pins as Output
* The R77 (on BRASS) / R81 (on Bronze) is not load!
* So NIC can work whether this function runs.
********************************************************/
static void enable_onboard_nic(void)
{
	u8 byte;

	printk(BIOS_INFO, "%s.\n", __func__);

	/* enable GPM8 output */
	byte = pm_ioread(0x95);
	byte &= 0xfe;
	pm_iowrite(0x95, byte);

	/* GPM8 outputs low. */
	byte = pm_ioread(0x94);
	byte &= 0xfe;
	pm_iowrite(0x94, byte);
}

/*
 * set thermal config
 */
static void set_thermal_config(void)
{
	u8 byte, byte2;
	u16 word;
	u32 dword;
	struct device *sm_dev;

	/* set adt7475 */
	ADT7475_write_byte(0x40, 0x04);
	/* Config Register 6 */
	ADT7475_write_byte(0x10, 0x00);
	/* Config Register 7 */
	ADT7475_write_byte(0x11, 0x00);

	/* set Offset 64 format, enable THERM on Remote 1& Remote 2 */
	ADT7475_write_byte(0x7c, 0xa0);
	/* No offset for remote 2 */
	ADT7475_write_byte(0x72, 0x00);
	/* PWM 1 configuration register    CPU fan controlled by CPU Thermal Diode */
	ADT7475_write_byte(0x5c, 0x02);
	/* PWM 3 configuration register    Case fan controlled by 690 temp */
	ADT7475_write_byte(0x5e, 0x42);

	/* remote 1 low temp limit */
	ADT7475_write_byte(0x4e, 0x00);
	/* remote 1 High temp limit    (90C) */
	ADT7475_write_byte(0x4f, 0x9a);

	/* remote2 Low Temp Limit */
	ADT7475_write_byte(0x52, 0x00);
	/* remote2 High Limit    (90C) */
	ADT7475_write_byte(0x53, 0x9a);

	/*  remote 1 therm temp limit    (95C) */
	ADT7475_write_byte(0x6a, 0x9f);
	/* remote 2 therm temp limit    (95C) */
	ADT7475_write_byte(0x6c, 0x9f);

	/* PWM 1 minimum duty cycle     (37%) */
	ADT7475_write_byte(0x64, 0x60);
	/* PWM 1 Maximum duty cycle    (100%) */
	ADT7475_write_byte(0x38, 0xff);
	/* PWM 3 minimum duty cycle     (37%) */
	ADT7475_write_byte(0x66, 0x60);
	/* PWM 3 Maximum Duty Cycle    (100%) */
	ADT7475_write_byte(0x3a, 0xff);

	/*  Remote 1 temperature Tmin     (32C) */
	ADT7475_write_byte(0x67, 0x60);
	/* Remote 2 temperature Tmin     (32C) */
	ADT7475_write_byte(0x69, 0x60);
	/* remote 1 Trange            (53C ramp range) */
	ADT7475_write_byte(0x5f, 0xe8);
	/* remote 2 Trange            (53C ramp range) */
	ADT7475_write_byte(0x61, 0xe8);

	/* PWM2 Duty cycle */
	ADT7475_write_byte(0x65, 0x00);
	/* PWM2 Disabled */
	ADT7475_write_byte(0x5d, 0x80);
	/* PWM2 Max Duty Cycle */
	ADT7475_write_byte(0x39, 0x00);

	/* Config Register 3 - enable smbalert & therm */
	ADT7475_write_byte(0x78, 0x03);
	/* Config Register 4 - enable therm output */
	ADT7475_write_byte(0x7d, 0x09);
	/* Interrupt Mask Register 2 - Mask SMB alert for Therm Conditions, Fan 2 fault, SmbAlert Fan for Therm Timer event */
	ADT7475_write_byte(0x75, 0x2a);
	/* Config Register 1 Set Start bit */
	ADT7475_write_byte(0x40, 0x05);
	/* Read status register to clear any old errors */
	byte2 = ADT7475_read_byte(0x42);
	byte = ADT7475_read_byte(0x41);

	/* remote 1 temperature offset */
	ADT7475_write_byte(0x70, 0x00);

	printk(BIOS_INFO, "Init adt7475 end , status 0x42 %02x, status 0x41 %02x\n",
		    byte2, byte);

	/* sb600 setting for thermal config. Set SB600 GPM5 to trigger ACPI event */
	/* set GPM5 as GPM5, not DDR3_memory disable */
	byte = pm_ioread(0x8f);
	byte |= 1 << 6;		/* enable GPE */
	pm_iowrite(0x8f, byte);

	/* GPM5 as GPIO not USB OC */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	dword = pci_read_config32(sm_dev, 0x64);
	dword |= 1 << 19;
	pci_write_config32(sm_dev, 0x64, dword);

	/* Enable Client Management Index/Data registers */
	dword = pci_read_config32(sm_dev, 0x78);
	dword |= 1 << 11;	/* Cms_enable */
	pci_write_config32(sm_dev, 0x78, dword);

	/* MiscfuncEnable */
	byte = pci_read_config8(sm_dev, 0x41);
	byte |= (1 << 5);
	pci_write_config8(sm_dev, 0x41, byte);

	/* set GPM5 as input */
	/* set index register 0C50h to 13h (miscellaneous control) */
	outb(0x13, 0xC50);	/* CMIndex */
	/* set CM data register 0C51h bits [7:6] to 01b to set Input/Out control */
	byte = inb(0xC51);	/* CMData */
	byte &= 0x3f;
	byte |= 1 << 6;
	outb(byte, 0xC51);
	/* set GPM port 0C52h bit 5 to 1 to tri-state the GPM port */
	byte = inb(0xc52);	/* GpmPort */
	byte |= 1 << 5;
	outb(byte, 0xc52);
	/* set CM data register 0C51h bits [7:6] to 00b to set GPM port for read */
	byte = inb(0xc51);
	byte &= 0x3f;
	outb(byte, 0xc51);

	/* trigger SCI/SMI */
	byte = pm_ioread(0x34);
	byte &= 0xcf;
	pm_iowrite(0x34, byte);

	/* set GPM5 to not wake from s5 */
	byte = pm_ioread(0x77);
	byte &= ~(1 << 5);
	pm_iowrite(0x77, byte);

	/* trigger on falling edge */
	byte = pm_ioread(0x38);
	byte &= ~(1 << 2);
	pm_iowrite(0x38, byte);

	/* set SB600 GPIO 64 to GPIO with pull-up */
	byte = pm2_ioread(0x42);
	byte &= 0x3f;
	pm2_iowrite(0x42, byte);

	/* set GPIO 64 to input */
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
* enable the dedicated function in pistachio board.
* This function called early than rs690_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard Pistachio Enable. dev=0x%p\n", dev);

	enable_onboard_nic();
	set_thermal_config();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
