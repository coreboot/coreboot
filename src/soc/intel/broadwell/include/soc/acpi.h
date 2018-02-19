/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _BROADWELL_ACPI_H_
#define _BROADWELL_ACPI_H_

#include <arch/acpi.h>
#include <soc/nvs.h>

/* P-state configuration */
#define PSS_MAX_ENTRIES			8
#define PSS_RATIO_STEP			2
#define PSS_LATENCY_TRANSITION		10
#define PSS_LATENCY_BUSMASTER		10

void acpi_create_intel_hpet(acpi_hpet_t *hpet);
void acpi_fill_in_fadt(acpi_fadt_t *fadt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
void acpi_init_gnvs(global_nvs_t *gnvs);
unsigned long northbridge_write_acpi_tables(struct device *dev,
	unsigned long current, struct acpi_rsdp *rsdp);
#endif
