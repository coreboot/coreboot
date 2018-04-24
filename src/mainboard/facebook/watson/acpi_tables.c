/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <arch/ioapic.h>
#include <soc/acpi.h>
#include <soc/iomap.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	u32 i;

	current = acpi_create_madt_lapics(current);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 8,
		IOXAPIC1_BASE_ADDRESS, 0);
	set_ioapic_id((u8 *)IOXAPIC1_BASE_ADDRESS, 8);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 9,
		IOXAPIC2_BASE_ADDRESS, 24);
	set_ioapic_id((u8 *)IOXAPIC2_BASE_ADDRESS, 9);

	current = acpi_madt_irq_overrides(current);

	for (i = 0; i < 16; i++)
		current += acpi_create_madt_lapic_nmi(
			(acpi_madt_lapic_nmi_t *)current, i, 0xD, 1);

	return current;
}
