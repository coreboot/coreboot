#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "amd8111.h"

static void ide_init(struct device *dev)
{

	/* Enable ide devices so the linux ide driver will work */
	uint16_t word;
	uint8_t byte;
	int enable_a=1, enable_b=1;

	word = pci_read_config16(dev, 0x40);
	/* Ensure prefetch is disabled */
	word &= ~((1 << 15) | (1 << 13));
	if (enable_b) {
		/* Enable secondary ide interface */
		word |= (1<<0);
		printk_debug("IDE1 ");
	}
	if (enable_a) {
		/* Enable primary ide interface */
		word |= (1<<1);
		printk_debug("IDE0 ");
	}

	word |= (1<<12);
	word |= (1<<14);

	pci_write_config16(dev, 0x40, word);


        byte = 0x20 ; // Latency: 64-->32
        pci_write_config8(dev, 0xd, byte);

	word = 0x0f;
	pci_write_config16(dev, 0x42, word);
}

static struct device_operations ide_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.scan_bus         = 0,
};

static struct pci_driver ide_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_IDE,
};

