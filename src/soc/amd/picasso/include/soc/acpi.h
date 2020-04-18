/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef __SOC_PICASSO_ACPI_H__
#define __SOC_PICASSO_ACPI_H__

#include <arch/acpi.h>

#ifndef FADT_PM_PROFILE
	#define FADT_PM_PROFILE PM_UNSPECIFIED
#endif

unsigned long southbridge_write_acpi_tables(struct device *device,
		unsigned long current, struct acpi_rsdp *rsdp);

void southbridge_inject_dsdt(struct device *device);

const char *soc_acpi_name(const struct device *dev);

#endif /* __SOC_PICASSO_ACPI_H__ */
