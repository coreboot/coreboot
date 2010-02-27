#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801dx.h"


static void ide_init(struct device *dev)
{
#if ICH5_SATA_ADDRESS_MAP<=1
	/* Enable ide devices so the linux ide driver will work */
	uint16_t word;
	uint8_t byte;
	int enable_a=1, enable_b=1;


	word = pci_read_config16(dev, 0x40);
	word &= ~((1 << 15));
	if (enable_a) {
		/* Enable first ide interface */
		word |= (1<<15);
		printk_debug("IDE0 ");
	}
	pci_write_config16(dev, 0x40, word);

        word = pci_read_config16(dev, 0x42);
        word &= ~((1 << 15));
        if (enable_b) {
                /* Enable secondary ide interface */
                word |= (1<<15);
                printk_debug("IDE1 ");
        }
        pci_write_config16(dev, 0x42, word);
#endif

}

static struct device_operations ide_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.scan_bus         = 0,
	.enable           = i82801dx_enable,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DBM_IDE,
};

