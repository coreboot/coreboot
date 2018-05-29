/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20170227	// OEM revision
)
{
	/* Sleep states */
	Name (\_S0, Package (0x04) { 0x00, 0x00, 0x00, 0x00 })
	Name (\_S5, Package (0x04) { 0x02, 0x02, 0x02, 0x02 })

	/* Interrupt model */
	Method (_PIC, 1) {
		Store (Arg0, \_SB.PCI0.ISAC.APIC)
	}

	Scope (\_SB) {
		/* PCI bus */
		Device (PCI0) {
			#include <northbridge/via/cn700/acpi/hostbridge.asl>
			#include <southbridge/via/vt8237r/acpi/lpc.asl>
			#include <southbridge/via/vt8237r/acpi/default_irq_route.asl>
		}
	}
}
