/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>

#include "pciehp.h"

void intel_acpi_pcie_hotplug_generator(u8 *hotplug_map, int port_number)
{
	int port;
	int have_hotplug = 0;

	for (port = 0; port < port_number; port++) {
		if (hotplug_map[port]) {
			have_hotplug = 1;
		}
	}

	if (!have_hotplug) {
		return;
	}

	for (port = 0; port < port_number; port++) {
		if (hotplug_map[port]) {
			char scope_name[] = "\\_SB.PCI0.RP0x";
			scope_name[sizeof("\\_SB.PCI0.RP0x") - 2] = '1' + port;
			acpigen_write_scope(scope_name);

			/*
			  Device (SLOT)
			  {
				Name (_ADR, 0x00)
				Method (_RMV, 0, NotSerialized)
				{
					Return (0x01)
				}
			  }
			*/

			acpigen_write_device("SLOT");

			acpigen_write_name_byte("_ADR", 0x00);

			acpigen_write_method("_RMV", 0);
			/* ReturnOp  */
			acpigen_emit_byte (0xa4);
			/* One  */
			acpigen_emit_byte (0x01);
			acpigen_pop_len();
			acpigen_pop_len();
			acpigen_pop_len();
		}
	}

	/* Method (_L01, 0, NotSerialized)
	{
		If (\_SB.PCI0.RP04.HPCS)
		{
			Sleep (100)
			Store (0x01, \_SB.PCI0.RP04.HPCS)
			If (\_SB.PCI0.RP04.PDC)
			{
				Store (0x01, \_SB.PCI0.RP04.PDC)
				Notify (\_SB.PCI0.RP04, 0x00)
			}
		}
	}

	*/
	acpigen_write_scope("\\_GPE");
	acpigen_write_method("_L01", 0);
	for (port = 0; port < port_number; port++) {
		if (hotplug_map[port]) {
			char reg_name[] = "\\_SB.PCI0.RP0x.HPCS";
			reg_name[sizeof("\\_SB.PCI0.RP0x") - 2] = '1' + port;
			acpigen_emit_byte(0xa0); /* IfOp. */
			acpigen_write_len_f();
			acpigen_emit_namestring(reg_name);

			/* Sleep (100) */
			acpigen_emit_byte(0x5b); /* SleepOp. */
			acpigen_emit_byte(0x22);
			acpigen_write_byte(100);

			/* Store (0x01, \_SB.PCI0.RP04.HPCS) */
			acpigen_emit_byte(0x70);
			acpigen_emit_byte(0x01);
			acpigen_emit_namestring(reg_name);

			memcpy(reg_name + sizeof("\\_SB.PCI0.RP0x.") - 1, "PDC", 4);

			/* If (\_SB.PCI0.RP04.PDC) */
			acpigen_emit_byte(0xa0); /* IfOp. */
			acpigen_write_len_f();
			acpigen_emit_namestring(reg_name);

			/* Store (0x01, \_SB.PCI0.RP04.PDC) */
			acpigen_emit_byte(0x70);
			acpigen_emit_byte(0x01);
			acpigen_emit_namestring(reg_name);

			reg_name[sizeof("\\_SB.PCI0.RP0x") - 1] = '\0';

			/* Notify(\_SB.PCI0.RP04, 0x00) */
			acpigen_emit_byte(0x86);
			acpigen_emit_namestring(reg_name);
			acpigen_emit_byte(0x00);
			acpigen_pop_len();
			acpigen_pop_len();
		}
	}
	acpigen_pop_len();
	acpigen_pop_len();

}
