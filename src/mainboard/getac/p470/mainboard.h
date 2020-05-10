/* SPDX-License-Identifier: GPL-2.0-only */

struct acpi_rsdp;

unsigned long mainboard_write_acpi_tables(const struct device *device,
					  unsigned long start,
					  struct acpi_rsdp *rsdp);
