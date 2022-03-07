/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/smbus.h>
#include <arch/ioapic.h>

#include "hudson.h"
#include "smbus.c"

#define NMI_OFF 0

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

/*
* HUDSON enables all USB controllers by default in SMBUS Control.
* HUDSON enables SATA by default in SMBUS Control.
*/

static void sm_init(struct device *dev)
{
	setup_ioapic(VIO_APIC_VADDR, CONFIG_MAX_CPUS);
}

static int lsmbus_recv_byte(struct device *dev)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(struct device *dev, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 val)
{
	u32 device;
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

static void hudson_sm_read_resources(struct device *dev)
{
}

static void hudson_sm_set_resources(struct device *dev)
{
}

static struct device_operations smbus_ops = {
	.read_resources = hudson_sm_read_resources,
	.set_resources = hudson_sm_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sm_init,
	.scan_bus = scan_smbus,
	.ops_pci = &pci_dev_ops_pci,
	.ops_smbus_bus = &lops_smbus_bus,
};
static const struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VID_AMD,
	/* PCI device ID is used on all discrete FCHs and Family 16h Models 00h-3Fh */
	.device = PCI_DID_AMD_SB900_SM,
};
