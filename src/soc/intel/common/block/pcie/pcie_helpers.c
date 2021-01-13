/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <intelblocks/pcie_rp.h>
#include <stdint.h>

static uint32_t pcie_slot_enable_mask(const struct pcie_rp_group *group)
{
	uint32_t mask = 0;
	unsigned int fn;
	unsigned int i;
	const struct device *dev;

	for (i = 0, fn = rp_start_fn(group); i < group->count; i++, fn++) {
		dev = pcidev_on_root(group->slot, fn);
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
		mask |= pcie_slot_enable_mask(group) << offset;
		offset += group->count;
	}

	return mask;
}
