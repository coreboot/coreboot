/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef INTEL_COMMON_ACPI_PIRQ_GEN_H
#define INTEL_COMMON_ACPI_PIRQ_GEN_H

typedef enum pci_pin {
	PCI_INT_NONE = 0,
	PCI_INT_A,
	PCI_INT_B,
	PCI_INT_C,
	PCI_INT_D,
} pci_pin_t;

typedef enum pirq {
	PIRQ_NONE = 0,
	PIRQ_A,
	PIRQ_B,
	PIRQ_C,
	PIRQ_D,
	PIRQ_E,
	PIRQ_F,
	PIRQ_G,
	PIRQ_H,
} pirq_t;

void intel_acpi_gen_def_acpi_pirq(device_t dev);
enum pirq intel_common_map_pirq(const device_t dev, const pci_pin_t pci_pin);

#endif
