/* Turn off machine check triggers when reading
 * pci space where there are no devices.
 * This is necessary when scaning the bus for
 * devices which is done by the kernel */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void misc_control_init(struct device *dev)
{
	uint32_t cmd;
	
	printk_debug("NB: Function 3 Misc Control.. ");
	cmd = pci_read_config32(dev, 0x44);
	cmd |= (1<<6) | (1<<25);
	pci_write_config32(dev, 0x44, cmd );

	printk_debug("done.\n");
}

static struct device_operations mcf3_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
};

static struct pci_driver mcf3_driver __pci_driver = {
	.ops    = &mcf3_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1103,
};

