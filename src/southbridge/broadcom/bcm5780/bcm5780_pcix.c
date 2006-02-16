/*
 * Copyright  2005 AMD
 *  by yinghai.lu@amd.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{       
        pci_write_config32(dev, 0x40, 
                ((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
        .set_subsystem = lpci_set_subsystem,
};

static struct device_operations ht_ops  = {
        .read_resources   = pci_bus_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_bus_enable_resources,
        .init             = 0 ,
        .scan_bus         = pci_scan_bridge,
        .reset_bus        = pci_bus_reset,
        .ops_pci          = &lops_pci,

};

static struct pci_driver ht_driver __pci_driver = {
	.ops    = &ht_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5780_PXB,
};

