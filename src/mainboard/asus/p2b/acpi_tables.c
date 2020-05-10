/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* mainboard has no ioapic */
	return current;
}
