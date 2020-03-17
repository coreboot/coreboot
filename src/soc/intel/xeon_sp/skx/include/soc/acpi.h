/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <arch/acpi.h>
#include <soc/nvs.h>

#define MEM_BLK_COUNT      0x140
typedef struct {
	uint8_t   buf[32];
} MEM_BLK;

void acpi_create_serialio_ssdt(acpi_header_t *ssdt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
void acpi_init_gnvs(global_nvs_t *gnvs);
unsigned long northbridge_write_acpi_tables(struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);
void uncore_inject_dsdt(void);
void motherboard_fill_fadt(acpi_fadt_t *fadt);

#endif /* _SOC_ACPI_H_ */
