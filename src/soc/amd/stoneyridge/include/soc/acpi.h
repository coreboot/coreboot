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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_STONEYRIDGE_ACPI_H__
#define __SOC_STONEYRIDGE_ACPI_H__

#include <arch/acpi.h>

#if IS_ENABLED(CONFIG_STONEYRIDGE_LEGACY_FREE)
	#define FADT_BOOT_ARCH ACPI_FADT_LEGACY_FREE
#else
	#define FADT_BOOT_ARCH (ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042)
#endif

#ifndef FADT_PM_PROFILE
	#define FADT_PM_PROFILE PM_UNSPECIFIED
#endif

unsigned long southbridge_write_acpi_tables(struct device *device,
		unsigned long current, struct acpi_rsdp *rsdp);

void southbridge_inject_dsdt(struct device *device);

const char *soc_acpi_name(const struct device *dev);

#endif /* __SOC_STONEYRIDGE_ACPI_H__ */
