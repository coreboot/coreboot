#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>

static void i2cmux2_set_link(device_t dev, unsigned int link)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			smbus_send_byte(dev, link);	// output value
		}
	}
}

static void i2cmux2_noop(device_t dummy)
{
}

static struct device_operations i2cmux2_operations = {
	.read_resources = i2cmux2_noop,
	.set_resources = i2cmux2_noop,
	.enable_resources = i2cmux2_noop,
	.init = i2cmux2_noop,
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
