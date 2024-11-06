/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <stdio.h>
#include <stdlib.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba_pirq.h>
#include <southbridge/intel/common/rcba.h>

#define MAX_SLOT 31
#define MIN_SLOT 19

static const u32 pirq_dir_route_reg[MAX_SLOT - MIN_SLOT + 1] = {
	D19IR, D20IR, D21IR, D22IR, D23IR, 0, D25IR,
	D26IR, D27IR, D28IR, D29IR, D30IR, D31IR,
};

static enum pirq map_pirq(const struct device *dev, const enum pci_pin pci_pin)
{
	u8 slot = PCI_SLOT(dev->path.pci.devfn);
	u8 shift = 4 * (pci_pin - PCI_INT_A);
	u8 pirq;
	u16 reg;

	if (pci_pin < PCI_INT_A || pci_pin > PCI_INT_D) {
		printk(BIOS_ERR, "ACPI_PIRQ_GEN: Slot %d PCI pin %d out of bounds\n",
			slot, pci_pin);
		return PIRQ_INVALID;
	}

	/* Slot 24 should not exist and has no D24IR but better be safe here */
	if (slot < MIN_SLOT || slot > MAX_SLOT || slot == 24) {
		/* non-PCH devices use 1:1 mapping. */
		return (enum pirq)pci_pin;
	}

	reg = pirq_dir_route_reg[slot - MIN_SLOT];

	pirq = (RCBA16(reg) >> shift) & 0x7;

	return (enum pirq)(pirq + PIRQ_A);
}

void intel_acpi_gen_def_acpi_pirq(const struct device *lpc)
{
	struct slot_pin_irq_map *pin_irq_map;
	const char *lpcb_path = acpi_device_path(lpc);
	struct pic_pirq_map pirq_map = {0};
	unsigned int map_count = 0;
	int i;

	if (!lpcb_path) {
		printk(BIOS_ERR, "ACPI_PIRQ_GEN: Missing LPCB ACPI path\n");
		return;
	}

	printk(BIOS_DEBUG, "Generating ACPI PIRQ entries\n");

	pin_irq_map = calloc(MAX_SLOTS * PCI_INT_MAX, sizeof(struct slot_pin_irq_map));
	pirq_map.type = PIRQ_SOURCE_PATH;
	for (i = 0; i < PIRQ_COUNT; i++)
		snprintf(pirq_map.source_path[i], sizeof(pirq_map.source_path[i]),
			 "%s.LNK%c", lpcb_path, 'A' + i);

	for (struct device *dev = pcidev_on_root(0, 0); dev; dev = dev->sibling) {
		const u8 pci_dev = PCI_SLOT(dev->path.pci.devfn);
		const u8 int_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN);

		if (int_pin < PCI_INT_A || int_pin > PCI_INT_D)
			continue;

		if (is_slot_pin_assigned(pin_irq_map, map_count, pci_dev, int_pin))
			continue;

		enum pirq pirq = map_pirq(dev, int_pin);
		if (pirq == PIRQ_INVALID)
			continue;

		pin_irq_map[map_count].slot = pci_dev;
		pin_irq_map[map_count].pin = (enum pci_pin)int_pin;
		pin_irq_map[map_count].pic_pirq = pirq;
		/* PIRQs are mapped to GSIs starting at 16 */
		pin_irq_map[map_count].apic_gsi = 16 + pirq_idx(pirq);
		printk(BIOS_SPEW, "ACPI_PIRQ_GEN: %s: pin=%d pirq=%zd\n",
		       dev_path(dev), int_pin - PCI_INT_A,
		       pirq_idx(pin_irq_map[map_count].pic_pirq));
		map_count++;
	}

	intel_write_pci_PRT("\\_SB.PCI0", pin_irq_map, map_count, &pirq_map);

	free(pin_irq_map);
}
