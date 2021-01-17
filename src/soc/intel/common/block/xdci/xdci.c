/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/xdci.h>
#include <security/vboot/vboot_common.h>

__weak void soc_xdci_init(struct device *dev) { /* no-op */ }

int xdci_can_enable(void)
{
	return vboot_can_enable_udc();
}

static struct device_operations usb_xdci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_xdci_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_XDCI,
	PCI_DEVICE_ID_INTEL_CNL_LP_XDCI,
	PCI_DEVICE_ID_INTEL_GLK_XDCI,
	PCI_DEVICE_ID_INTEL_SPT_LP_XDCI,
	PCI_DEVICE_ID_INTEL_CNP_H_XDCI,
	PCI_DEVICE_ID_INTEL_ICP_LP_XDCI,
	PCI_DEVICE_ID_INTEL_CMP_LP_XDCI,
	PCI_DEVICE_ID_INTEL_CMP_H_XDCI,
	PCI_DEVICE_ID_INTEL_TGP_LP_XDCI,
	PCI_DEVICE_ID_INTEL_MCC_XDCI,
	PCI_DEVICE_ID_INTEL_JSP_XDCI,
	PCI_DEVICE_ID_INTEL_ADP_P_XDCI,
	PCI_DEVICE_ID_INTEL_ADP_S_XDCI,
	PCI_DEVICE_ID_INTEL_ADP_M_XDCI,
	0
};

static const struct pci_driver pch_usb_xdci __pci_driver = {
	.ops	 = &usb_xdci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices	 = pci_device_ids,
};
