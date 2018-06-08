/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2014 - 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _DENVERTON_NS_ACPI_H_
#define _DENVERTON_NS_ACPI_H_

#include <arch/acpi.h>
#include <soc/nvs.h>

void acpi_create_intel_hpet(acpi_hpet_t *hpet);
void acpi_create_serialio_ssdt(acpi_header_t *ssdt);
void acpi_fill_in_fadt(acpi_fadt_t *fadt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
void acpi_init_gnvs(global_nvs_t *gnvs);
unsigned long southcluster_write_acpi_tables(struct device *device,
					     unsigned long current,
					     struct acpi_rsdp *rsdp);
void southcluster_inject_dsdt(struct device *device);

#endif /* _DENVERTON_NS_ACPI_H_ */
