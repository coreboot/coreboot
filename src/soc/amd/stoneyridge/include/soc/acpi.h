/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_STONEYRIDGE_ACPI_H
#define AMD_STONEYRIDGE_ACPI_H

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>

#if CONFIG(STONEYRIDGE_LEGACY_FREE)
	#define FADT_BOOT_ARCH ACPI_FADT_LEGACY_FREE
#else
	#define FADT_BOOT_ARCH (ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042)
#endif

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current, struct acpi_rsdp *rsdp);

const char *soc_acpi_name(const struct device *dev);

#endif /* AMD_STONEYRIDGE_ACPI_H */
