/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_ehci.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>

static void set_usb_over_current(struct device *dev)
{
	uint16_t map = USB_OC_DISABLE_ALL;

	if (dev->path.pci.devfn == XHCI_DEVFN) {
		if (mainboard_get_xhci_oc_map(&map) == 0) {
			xhci_pm_write32(XHCI_PM_INDIRECT_INDEX,
						XHCI_OVER_CURRENT_CONTROL);
			xhci_pm_write16(XHCI_PM_INDIRECT_DATA, map);
		}
	}

	if (dev->path.pci.devfn == EHCI1_DEVFN) {
		if (mainboard_get_ehci_oc_map(&map) == 0)
			pci_write_config16(dev, EHCI_OVER_CURRENT_CONTROL, map);
	}
}

int __weak mainboard_get_xhci_oc_map(uint16_t *map)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1;
}

int __weak mainboard_get_ehci_oc_map(uint16_t *map)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1;
}

struct device_operations stoneyridge_usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = set_usb_over_current,
	.scan_bus = scan_static_bus,
	.acpi_name = soc_acpi_name,
	.ops_pci = &pci_dev_ops_pci,
};
