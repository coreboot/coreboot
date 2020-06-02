/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOC_PICASSO_ACPI_H__
#define __SOC_PICASSO_ACPI_H__

#include <acpi/acpi.h>

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current, struct acpi_rsdp *rsdp);

void southbridge_inject_dsdt(const struct device *device);

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp);

const char *soc_acpi_name(const struct device *dev);

#endif /* __SOC_PICASSO_ACPI_H__ */
