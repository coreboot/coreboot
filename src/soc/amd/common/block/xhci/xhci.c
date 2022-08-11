/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/xhci.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <soc/xhci.h>

void soc_xhci_store_resources(struct smm_pci_resource_info *slots, size_t count)
{
	const struct device *devices[] = SOC_XHCI_DEVICES;
	size_t devices_count;

	for (devices_count = 0; devices_count < ARRAY_SIZE(devices); devices_count++) {
		if (!devices[devices_count])
			break;
	}

	smm_pci_resource_store_fill_resources(slots, count, &devices[0], devices_count);
}
