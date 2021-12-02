/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <intelblocks/pcie_rp.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 4 },
	{ 0 }
};

static const struct pcie_rp_group pch_m_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 2 },
	{ 0 }
};

const struct pcie_rp_group *get_pch_pcie_rp_table(void)
{
	if (CONFIG(SOC_INTEL_ALDERLAKE_PCH_M))
		return pch_m_rp_groups;

	return pch_lp_rp_groups; /* Valid for PCH-P and PCH-N */
}

/*
 * ADL-P FSP define CPU RP as below:
 * RP1: PEG60 : 0:6:0 : x4 CPU Slot
 * RP2: PEG10 : 0:1:0 : x8 CPU Slot
 * RP3: PEG62 : 0:6:2 : x4 CPU Slot
 */
static const struct pcie_rp_group cpu_rp_groups[] = {
	{ .slot = SA_DEV_SLOT_CPU_6, .start = 0, .count = 1 },
	{ .slot = SA_DEV_SLOT_CPU_1, .start = 0, .count = 1 },
	{ .slot = SA_DEV_SLOT_CPU_6, .start = 2, .count = 1 },
	{ 0 }
};

static const struct pcie_rp_group cpu_m_rp_groups[] = {
	{ .slot = SA_DEV_SLOT_CPU_6, .start = 0, .count = 1 },
	{ 0 }
};

static const struct pcie_rp_group cpu_n_rp_groups[] = {
	{ 0 }
};

const struct pcie_rp_group *get_cpu_pcie_rp_table(void)
{
	if (CONFIG(SOC_INTEL_ALDERLAKE_PCH_M))
		return cpu_m_rp_groups;

	if (CONFIG(SOC_INTEL_ALDERLAKE_PCH_N))
		return cpu_n_rp_groups;

	return cpu_rp_groups;
}

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

	if (CONFIG_MAX_CPU_ROOT_PORTS && is_part_of_group(dev, cpu_rp_groups))
		return PCIE_RP_CPU;

	return PCIE_RP_UNKNOWN;
}
