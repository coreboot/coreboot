/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/ioapic.h>
#include <northbridge/amd/nb_common.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* TODO: Remove the hardcode */
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC2_ADDR);

	return current;
}
