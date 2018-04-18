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

#include <arch/acpigen.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <string.h>

#include "acpi_pirq_gen.h"

enum emit_type {
	EMIT_NONE,
	EMIT_APIC,
	EMIT_PICM,
};

static size_t enumerate_root_pci_pins(const enum emit_type emit,
				      const char *lpcb_path)
{
	char buffer[DEVICE_PATH_MAX];
	device_t dev;
	pci_pin_t prev_int_pin = PCI_INT_NONE;
	u8 prev_pci_dev = 0;
	size_t num_devs = 0;

	for (dev = all_devices; dev; dev = dev->next) {
		u8 pci_dev;
		u8 int_pin;
		pirq_t pirq;

		if (dev->path.type != DEVICE_PATH_PCI ||
		    dev->bus->secondary != 0)
			continue;

		pci_dev = PCI_SLOT(dev->path.pci.devfn);
		int_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN);

		if (int_pin == PCI_INT_NONE || int_pin > PCI_INT_D)
			continue;

		pirq = intel_common_map_pirq(dev, int_pin);
		if (emit == EMIT_NONE)  /* Only print on the first pass */
			printk(BIOS_SPEW, "ACPI_PIRQ_GEN: %s: pin=%d pirq=%d\n",
				dev_path(dev), int_pin, pirq);

		if (pirq == PIRQ_NONE)
			continue;

		/* Avoid duplicate entries */
		if (prev_pci_dev == pci_dev && prev_int_pin == int_pin) {
			continue;
		} else {
			prev_int_pin = int_pin;
			prev_pci_dev = pci_dev;
		}
		if (emit != EMIT_NONE) {
			acpigen_write_package(4);
			acpigen_write_dword((pci_dev << 16) | 0xffff);
			acpigen_write_dword(int_pin - PCI_INT_A);
			if (emit == EMIT_APIC) {
				acpigen_write_dword(0);
				acpigen_write_dword(16 + (pirq - PIRQ_A));
			} else {
				snprintf(buffer, sizeof(buffer),
					"%s.LNK%c",
					lpcb_path, 'A' + pirq - PIRQ_A);
				acpigen_emit_namestring(buffer);
				acpigen_write_dword(0);
			}
			acpigen_pop_len();
		}
		num_devs++;
	}
	return num_devs;
}

void intel_acpi_gen_def_acpi_pirq(device_t dev)
{
	const char *lpcb_path = acpi_device_path(dev);
	const size_t num_devs = enumerate_root_pci_pins(EMIT_NONE, lpcb_path);

	if (!lpcb_path)
		die("ACPI_PIRQ_GEN: Missing LPCB ACPI path\n");

	acpigen_write_scope("\\_SB.PCI0");
	acpigen_write_method("_PRT", 0);
	acpigen_write_if();
	acpigen_emit_namestring("PICM");
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(num_devs);
	enumerate_root_pci_pins(EMIT_APIC, lpcb_path);
	acpigen_pop_len(); /* package */
	acpigen_pop_len(); /* if PICM */
	acpigen_write_else();
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(num_devs);
	enumerate_root_pci_pins(EMIT_PICM, lpcb_path);
	acpigen_pop_len(); /* package */
	acpigen_pop_len(); /* else PICM */
	acpigen_pop_len(); /* _PRT */
	acpigen_pop_len(); /* \_SB */
}
