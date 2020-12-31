/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/pcie_rp.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 4 },
	{ 0 }
};

const struct pcie_rp_group *get_pch_pcie_rp_table(void)
{
	return pch_lp_rp_groups;
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

const struct pcie_rp_group *get_cpu_pcie_rp_table(void)
{
	return cpu_rp_groups;
}
