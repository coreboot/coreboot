/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <bitops.h>
#include <arch/io.h>
#include "ck804.h"
#include "ck804_smbus.h"

static int lsmbus_recv_byte(device_t dev)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.u.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(device_t dev, uint8_t val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.u.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(device_t dev, uint8_t address)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.u.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(device_t dev, uint8_t address, uint8_t val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.u.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_write_byte(res->base, device, address, val);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte  = lsmbus_recv_byte,
	.send_byte  = lsmbus_send_byte,
	.read_byte  = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations smbus_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = 0,
	.scan_bus         = scan_static_bus,
	// .enable        = ck804_enable,
	.ops_pci          = &lops_pci,
	.ops_smbus_bus    = &lops_smbus_bus,
};

static const struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_SM,
};
