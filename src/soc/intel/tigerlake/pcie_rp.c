/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <intelblocks/pcie_rp.h>
#include <soc/pci_devs.h>
#include <stdbool.h>

#define CPU_CPIE_VW_IDX_BASE		24

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 4, .lcap_port_base = 1 },
	{ 0 }
};

static const struct pcie_rp_group pch_h_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_2,	.count = 8, .lcap_port_base = 1 },
	{ 0 }
};

static const struct pcie_rp_group cpu_rp_groups[] = {
	{ .slot = SA_DEV_SLOT_PEG,	.start = 0, .count = 3, .lcap_port_base = 1 },
	{ .slot = SA_DEV_SLOT_CPU_PCIE,	.start = 0, .count = 1, .lcap_port_base = 1 },
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

const struct pcie_rp_group *soc_get_pch_rp_groups(void)
{
	if (CONFIG(SOC_INTEL_TIGERLAKE_PCH_H))
		return pch_h_rp_groups;
	else
		return pch_lp_rp_groups;
}

enum pcie_rp_type soc_get_pcie_rp_type(const struct device *dev)
{
	const struct pcie_rp_group *pch_rp_groups = soc_get_pch_rp_groups();

	if (is_part_of_group(dev, pch_rp_groups))
		return PCIE_RP_PCH;

	if (is_part_of_group(dev, cpu_rp_groups))
		return PCIE_RP_CPU;

	return PCIE_RP_UNKNOWN;
}

int soc_get_cpu_rp_vw_idx(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return -1;

	switch (dev->path.pci.devfn) {
	case SA_DEVFN_PEG1:
		return CPU_CPIE_VW_IDX_BASE + 2;
	case SA_DEVFN_PEG2:
		return CPU_CPIE_VW_IDX_BASE + 1;
	case SA_DEVFN_PEG3:
		return CPU_CPIE_VW_IDX_BASE;
	case SA_DEVFN_CPU_PCIE:
		return CPU_CPIE_VW_IDX_BASE + 3;
	default:
		return -1;
	}
}
