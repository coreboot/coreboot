/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/xdci.h>
#include <security/vboot/vboot_common.h>
#include <stdbool.h>

__weak void soc_xdci_init(struct device *dev) { /* no-op */ }

bool xdci_can_enable(unsigned int xdci_devfn)
{
	/* Enable xDCI controller if enabled in devicetree and allowed */
	if (!vboot_can_enable_udc()) {
		devfn_disable(pci_root_bus(), xdci_devfn);
		return false;
	}
	return is_devfn_enabled(xdci_devfn);
}

struct device_operations usb_xdci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_xdci_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_XDCI,
	PCI_DID_INTEL_APL_XDCI,
	PCI_DID_INTEL_CNL_LP_XDCI,
	PCI_DID_INTEL_GLK_XDCI,
	PCI_DID_INTEL_CNP_H_XDCI,
	PCI_DID_INTEL_ICP_LP_XDCI,
	PCI_DID_INTEL_CMP_LP_XDCI,
	PCI_DID_INTEL_CMP_H_XDCI,
	PCI_DID_INTEL_TGP_LP_XDCI,
	PCI_DID_INTEL_TGP_H_XDCI,
	PCI_DID_INTEL_MCC_XDCI,
	PCI_DID_INTEL_JSP_XDCI,
	PCI_DID_INTEL_ADP_P_XDCI,
	PCI_DID_INTEL_ADP_S_XDCI,
	PCI_DID_INTEL_ADP_M_XDCI,
	0
};

static const struct pci_driver pch_usb_xdci __pci_driver = {
	.ops	 = &usb_xdci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices	 = pci_device_ids,
};
