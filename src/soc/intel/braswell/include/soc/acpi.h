/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <acpi/acpi.h>

int acpi_sci_irq(void);
void acpi_create_serialio_ssdt(acpi_header_t *ssdt);
unsigned long southcluster_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);

#endif /* _SOC_ACPI_H_ */
