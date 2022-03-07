/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/i2c.h>
#include <soc/ramstage.h>
#include <soc/reg_access.h>

__weak void mainboard_gpio_i2c_init(struct device *dev)
{
	/* Initialize any of the GPIOs or I2C devices */
	printk(BIOS_SPEW, "WEAK; %s\n", __func__);
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= mainboard_gpio_i2c_init,
};

static const struct pci_driver gfx_driver __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= I2CGPIO_DEVID,
};
