/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux NetworX
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
#include <console.h>
#include <device/device.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include <device/smbus.h>
#include <mc146818rtc.h>
#include "amd8111.h"
#include "amd8111_smbus.h"

#define PREVIOUS_POWER_STATE 0x43
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1
#define SLOW_CPU_OFF 0
#define SLOW_CPU__ON 1

#ifndef MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif


static int lsmbus_recv_byte(struct device * dev)
{
	int do_smbus_recv_byte(u16 smbus_io_base, u16 device);
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);
	
	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(struct device * dev, u8 val)
{
	int do_smbus_send_byte(u16 smbus_io_base, u8 device, u8 val);
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_send_byte(res->base, device, val);
}


static int lsmbus_read_byte(struct device * dev, u8 address)
{
	int do_smbus_read_byte(u16 smbus_io_base, u8 device, u8 address);
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);
	
	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device * dev, u8 address, u8 val)
{
	int do_smbus_write_byte(u16 smbus_io_base, u8 device, u8 address, u8 val);
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);
	
	return do_smbus_write_byte(res->base, device, address, val);
}

#ifdef CONFIG_ACPI_TABLE
unsigned pm_base;
#endif

static void acpi_init(struct device *dev)
{
	u8 byte;
	u16 pm10_bar;
	u32 dword;
	int on;

#if 0
	u16 word;
	printk(BIOS_DEBUG, "ACPI: disabling NMI watchdog.. ");
	byte = pci_read_config8(dev, 0x49);
	pci_write_config8(dev, 0x49, byte | (1<<2));


	byte = pci_read_config8(dev, 0x41);
	pci_write_config8(dev, 0x41, byte | (1<<6)|(1<<2));

	/* added from sourceforge */
	byte = pci_read_config8(dev, 0x48);
	pci_write_config8(dev, 0x48, byte | (1<<3));

	printk(BIOS_DEBUG, "done.\n");


	printk(BIOS_DEBUG, "ACPI: Routing IRQ 12 to PS2 port.. ");
	word = pci_read_config16(dev, 0x46);
	pci_write_config16(dev, 0x46, word | (1<<9));
	printk(BIOS_DEBUG, "done.\n");

	
#endif
	/* To enable the register 0xcf9 in the IO space
	 * bit [D5] is set in the amd8111 configuration register.
	 * The config. reg. is devBx41.  Register 0xcf9 allows
	 * hard reset capability to the system.  For the ACPI
	 * reset.reg values in fadt.c to work this register
	 * must be enabled.
	 */
	byte = pci_read_config8(dev, 0x41);
	pci_write_config8(dev, 0x41, byte | (1<<6)|(1<<5));
	
	/* power on after power fail */
	on = MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&on, "power_on_after_fail");
	byte = pci_read_config8(dev, PREVIOUS_POWER_STATE);
	byte &= ~0x40;
	if (!on) {
		byte |= 0x40;
	}
	pci_write_config8(dev, PREVIOUS_POWER_STATE, byte);
	printk(BIOS_INFO, "set power %s after power fail\n", on?"on":"off");

	/* switch serial irq logic from quiet mode to continuous
	 * mode for Winbond W83627HF Rev. 17
	 */
	byte = pci_read_config8(dev, 0x4a);
	pci_write_config8(dev, 0x4a, byte | (1<<6));
	
	/* Throttle the CPU speed down for testing */
	on = SLOW_CPU_OFF;
	get_option(&on, "slow_cpu");
	if(on) {
		pm10_bar = (pci_read_config16(dev, 0x58)&0xff00);
		outl(((on<<1)+0x10)  ,(pm10_bar + 0x10));
		dword = inl(pm10_bar + 0x10);
		on = 8-on;
		printk(BIOS_DEBUG, "Throttling CPU %2d.%1.1d percent.\n",
				(on*12)+(on>>1),(on&1)*5);
	}

#ifdef CONFIG_ACPI_TABLE
	pm_base = pci_read_config16(dev, 0x58) & 0xff00;
	printk(BIOS_DEBUG, "pm_base: 0x%04x\n",pm_base);
#endif

}

static void acpi_read_resources(struct device * dev)
{
	struct resource *resource;

	/* Handle the generic bars */
	pci_dev_read_resources(dev);

	/* Add the ACPI/SMBUS bar */
	resource = new_resource(dev, 0x58);
	resource->base  = 0;
	resource->size  = 256;
	resource->align = log2f(256);
	resource->gran  = log2f(256);
	resource->limit = 65536;
	resource->flags = IORESOURCE_IO;
	resource->index = 0x58;
}

static void acpi_enable_resources(struct device * dev)
{
	u8 byte;
	/* Enable the generic pci resources */
	pci_dev_enable_resources(dev);

	/* Enable the ACPI/SMBUS Bar */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1 << 7);
	pci_write_config8(dev, 0x41, byte);

	/* Set the class code */
	pci_write_config32(dev, 0x60, 0x06800000);
	
}

static void lpci_set_subsystem(struct device * dev, u16 vendor, u16 device)
{
	pci_write_config32(dev, 0x7c, 
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte  = lsmbus_recv_byte,
	.send_byte  = lsmbus_send_byte,
	.read_byte  = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

struct device_operations acpi = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			 .device = PCI_DEVICE_ID_AMD_8111_ACPI}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase3_chip_setup_dev	 = amd8111_enable,
	.phase4_read_resources	 = acpi_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = acpi_enable_resources,
	.phase6_init		 = acpi_init,
	.ops_pci		 = &lops_pci,
	.ops_smbus_bus 		 = &lops_smbus_bus,
};
