/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/smbus_ops.h>

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 data)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_write_byte(res->base, device, address, data);
}

static int lsmbus_block_write(struct device *dev, u8 cmd, u8 bytes, const u8 *buf)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_block_write(res->base, device, cmd, bytes, buf);
}

static int lsmbus_block_read(struct device *dev, u8 cmd, u8 bytes, u8 *buf)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_block_read(res->base, device, cmd, bytes, buf);
}

struct smbus_bus_operations lops_smbus_bus = {
	.read_byte	= lsmbus_read_byte,
	.write_byte	= lsmbus_write_byte,
	.block_read	= lsmbus_block_read,
	.block_write	= lsmbus_block_write,
};

void smbus_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	struct resource *res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = CONFIG_FIXED_SMBUS_IO_BASE;
	res->size = 32;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}
