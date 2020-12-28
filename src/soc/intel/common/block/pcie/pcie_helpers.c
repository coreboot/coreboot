/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <intelblocks/pcie_rp.h>
#include <stdint.h>

static uint32_t pcie_slot_enable_mask(unsigned int slot, unsigned int count)
{
	uint32_t mask = 0;
	unsigned int i;
	const struct device *dev;

	for (i = 0; i < count; i++) {
		dev = pcidev_on_root(slot, i);
		if (is_dev_enabled(dev))
			mask |= BIT(i);
	}

	return mask;
}

uint32_t pcie_rp_enable_mask(const struct pcie_rp_group *const groups)
{
	uint32_t mask = 0;
	uint32_t offset = 0;
	const struct pcie_rp_group *group;

	for (group = groups; group->count; ++group) {
		if (group->count + offset >= sizeof(mask) * 8) {
			printk(BIOS_ERR, "ERROR: %s: Root port count greater than mask size!\n",
			       __func__);
			break;
		}
		mask |= pcie_slot_enable_mask(group->slot, group->count) << offset;
		offset += group->count;
	}

	return mask;
}
