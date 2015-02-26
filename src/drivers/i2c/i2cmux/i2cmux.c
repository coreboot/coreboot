#include <device/device.h>
#include <device/smbus.h>

static void i2cmux_set_link(struct device *dev, unsigned int link)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			smbus_write_byte(dev, 0x01, 1 << link);	// output value
			smbus_write_byte(dev, 0x03, 0);	// all output
		}
	}
}

static struct device_operations i2cmux_operations = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = DEVICE_NOOP,
	.scan_bus = scan_smbus,
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
