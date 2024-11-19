/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <amdblocks/smn.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <types.h>

#define MAX_BRIDGES	20

#define SMN_DEVICE_REMAP_BASE_ADDR		0x13b100b8
#define SMN_PCI_INTERRUPT_ROUTING_BASE_ADDR	0x14300040

union iohc_nb_prog_device_remap {
	struct {
		uint32_t fn_num	  :  3; /* [ 2.. 0] */
		uint32_t dev_num  :  5; /* [ 7.. 3] */
		uint32_t          : 24; /* [31.. 8] */
	};
	uint32_t raw;
};

union ioapic_br_irq_routing {
	struct {
		uint32_t intr_grp  :  3; /* [ 2.. 0] */
		uint32_t           :  1; /* [ 3.. 3] */
		uint32_t intr_swz  :  2; /* [ 5.. 4] */
		uint32_t           : 10; /* [15.. 6] */
		uint32_t intr_map  :  5; /* [20..16] */
		uint32_t           : 11; /* [31..21] */
	};
	uint32_t raw;
};

const struct pci_routing_info *get_pci_routing_table(size_t *entries)
{
	static bool table_initialized;
	static struct pci_routing_info routing_table[MAX_BRIDGES];

	union iohc_nb_prog_device_remap dev_remap;
	union ioapic_br_irq_routing ioapic_routing;

	if (table_initialized) {
		*entries = MAX_BRIDGES;
		return routing_table;
	}

	/*
	 * For Phoenix, the logical and physical bridge numbers map 1:1, which may not be
	 * the case for other SoCs.
	 */
	for (size_t i = 0; i < MAX_BRIDGES; ++i) {
		dev_remap.raw = smn_read32(SMN_DEVICE_REMAP_BASE_ADDR
			+ sizeof(uint32_t) * i);
		routing_table[i].devfn = (dev_remap.dev_num << 3) | dev_remap.fn_num;
	}

	for (size_t i = 0; i < MAX_BRIDGES; ++i) {
		ioapic_routing.raw = smn_read32(SMN_PCI_INTERRUPT_ROUTING_BASE_ADDR
			+ sizeof(uint32_t) * i);
		routing_table[i].group = ioapic_routing.intr_grp;
		routing_table[i].swizzle = ioapic_routing.intr_swz;
		routing_table[i].bridge_irq = ioapic_routing.intr_map;
		printk(BIOS_DEBUG, "%02x.%x: group: %u, swizzle: %u, irq: %u\n",
		       PCI_SLOT(routing_table[i].devfn),
		       PCI_FUNC(routing_table[i].devfn),
		       routing_table[i].group,
		       routing_table[i].swizzle,
		       routing_table[i].bridge_irq);
	}
	table_initialized = true;

	*entries = MAX_BRIDGES;
	return routing_table;
}
