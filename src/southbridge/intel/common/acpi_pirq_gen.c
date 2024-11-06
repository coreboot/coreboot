/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include "acpi_pirq_gen.h"

static void gen_apic_route(const struct slot_pin_irq_map *pin_irq_map,
			   unsigned int map_count)
{
	for (unsigned int i = 0; i < map_count; i++)
		/*
		 * The reason for subtracting PCI_INT_A from the pin given is
		 * that PCI defines pins as 1-4, and _PRT uses 0-3.
		 */
		acpigen_write_PRT_GSI_entry(pin_irq_map[i].slot,
					    pin_irq_map[i].pin - PCI_INT_A,
					    pin_irq_map[i].apic_gsi);
}

static void gen_pic_route(const struct slot_pin_irq_map *pin_irq_map,
			  unsigned int map_count,
			  const struct pic_pirq_map *pirq_map)
{
	for (unsigned int i = 0; i < map_count; i++) {
		const enum pirq pirq = pin_irq_map[i].pic_pirq;
		const unsigned int pin = pin_irq_map[i].pin - PCI_INT_A;
		if (pirq == PIRQ_INVALID)
			continue;

		const size_t pirq_index = pirq_idx(pirq);
		if (pirq_map->type == PIRQ_GSI)
			acpigen_write_PRT_GSI_entry(pin_irq_map[i].slot,
						    pin,
						    pirq_map->gsi[pirq_index]);
		else
			acpigen_write_PRT_source_entry(pin_irq_map[i].slot,
						       pin,
						       pirq_map->source_path[pirq_index],
						       0);
	}
}

void intel_write_pci_PRT(const char *scope, const struct slot_pin_irq_map *pin_irq_map,
			 unsigned int map_count, const struct pic_pirq_map *pirq_map)
{
	acpigen_write_scope(scope);
	acpigen_write_method("_PRT", 0);
	acpigen_write_if();
	acpigen_emit_namestring("PICM");
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(map_count);
	gen_apic_route(pin_irq_map, map_count);
	acpigen_pop_len(); /* package */
	acpigen_write_else();
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(map_count);
	gen_pic_route(pin_irq_map, map_count, pirq_map);
	acpigen_pop_len(); /* package */
	acpigen_pop_len(); /* else PICM */
	acpigen_pop_len(); /* _PRT */
	acpigen_pop_len(); /* \_SB */
}

bool is_slot_pin_assigned(const struct slot_pin_irq_map *pin_irq_map,
			  unsigned int map_count, unsigned int slot,
			  enum pci_pin pin)
{
	for (size_t i = 0; i < map_count; i++) {
		if (pin_irq_map[i].slot == slot && pin_irq_map[i].pin == pin)
			return true;
	}

	return false;
}
