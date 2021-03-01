/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <string.h>

#include "acpi_pirq_gen.h"

enum emit_type {
	EMIT_APIC,
	EMIT_PICM,
};

static int create_pirq_matrix(char matrix[32][4])
{
	struct device *dev;
	int num_devs = 0;

	for (dev = pcidev_on_root(0, 0); dev; dev = dev->sibling) {
		u8 pci_dev;
		u8 int_pin;

		pci_dev = PCI_SLOT(dev->path.pci.devfn);
		int_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN);

		if (int_pin == PCI_INT_NONE || int_pin > PCI_INT_D ||
				matrix[pci_dev][int_pin - PCI_INT_A]
				!= PIRQ_NONE)
			continue;

		matrix[pci_dev][int_pin - PCI_INT_A] =
			intel_common_map_pirq(dev, int_pin);
		printk(BIOS_SPEW, "ACPI_PIRQ_GEN: %s: pin=%d pirq=%d\n",
			dev_path(dev), int_pin - PCI_INT_A,
			matrix[pci_dev][int_pin - PCI_INT_A] - PIRQ_A);
		num_devs++;
	}
	return num_devs;
}

static void gen_pirq_route(const enum emit_type emit, const char *lpcb_path,
			char pci_int_mapping[32][4])
{
	int pci_dev, int_pin;
	char buffer[DEVICE_PATH_MAX];
	char pirq;

	for (pci_dev = 0; pci_dev < 32; pci_dev++) {
		for (int_pin = 0; int_pin < 4; int_pin++) {
			pirq = pci_int_mapping[pci_dev][int_pin];
			if (pirq == PIRQ_NONE)
				continue;

			if (emit == EMIT_APIC) {
				const unsigned int gsi = 16 + pirq - PIRQ_A;
				acpigen_write_PRT_GSI_entry(pci_dev, int_pin, gsi);
			} else {
				snprintf(buffer, sizeof(buffer),
					 "%s.LNK%c",
					 lpcb_path, 'A' + pirq - PIRQ_A);
				acpigen_write_PRT_source_entry(pci_dev, int_pin, buffer, 0);
			}
		}
	}
}

void intel_acpi_gen_def_acpi_pirq(const struct device *dev)
{
	const char *lpcb_path = acpi_device_path(dev);
	char pci_int_mapping[32][4];
	int num_devs;

	printk(BIOS_DEBUG, "Generating ACPI PIRQ entries\n");

	if (!lpcb_path) {
		printk(BIOS_ERR, "ACPI_PIRQ_GEN: Missing LPCB ACPI path\n");
		return;
	}

	memset(pci_int_mapping, 0, sizeof(pci_int_mapping));
	num_devs = create_pirq_matrix(pci_int_mapping);

	acpigen_write_scope("\\_SB.PCI0");
	acpigen_write_method("_PRT", 0);
	acpigen_write_if();
	acpigen_emit_namestring("PICM");
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(num_devs);
	gen_pirq_route(EMIT_APIC, lpcb_path, pci_int_mapping);
	acpigen_pop_len(); /* package */
	acpigen_write_else();
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(num_devs);
	gen_pirq_route(EMIT_PICM, lpcb_path, pci_int_mapping);
	acpigen_pop_len(); /* package */
	acpigen_pop_len(); /* else PICM */
	acpigen_pop_len(); /* _PRT */
	acpigen_pop_len(); /* \_SB */
}
