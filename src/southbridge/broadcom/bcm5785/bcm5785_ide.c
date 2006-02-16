/*
 * Copyright  2005 AMD
 *  by yinghai.lu@amd.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "bcm5785.h"

static void bcm5785_ide_read_resources(device_t dev)
{
        struct resource *res;
        unsigned long index;

        /* Get the normal pci resources of this device */
        pci_dev_read_resources(dev);

        /* BAR */
        pci_get_resource(dev, 0x64);

        compact_resources(dev);
}

static void ide_init(struct device *dev)
{
	uint16_t word;


}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
        pci_write_config32(dev, 0x40,
                ((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
        .set_subsystem = lpci_set_subsystem,
};

static struct device_operations ide_ops  = {
	.read_resources   = bcm5785_ide_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.scan_bus         = 0,
//	.enable           = bcm5785_enable,
	.ops_pci          = &lops_pci,
};

static struct pci_driver ide_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5785_IDE,
};

