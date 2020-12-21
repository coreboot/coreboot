/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_ACPI_H_
#define _BAYTRAIL_ACPI_H_

#include <acpi/acpi.h>

int acpi_sci_irq(void);
unsigned long acpi_madt_irq_overrides(unsigned long current);

#endif /* _BAYTRAIL_ACPI_H_ */
