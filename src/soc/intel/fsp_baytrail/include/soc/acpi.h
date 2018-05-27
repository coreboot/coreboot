/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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

#ifndef _BAYTRAIL_ACPI_H_
#define _BAYTRAIL_ACPI_H_

#include <arch/acpi.h>
#include <soc/nvs.h>
#include <device/device.h>

void acpi_create_intel_hpet(acpi_hpet_t * hpet);
void acpi_create_serialio_ssdt(acpi_header_t *ssdt);
void acpi_fill_in_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
void acpi_init_gnvs(global_nvs_t *gnvs);

#ifndef __SIMPLE_DEVICE__
unsigned long southcluster_write_acpi_tables(struct device *device,
					     unsigned long current,
					     struct acpi_rsdp *rsdp);
void southcluster_inject_dsdt(struct device *device);
#endif

#endif /* _BAYTRAIL_ACPI_H_ */
