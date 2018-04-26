#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>

#define ADM1027_REG_CONFIG1	0x40
#define CFG1_STRT		0x01
#define CFG1_LOCK		0x02
#define CFG1_RDY		0x04
#define CFG1_FSPD		0x08
#define CFG1_VXI		0x10
#define CFT1_FSPDIS		0x20
#define CFG1_TODIS		0x40
#define CFG1_VCC		0x80
#define ADM1027_REG_CONFIG2	0x73
#define ADM1027_REG_CONFIG3	0x78

static void adm1027_enable_monitoring(struct device *dev)
{
	int result;

	result = smbus_read_byte(dev, ADM1027_REG_CONFIG1);

	if (!(result & CFG1_RDY)) {
		printk(BIOS_DEBUG, "ADM1027: monitoring not ready\n");
		return;
	}
	result = (result | CFG1_STRT);
	result = smbus_write_byte(dev, ADM1027_REG_CONFIG1, result);

	result = smbus_read_byte(dev, ADM1027_REG_CONFIG1);
	if (!(result & CFG1_STRT)) {
		printk(BIOS_DEBUG, "ADM1027: monitoring would not enable\n");
		return;
	}
	printk(BIOS_DEBUG, "ADM1027: monitoring enabled\n");
}

static void adm1027_init(struct device *dev)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			if (dev->bus->dev->path.type == DEVICE_PATH_I2C)
				smbus_set_link(dev);	// it is under mux
			adm1027_enable_monitoring(dev);
		}
	}
}

static struct device_operations adm1027_operations = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = adm1027_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &adm1027_operations;
}

struct chip_operations drivers_i2c_adm1027_ops = {
	CHIP_NAME("adm1027")
	.enable_dev = enable_dev,
};
