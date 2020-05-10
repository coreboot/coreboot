/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTEL_COMMON_ACPI_PIRQ_GEN_H
#define INTEL_COMMON_ACPI_PIRQ_GEN_H

enum pci_pin {
	PCI_INT_NONE = 0,
	PCI_INT_A,
	PCI_INT_B,
	PCI_INT_C,
	PCI_INT_D,
};

enum pirq {
	PIRQ_NONE = 0,
	PIRQ_A,
	PIRQ_B,
	PIRQ_C,
	PIRQ_D,
	PIRQ_E,
	PIRQ_F,
	PIRQ_G,
	PIRQ_H,
};

void intel_acpi_gen_def_acpi_pirq(const struct device *dev);
enum pirq intel_common_map_pirq(const struct device *dev,
				const enum pci_pin pci_pin);

#endif
