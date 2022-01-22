/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <fsp/util.h>
#include <FspGuids.h>
#include <types.h>

const struct pci_routing_info *get_pci_routing_table(size_t *entries)
{
	static const struct pci_routing_info *routing_table;
	static size_t routing_table_entries;
	size_t hob_size = 0;
	const struct {
		uint32_t num_of_entries;
		struct pci_routing_info routing_table[];
	} __packed *routing_hob;

	if (routing_table) {
		*entries = routing_table_entries;
		return routing_table;
	}

	routing_hob = fsp_find_extension_hob_by_guid(AMD_FSP_PCIE_DEVFUNC_REMAP_HOB_GUID.b,
						       &hob_size);

	if (routing_hob == NULL || hob_size == 0 || routing_hob->num_of_entries == 0) {
		printk(BIOS_ERR, "Couldn't find valid PCIe interrupt routing HOB.\n");
		return NULL;
	}

	routing_table = routing_hob->routing_table;
	routing_table_entries = routing_hob->num_of_entries;

	for (size_t i = 0; i < routing_table_entries; ++i) {
		printk(BIOS_DEBUG, "%02x.%x: group: %u, swizzle: %u, irq: %u\n",
		       PCI_SLOT(routing_table[i].devfn), PCI_FUNC(routing_table[i].devfn),
		       routing_table[i].group, routing_table[i].swizzle, routing_table[i].irq);
	}

	*entries = routing_table_entries;

	return routing_table;
}
