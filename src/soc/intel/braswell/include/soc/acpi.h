/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <arch/acpi.h>
#include <soc/nvs.h>

void acpi_create_serialio_ssdt(acpi_header_t *ssdt);
void acpi_fill_in_fadt(acpi_fadt_t *fadt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
void acpi_init_gnvs(global_nvs_t *gnvs);
void southcluster_inject_dsdt(struct device *device);
unsigned long southcluster_write_acpi_tables(struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);

#endif /* _SOC_ACPI_H_ */
