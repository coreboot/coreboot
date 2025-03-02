/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <string.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/intel/smm_reloc.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <smp/node.h>
#include <soc/msr.h>
#include <soc/smmrelocate.h>
#include <soc/pci_devs.h>

void soc_ubox_store_resources(struct smm_pci_resource_info *slots, size_t size)
{
	struct device *devices[CONFIG_MAX_SOCKET] = {0};
	size_t devices_count = 0;
	struct device *dev = NULL;

	/*
	 * Collect all UBOX DFX devices. Depending on the actual socket count
	 * the bus numbers changed and the PCI segment group might be different.
	 * Pass all devices to SMM for platform lockdown.
	 */
	while ((dev = dev_find_device(PCI_VID_INTEL, UBOX_DFX_DEVID, dev))) {
		devices[devices_count++] = dev;
	}

	smm_pci_resource_store_fill_resources(slots, size, (const struct device **)devices, devices_count);
}
