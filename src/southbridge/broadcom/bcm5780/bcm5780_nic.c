/*
 * Copyright  2005 AMD
 *  by yinghai.lu@amd.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>


static void nic_init(struct device *dev)
{

#if CONFIG_PCI_ROM_RUN == 1
	pci_dev_init(dev);// it will init option rom
#endif

}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
        pci_write_config32(dev, 0x40,
                ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
        .set_subsystem = lpci_set_subsystem,
};

static struct device_operations nic_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = nic_init,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};
static struct pci_driver nic_driver __pci_driver = {
	.ops    = &nic_ops,
	.vendor = PCI_VENDOR_ID_BROADCOM,
	.device = PCI_DEVICE_ID_BROADCOM_BCM5780_NIC,
};
static struct pci_driver nic1_driver __pci_driver = {
        .ops    = &nic_ops,
        .vendor = PCI_VENDOR_ID_BROADCOM,
        .device = PCI_DEVICE_ID_BROADCOM_BCM5780_NIC1,
};
