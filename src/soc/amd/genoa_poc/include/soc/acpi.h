/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_GENOA_POC_ACPI_H
#define AMD_GENOA_POC_ACPI_H

#include <acpi/acpi.h>
#include <device/device.h>

#define ACPI_SCI_IRQ 9

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    struct acpi_rsdp *rsdp);

#endif /* AMD_GENOA_POC_ACPI_H */
