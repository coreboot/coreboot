/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <stdlib.h>
#include <types.h>

enum pcie_swizzle_pin {
	PIN_A,
	PIN_B,
	PIN_C,
	PIN_D,
};

static const uint8_t pcie_swizzle_table[][4] = {
	{PIN_A, PIN_B, PIN_C, PIN_D},
	{PIN_B, PIN_C, PIN_D, PIN_A},
	{PIN_C, PIN_D, PIN_A, PIN_B},
	{PIN_D, PIN_A, PIN_B, PIN_C},
};

const struct pci_routing_info *get_pci_routing_info(unsigned int devfn)
{
	const struct pci_routing_info *routing_info;
	size_t entries = 0;

	routing_info = get_pci_routing_table(&entries);

	if (!routing_info || !entries)
		return NULL;

	for (size_t i = 0; i < entries; ++i, ++routing_info)
		if (routing_info->devfn == devfn)
			return routing_info;

	printk(BIOS_ERR, "Failed to find PCIe routing info for dev: %#x, fn: %#x\n",
	       PCI_SLOT(devfn), PCI_FUNC(devfn));

	return NULL;
}

unsigned int pci_calculate_irq(const struct pci_routing_info *routing_info,
					      unsigned int pin)
{
	unsigned int irq;

	if (routing_info->swizzle >= ARRAY_SIZE(pcie_swizzle_table))
		die("%s: swizzle %u out of bounds\n", __func__, routing_info->swizzle);

	if (pin >= ARRAY_SIZE(pcie_swizzle_table[routing_info->swizzle]))
		die("%s: pin %u out of bounds\n", __func__, pin);

	irq = routing_info->group * 4;
	irq += pcie_swizzle_table[routing_info->swizzle][pin];

	return irq;
}

void populate_pirq_data(void)
{
	const struct pci_routing_info *routing_table, *routing_entry;
	size_t entries = 0;
	struct pirq_struct *pirq;
	unsigned int irq;

	routing_table = get_pci_routing_table(&entries);

	if (!routing_table || !entries)
		return;

	pirq = calloc(entries, sizeof(*pirq));

	if (!pirq) {
		printk(BIOS_ERR, "%s: Allocation failed\n", __func__);
		return;
	}

	for (size_t i = 0; i < entries; ++i) {
		routing_entry = &routing_table[i];

		pirq[i].devfn = routing_entry->devfn;
		for (size_t j = 0; j < 4; ++j) {
			irq = pci_calculate_irq(routing_entry, j);

			pirq[i].PIN[j] = irq % 8;
		}
	}

	pirq_data_ptr = pirq;
	pirq_data_size = entries;
}
