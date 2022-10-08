/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/amd_pci_util.h>
#include <commonlib/bsd/helpers.h>
#include <soc/pci_devs.h>
#include <stddef.h>

/* See AMD PPR 55570 - IOAPIC Initialization for the table that AGESA sets up */
const struct pci_routing_info pci_routing_table[] = {
	{PCIE_GPP_0_DEVFN, 0, PCI_SWIZZLE_ABCD, 0x10},
	{PCIE_GPP_1_DEVFN, 1, PCI_SWIZZLE_ABCD, 0x11},
	{PCIE_GPP_2_DEVFN, 2, PCI_SWIZZLE_ABCD, 0x12},
	{PCIE_GPP_3_DEVFN, 3, PCI_SWIZZLE_ABCD, 0x13},
	{PCIE_GPP_4_DEVFN, 4, PCI_SWIZZLE_ABCD, 0x10},
	{PCIE_GPP_5_DEVFN, 5, PCI_SWIZZLE_ABCD, 0x11},
	{PCIE_GPP_6_DEVFN, 6, PCI_SWIZZLE_ABCD, 0x12},
	{PCIE_GPP_A_DEVFN, 7, PCI_SWIZZLE_ABCD, 0x13},
	{PCIE_GPP_B_DEVFN, 7, PCI_SWIZZLE_CDAB, 0x0C},
};

const struct pci_routing_info *get_pci_routing_table(size_t *entries)
{
	*entries = ARRAY_SIZE(pci_routing_table);
	return pci_routing_table;
}
