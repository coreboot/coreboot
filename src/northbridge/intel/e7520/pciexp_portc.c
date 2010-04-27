#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <arch/io.h>
#include "chip.h"

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

static struct device_operations pcie_ops  = {
        .read_resources   = pci_bus_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_bus_enable_resources,
        .init             = pcie_init,
        .scan_bus         = pciexp_scan_bridge,
	.reset_bus        = pci_bus_reset,
        .ops_pci          = 0,
};

static const struct pci_driver pci_driver __pci_driver = {
        .ops    = &pcie_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_PCIE_PC,
};


