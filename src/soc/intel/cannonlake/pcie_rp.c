/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/pcie_rp.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
	{ 0 }
};

static const struct pcie_rp_group pch_h_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_2,	.count = 8, .lcap_port_base = 1 },
	{ 0 }
};

const struct pcie_rp_group *get_pch_pcie_rp_table(void)
{
	if (CONFIG(SOC_INTEL_CANNONLAKE_PCH_H))
		return pch_h_rp_groups;

	return pch_lp_rp_groups;
}
