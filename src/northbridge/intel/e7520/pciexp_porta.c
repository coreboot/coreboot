#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <arch/io.h>
#include "chip.h"
#include <reset.h>

typedef struct northbridge_intel_e7520_config config_t;

static void pcie_init(struct device *dev)
{
        config_t *config;

        /* Get the chip configuration */
        config = dev->chip_info;

        if(config->intrline) {
                pci_write_config32(dev, 0x3c, config->intrline);
        }

}

static unsigned int pcie_scan_bridge(struct device *dev, unsigned int max)
{
	uint16_t val;
	uint16_t ctl;
	int flag = 0;
	do {
		val = pci_read_config16(dev, 0x76);
		printk(BIOS_DEBUG, "pcie porta 0x76: %02x\n", val);
		if((val & (1<<10) )&&(!flag)) { /* training error */
			ctl = pci_read_config16(dev, 0x74);
			pci_write_config16(dev, 0x74, (ctl | (1<<5)));
			val = pci_read_config16(dev, 0x76);
			printk(BIOS_DEBUG, "pcie porta reset 0x76: %02x\n", val);
			flag=1;
			hard_reset();
		}
	} while	( val & (3<<10) );
	return pciexp_scan_bridge(dev, max);
}

static struct device_operations pcie_ops  = {
        .read_resources   = pci_bus_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_bus_enable_resources,
        .init             = pcie_init,
        .scan_bus         = pcie_scan_bridge,
	.reset_bus        = pci_bus_reset,
        .ops_pci          = 0,
};

static const struct pci_driver pci_driver __pci_driver = {
        .ops    = &pcie_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_PCIE_PA,
};


