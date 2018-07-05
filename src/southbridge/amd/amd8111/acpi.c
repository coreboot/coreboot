/*
 * This file is part of the coreboot project.
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
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <pc80/mc146818rtc.h>
#include <lib.h>
#include <arch/io.h>
#include "amd8111.h"
#include "amd8111_smbus.h"

#define PREVIOUS_POWER_STATE 0x43
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1
#define SLOW_CPU_OFF 0
#define SLOW_CPU__ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif


static int lsmbus_recv_byte(struct device *dev)
{
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(struct device *dev, uint8_t val)
{
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_send_byte(res->base, device, val);
}


static int lsmbus_read_byte(struct device *dev, uint8_t address)
{
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, uint8_t address, uint8_t val)
{
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_write_byte(res->base, device, address, val);
}

static int lsmbus_block_read(struct device *dev, uint8_t cmd, u8 bytes,
			     u8 *buffer)
{
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_block_read(res->base, device, cmd, bytes, buffer);
}

static int lsmbus_block_write(struct device *dev, uint8_t cmd, u8 bytes,
			      const u8 *buffer)
{
	unsigned device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, 0x58);

	return do_smbus_block_write(res->base, device, cmd, bytes, buffer);
}


#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
unsigned pm_base;
#endif

static void acpi_init(struct device *dev)
{
	uint8_t byte;
	uint16_t pm10_bar;
	int on;

#if 0
	uint16_t word;
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
	on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
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
	if (on) {
		pm10_bar = (pci_read_config16(dev, 0x58)&0xff00);
		outl(((on<<1)+0x10)  ,(pm10_bar + 0x10));
		inl(pm10_bar + 0x10);
		on = 8-on;
		printk(BIOS_DEBUG, "Throttling CPU %2d.%1.1d percent.\n",
				(on*12)+(on>>1),(on&1)*5);
	}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	pm_base = pci_read_config16(dev, 0x58) & 0xff00;
	printk(BIOS_DEBUG, "pm_base: 0x%04x\n",pm_base);
#endif

}

static void acpi_read_resources(struct device *dev)
{
	struct resource *resource;

	/* Handle the generic bars */
	pci_dev_read_resources(dev);

	/* Add the ACPI/SMBUS bar */
	resource = new_resource(dev, 0x58);
	resource->base  = 0;
	resource->size  = 256;
	resource->align = log2(256);
	resource->gran  = log2(256);
	resource->limit = 65536;
	resource->flags = IORESOURCE_IO;
	resource->index = 0x58;
}

static void acpi_enable_resources(struct device *dev)
{
	uint8_t byte;
	/* Enable the generic pci resources */
	pci_dev_enable_resources(dev);

	/* Enable the ACPI/SMBUS Bar */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1 << 7);
	pci_write_config8(dev, 0x41, byte);

	/* Set the class code */
	pci_write_config32(dev, 0x60, 0x06800000);

}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x7c,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte  = lsmbus_recv_byte,
	.send_byte  = lsmbus_send_byte,
	.read_byte  = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
	.block_read = lsmbus_block_read,
	.block_write= lsmbus_block_write,
};

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations acpi_ops  = {
	.read_resources   = acpi_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = acpi_enable_resources,
	.init             = acpi_init,
	.scan_bus         = scan_smbus,
	/*  We don't need amd8111_enable, chip ops takes care of it.
	 *  It could be useful if these devices were not
	 *  enabled by default.
	 */
//	.enable           = amd8111_enable,
	.ops_pci          = &lops_pci,
	.ops_smbus_bus    = &lops_smbus_bus,
};

static const struct pci_driver acpi_driver __pci_driver = {
	.ops    = &acpi_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_ACPI,
};
