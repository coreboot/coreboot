#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>

static void i2cmux2_set_link(struct device *dev, unsigned int link)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			smbus_send_byte(dev, link);	// output value
		}
	}
}

static struct device_operations i2cmux2_operations = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = DEVICE_NOOP,
	.scan_bus = scan_static_bus,
	.set_link = i2cmux2_set_link,
};

static void enable_dev(struct device *dev)
{
	if (dev->link_list != NULL)
		dev->ops = &i2cmux2_operations;
}

struct chip_operations drivers_i2c_i2cmux2_ops = {
	CHIP_NAME("i2cmux2")
	.enable_dev = enable_dev,
};
