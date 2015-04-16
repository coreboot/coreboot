/*
 * Copyright (C) 2015 Alexander Couzens <lynxis@fe80.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include "sata.h"

#include <arch/acpi.h>
#include <arch/acpigen.h>

/* e.g.
 * generate_sata_ssdt_ports("\_SB.PCI0.SATA", 0x3);
 * generates:
 * Scope (\_SB.PCI0.SATA)
 * {
 *     Device (PR00)
 *     {
 *         Name (_ADR, 0x0000FFFF)  // _ADR: Address
 *     }
 *
 *     Device (PR01)
 *     {
 *         Name (_ADR, 0x0001FFFF)  // _ADR: Address
 *     }
 * }
 *
 */
void generate_sata_ssdt_ports(const char *scope, uint32_t enable_map)
{
	int i;
	uint32_t bit;
	char port_name[4] = "PR00";

	acpigen_write_scope(scope);

	/* generate a device for every enabled port */
	for (i = 0; i < 32; i++) {
		bit = 1 << i;
		if (!(bit & enable_map))
			continue;

		port_name[2] = '0' + i / 10;
		port_name[3] = '0' + i % 10;

		acpigen_write_device(port_name);

		acpigen_write_name_dword("_ADR", 0xffff + i * 0x10000);
		acpigen_pop_len(); /* close PRT%d */
	}

	acpigen_pop_len(); /* close scope */
}
