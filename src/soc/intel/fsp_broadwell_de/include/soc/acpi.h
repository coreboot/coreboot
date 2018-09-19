/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2016-2018 Siemens AG
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

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <arch/acpi.h>

void acpi_create_intel_hpet(acpi_hpet_t *hpet);
void acpi_fill_in_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
uint16_t get_pmbase(void);
unsigned long vtd_write_acpi_tables(struct device *const dev,
				    unsigned long current,
				    struct acpi_rsdp *const rsdp);

#endif /* _SOC_ACPI_H_ */
