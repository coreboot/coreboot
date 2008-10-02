/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"

static void usb1_init(struct device *dev)
{
	struct southbridge_nvidia_ck804_config const *conf = dev->chip_info;
	if (conf->usb1_hc_reset) {
		/*
		 * Somehow the warm reset does not really reset the USB
		 * controller. Later, during boot, when the Bus Master bit is
		 * set, the USB controller trashes the memory, causing weird
		 * misbehavior. Was detected on Sun Ultra40, where mptable
		 * was damaged.
		 */
		uint32_t bar0 = pci_read_config32(dev, 0x10);
		uint32_t *regs = (uint32_t *) (bar0 & ~0xfff);

		/* OHCI USB HCCommandStatus Register, HostControllerReset bit */
		regs[2] |= 1;
	}
}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations usb_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb1_init,
	// .enable        = ck804_enable,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver usb_driver __pci_driver = {
	.ops    = &usb_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_USB,
};
