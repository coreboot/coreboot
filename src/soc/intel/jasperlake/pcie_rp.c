/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/pcie_rp.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>

const struct pcie_rp_group pch_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,    .count = 8, .lcap_port_base = 1 },
	{ 0 }
};
