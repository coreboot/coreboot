#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801cx.h"


static void ide_init(struct device *dev)
{
	/* Enable ide devices so the linux ide driver will work */
	uint16_t ideTimingConfig;
	int enable_primary = 1;
	int enable_secondary = 1;

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_PRI);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	if (enable_primary) {
		/* Enable first ide interface */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		printk(BIOS_DEBUG, "IDE0 ");
	}
	pci_write_config16(dev, IDE_TIM_PRI, ideTimingConfig);

    ideTimingConfig = pci_read_config16(dev, IDE_TIM_SEC);
    ideTimingConfig &= ~IDE_DECODE_ENABLE;
    if (enable_secondary) {
		/* Enable secondary ide interface */
        ideTimingConfig |= IDE_DECODE_ENABLE;
        printk(BIOS_DEBUG, "IDE1 ");
	}
    pci_write_config16(dev, IDE_TIM_SEC, ideTimingConfig);
}

static struct device_operations ide_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.scan_bus         = 0,
	.enable           = i82801cx_enable,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801CA_IDE,
};
