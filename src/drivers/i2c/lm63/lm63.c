#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>

static void lm63_init(device_t dev)
{
	int result;
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			if (dev->bus->dev->path.type == DEVICE_PATH_I2C)
				smbus_set_link(dev);	// it is under mux
			result = smbus_read_byte(dev, 0x03);
//                      result &= ~0x04;
			result |= 0x04;
			smbus_write_byte(dev, 0x03, result & 0xff);	// config lm63
		}
	}
}

static void lm63_noop(device_t dummy)
{
}

static struct device_operations lm63_operations = {
	.read_resources = lm63_noop,
	.set_resources = lm63_noop,
	.enable_resources = lm63_noop,
	.init = lm63_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &lm63_operations;
}

struct chip_operations drivers_i2c_lm63_ops = {
	CHIP_NAME("National Semiconductor LM63")
	.enable_dev = enable_dev,
};
