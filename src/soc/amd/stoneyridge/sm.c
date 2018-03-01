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
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <pc80/mc146818rtc.h>
#include <arch/io.h>
#include <cpu/x86/lapic.h>
#include <arch/ioapic.h>
#include <stdlib.h>
#include <soc/southbridge.h>
#include <soc/smbus.h>

#define NMI_OFF 0

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

/*
* The southbridge enables all USB controllers by default in SMBUS Control.
* The southbridge enables SATA by default in SMBUS Control.
*/

static void sm_init(device_t dev)
{
	setup_ioapic(VIO_APIC_VADDR, CONFIG_MAX_CPUS);
}

static int lsmbus_recv_byte(device_t dev)
{
	u8 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(device_t dev, u8 val)
{
	u8 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(device_t dev, u8 address)
{
	u8 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(device_t dev, u8 address, u8 val)
{
	u8 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_write_byte(res->base, device, address, val);
}
static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte = lsmbus_recv_byte,
	.send_byte = lsmbus_send_byte,
	.read_byte = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};
static struct device_operations smbus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = pci_dev_enable_resources,
	.init = sm_init,
	.scan_bus = scan_smbus,
	.ops_pci = &lops_pci,
	.ops_smbus_bus = &lops_smbus_bus,
};
static const struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CZ_SMBUS,
};
