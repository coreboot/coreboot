/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <intelblocks/pcie_rp.h>
#include <soc/pci_devs.h>

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8 },
	{ .slot = PCH_DEV_SLOT_PCIE_2,	.count = 4 },
	{ 0 }
};

static const struct pcie_rp_group cpu_rp_groups[] = {
	{ .slot = SA_DEV_SLOT_PEG,	.start = 0, .count = 3 },
	{ .slot = SA_DEV_SLOT_CPU_PCIE,	.start = 0, .count = 1 },
	{ 0 }
};

static bool is_part_of_group(const struct device *dev,
			     const struct pcie_rp_group *groups)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return false;

	const unsigned int slot_to_find = PCI_SLOT(dev->path.pci.devfn);
	const unsigned int fn_to_find = PCI_FUNC(dev->path.pci.devfn);
	const struct pcie_rp_group *group;
	unsigned int i;
	unsigned int fn;

	for (group = groups; group->count; ++group) {
		for (i = 0, fn = rp_start_fn(group); i < group->count; i++, fn++) {
			if (slot_to_find == group->slot && fn_to_find == fn)
				return true;
		}
	}

	return false;
}

enum pcie_rp_type soc_get_pcie_rp_type(const struct device *dev)
{
	if (is_part_of_group(dev, pch_lp_rp_groups))
		return PCIE_RP_PCH;

	if (is_part_of_group(dev, cpu_rp_groups))
		return PCIE_RP_CPU;

	return PCIE_RP_UNKNOWN;
}
