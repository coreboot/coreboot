
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "cs5536.h"

static void southbridge_init(struct device *dev)
{
	printk_err("cs5536: %s\n", __FUNCTION__);
	setup_i8259(); 
}

static void southbridge_enable(struct device *dev)
{
	printk_err("%s: dev is %p\n", __FUNCTION__, dev);
}

static void cs5536_pci_dev_enable_resources(device_t dev)
{
	printk_err("cs5536.c: %s()\n", __FUNCTION__);
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations southbridge_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cs5536_pci_dev_enable_resources,
	.init             = southbridge_init,
	.enable           = southbridge_enable,
	.scan_bus         = scan_static_bus,
};

static struct pci_driver cs5535_pci_driver __pci_driver = {
	.ops 	= &southbridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CS5536_ISA
};
