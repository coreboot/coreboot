/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include "x4x.h"

unsigned long northbridge_write_acpi_tables(const struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	unsigned long current;
	current = acpi_align_current(start);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
