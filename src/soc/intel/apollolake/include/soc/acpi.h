/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _SOC_APOLLOLAKE_ACPI_H_
#define _SOC_APOLLOLAKE_ACPI_H_

#include <arch/acpi.h>

void soc_fill_common_fadt(acpi_fadt_t * fadt);

unsigned long southbridge_write_acpi_tables(device_t device,
                unsigned long current, struct acpi_rsdp *rsdp);

#endif	/* _SOC_APOLLOLAKE_ACPI_H_ */
