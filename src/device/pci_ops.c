/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>

u8 *const pci_mmconf = (void *)(uintptr_t)CONFIG_ECAM_MMCONF_BASE_ADDRESS;

/**
 * Given a device, a capability type, and a last position, return the next
 * matching capability. Always start at the head of the list.
 *
 * @param dev Pointer to the device structure.
 * @param cap PCI_CAP_LIST_ID of the PCI capability we're looking for.
 * @param last Location of the PCI capability register to start from.
 * @return The next matching capability.
 */
u16 pci_s_find_next_capability(pci_devfn_t dev, u16 cap, u16 last)
{
	u16 pos = 0;
	u16 status;
	int reps = 48;

	status = pci_s_read_config16(dev, PCI_STATUS);
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	u8 hdr_type = pci_s_read_config8(dev, PCI_HEADER_TYPE);
	switch (hdr_type & 0x7f) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		pos = PCI_CAPABILITY_LIST;
		break;
	case PCI_HEADER_TYPE_CARDBUS:
		pos = PCI_CB_CAPABILITY_LIST;
		break;
	default:
		return 0;
	}

	pos = pci_s_read_config8(dev, pos);
	while (reps-- && (pos >= 0x40)) { /* Loop through the linked list. */
		int this_cap;

		pos &= ~3;
		this_cap = pci_s_read_config8(dev, pos + PCI_CAP_LIST_ID);
		if (this_cap == 0xff)
			break;

		if (!last && (this_cap == cap))
			return pos;

		if (last == pos)
			last = 0;

		pos = pci_s_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
	}
	return 0;
}

/**
 * Given a device, and a capability type, return the next matching
 * capability. Always start at the head of the list.
 *
 * @param dev Pointer to the device structure.
 * @param cap PCI_CAP_LIST_ID of the PCI capability we're looking for.
 * @return The next matching capability.
 */
u16 pci_s_find_capability(pci_devfn_t dev, u16 cap)
{
	return pci_s_find_next_capability(dev, cap, 0);
}

void __noreturn pcidev_die(void)
{
	die("PCI: dev is NULL!\n");
}

bool pci_dev_is_wake_source(pci_devfn_t dev)
{
	unsigned int pm_cap;
	uint16_t pmcs;

	pm_cap = pci_s_find_capability(dev, PCI_CAP_ID_PM);
	if (!pm_cap)
		return false;

	pmcs = pci_s_read_config16(dev, pm_cap + PCI_PM_CTRL);

	/* PCI Device is a wake source if PME_ENABLE and PME_STATUS are set in PMCS register. */
	return (pmcs & PCI_PM_CTRL_PME_ENABLE) && (pmcs & PCI_PM_CTRL_PME_STATUS);
}
