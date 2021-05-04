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

	if (routing_table) {
		*entries = routing_table_entries;
		return routing_table;
	}

	routing_table = fsp_find_extension_hob_by_guid(AMD_FSP_PCIE_DEVFUNC_REMAP_HOB_GUID.b,
						       &hob_size);

	if (routing_table == NULL || hob_size == 0) {
		printk(BIOS_ERR, "Couldn't find PCIe routing HOB.\n");
		return NULL;
	}

	routing_table_entries = hob_size / sizeof(struct pci_routing_info);

	for (size_t i = 0; i < routing_table_entries; ++i) {
		printk(BIOS_DEBUG, "%02x.%x: group: %u, swizzle: %u, irq: %u\n",
		       PCI_SLOT(routing_table[i].devfn), PCI_FUNC(routing_table[i].devfn),
		       routing_table[i].group, routing_table[i].swizzle, routing_table[i].irq);
	}

	*entries = routing_table_entries;

	return routing_table;
}
