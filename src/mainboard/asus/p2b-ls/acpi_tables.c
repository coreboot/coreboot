/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* mainboard has no ioapic */
	return current;
}
