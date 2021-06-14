/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_ACPI_H_
#define _BROADWELL_ACPI_H_

#include <acpi/acpi.h>

unsigned long northbridge_write_acpi_tables(const struct device *dev,
	unsigned long current, struct acpi_rsdp *rsdp);
#endif
