/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/pcie_rp.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>

/*
 * Document #815002 Panther Lake H Processor - 2.3 Device IDs - Table 8 "Other Device ID"
 * specifies that the first Thunderbolt PCIe root port number is 21. TBT's LCAP registers return
 * port index which starts from 21 (usually for other PCIe root ports index starts from
 * 1). Thus, keeping lcap_port_base 21 for TBT, so that coreboot's PCIe remapping logic can
 * return a correct index (0-based).
*/

static const struct pcie_rp_group tbt_rp_groups[] = {
	{ .slot = PCI_DEV_SLOT_TBT, .count = CONFIG_MAX_TBT_ROOT_PORTS, .lcap_port_base = 21 },
	{ 0 }
};

static const struct pcie_rp_group ptl_rp_groups[] = {
#if CONFIG(SOC_INTEL_WILDCATLAKE)
	{ .slot = PCI_DEV_SLOT_PCIE_1,	.count = 4, .lcap_port_base = 1 },
#else
	{ .slot = PCI_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
#endif
#if CONFIG(SOC_INTEL_PANTHERLAKE_U_H)
	{ .slot = PCI_DEV_SLOT_PCIE_2,	.count = 4, .lcap_port_base = 1 },
#else
	{ .slot = PCI_DEV_SLOT_PCIE_2, .count = 2, .lcap_port_base = 1 },
#endif
	{ 0 }
};

const struct pcie_rp_group *get_pcie_rp_table(void)
{
	return ptl_rp_groups;
}

const struct pcie_rp_group *get_tbt_pcie_rp_table(void)
{
	return tbt_rp_groups;
}

enum pcie_rp_type soc_get_pcie_rp_type(const struct device *dev)
{
	return PCIE_RP_PCH;
}

int soc_get_cpu_rp_vw_idx(const struct device *dev)
{
	return -1;
}
