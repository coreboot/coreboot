#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include "chip.h"

#define ADM1026_DEVICE 0x2d	/* Either 0x2c or 0x2d or 0x2e */
#define ADM1026_REG_CONFIG1 0x00
#define CFG1_MONITOR     0x01
#define CFG1_INT_ENABLE  0x02
#define CFG1_INT_CLEAR   0x04
#define CFG1_AIN8_9      0x08
#define CFG1_THERM_HOT   0x10
#define CFT1_DAC_AFC     0x20
#define CFG1_PWM_AFC     0x40
#define CFG1_RESET       0x80
#define ADM1026_REG_CONFIG2 0x01
#define ADM1026_REG_CONFIG3 0x07

static void adm1026_enable_monitoring(device_t dev);

static void adm1026_init(device_t dev)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C) {
		if (ops_smbus_bus(get_pbus_smbus(dev))) {
			if (dev->bus->dev->path.type == DEVICE_PATH_I2C)
				smbus_set_link(dev);	// it is under mux
			adm1026_enable_monitoring(dev);
		}
	}
}

static void adm1026_enable_monitoring(device_t dev)
{
	int result;
	result = smbus_read_byte(dev, ADM1026_REG_CONFIG1);

	result = (result | CFG1_MONITOR) & ~(CFG1_INT_CLEAR | CFG1_RESET);
	result = smbus_write_byte(dev, ADM1026_REG_CONFIG1, result);

	result = smbus_read_byte(dev, ADM1026_REG_CONFIG1);
	if (!(result & CFG1_MONITOR)) {
		printk(BIOS_DEBUG, "ADM1026: monitoring would not enable");
	}
}

static void adm1026_noop(device_t dummy)
{
}

static struct device_operations adm1026_operations = {
	.read_resources = adm1026_noop,
	.set_resources = adm1026_noop,
	.enable_resources = adm1026_noop,
	.init = adm1026_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &adm1026_operations;
}

struct chip_operations drivers_i2c_adm1026_ops = {
	CHIP_NAME("adm1026")
	.enable_dev = enable_dev,
};
