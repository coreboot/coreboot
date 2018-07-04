/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
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

static void slot_dev_read_resources(struct device *dev)
{
	struct resource *resource;

	resource = new_resource(dev, 0x10);
	resource->size = 1 << 23;
	resource->align = 22;
	resource->gran = 22;
	resource->limit = 0xffffffff;
	resource->flags |= IORESOURCE_MEM;

	resource = new_resource(dev, 0x14);
	resource->size = 1 << 23;
	resource->align = 22;
	resource->gran = 22;
	resource->limit = 0xffffffff;
	resource->flags |= IORESOURCE_MEM | IORESOURCE_PREFETCH;

	resource = new_resource(dev, 0x18);
	resource->size = 1 << 12;
	resource->align = 12;
	resource->gran = 12;
	resource->limit = 0xffff;
	resource->flags |= IORESOURCE_IO;
}

static struct device_operations slot_dev_ops = {
	.read_resources   = slot_dev_read_resources,
};

/* Add a dummy device to reserve I/O space for hotpluggable devices.  */
void intel_acpi_pcie_hotplug_scan_slot(struct bus *bus)
{
	struct device *slot;
	struct device_path slot_path = { .type = DEVICE_PATH_NONE };
	slot = alloc_dev(bus, &slot_path);
	slot->ops = &slot_dev_ops;
}
