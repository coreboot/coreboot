/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/xhci.h>

enum cb_err xhci_for_each_ext_cap(const struct device *device, void *context,
				  void (*callback)(void *context,
						   const struct xhci_ext_cap *cap))
{
	const struct resource *res;

	if (!device)
		return CB_ERR;

	res = probe_resource(device, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "%s: Unable to find BAR resource for %s\n", __func__,
		       dev_path(device));
		return CB_ERR;
	}

	return xhci_resource_for_each_ext_cap(res, context, callback);
}

enum cb_err xhci_for_each_supported_usb_cap(
	const struct device *device, void *context,
	void (*callback)(void *context, const struct xhci_supported_protocol *data))
{
	const struct resource *res;

	if (!device)
		return CB_ERR;

	res = probe_resource(device, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "%s: Unable to find BAR resource for %s\n", __func__,
		       dev_path(device));
		return CB_ERR;
	}

	return xhci_resource_for_each_supported_usb_cap(res, context, callback);
}
