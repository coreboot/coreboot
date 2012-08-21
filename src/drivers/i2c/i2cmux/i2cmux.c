#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>

static void i2cmux_set_link(device_t dev, unsigned int link)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			smbus_write_byte(dev, 0x01, 1 << link);	// output value
			smbus_write_byte(dev, 0x03, 0);	// all output
		}
	}
}

static void i2cmux_noop(device_t dummy)
{
}

static struct device_operations i2cmux_operations = {
	.read_resources = i2cmux_noop,
	.set_resources = i2cmux_noop,
	.enable_resources = i2cmux_noop,
	.init = i2cmux_noop,
	.scan_bus = scan_static_bus,
	.set_link = i2cmux_set_link,
};

static void enable_dev(struct device *dev)
{
	if (dev->link_list != NULL)
		dev->ops = &i2cmux_operations;
}

struct chip_operations drivers_i2c_i2cmux_ops = {
	CHIP_NAME("i2cmux")
	.enable_dev = enable_dev,
};
