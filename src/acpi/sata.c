/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_sata.h>

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
	char port_name[5] = "PR00";

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
