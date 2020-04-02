/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

struct acpi_rsdp;

unsigned long mainboard_write_acpi_tables(struct device *device,
					  unsigned long start,
					  struct acpi_rsdp *rsdp);
