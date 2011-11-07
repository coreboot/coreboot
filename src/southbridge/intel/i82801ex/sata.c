#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801ex.h"

#define SATA_PI		0x9
#define SATA_MAP	0x90
#define SATA_PCS	0x92
#define SATA_SRI	0xa0
#define SATA_SRD	0xa4

static void sata_init(struct device *dev)
{
  	printk(BIOS_DEBUG, "SATA init\n");
	/* SATA configuration */
	pci_write_config8(dev, PCICMD, 0x07);
	pci_write_config8(dev, SATA_PI, 0x8f);

	/* Set timings */
	pci_write_config16(dev, IDE_TIMP, 0x0a307);
	pci_write_config16(dev, IDE_TIMS, 0x0a307);

	/* Sync DMA */
	pci_write_config16(dev, SDMA_CNT, 0x000f);
	pci_write_config16(dev, SDMA_TIM, 0x1111);

	/* 66 mhz */
	pci_write_config16(dev, IDE_CONFIG, 0xf00f);

	/* Combine ide - sata configuration */
	pci_write_config8(dev, SATA_MAP, 0x0);

	/* port 0 & 1 enable */
	pci_write_config8(dev, SATA_PCS, 0x33);

	/* initialize SATA  */
	pci_write_config16(dev, SATA_SRI, 0x0018);
	pci_write_config32(dev, SATA_SRD, 0x00000264);
	pci_write_config16(dev, SATA_SRI, 0x0040);
	pci_write_config32(dev, SATA_SRD, 0x00220043);

}

static struct device_operations sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = sata_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static const struct pci_driver sata_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801ER_SATA,
};

static const struct pci_driver sata_driver_nr __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801EB_SATA,
};
