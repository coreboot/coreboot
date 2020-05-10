/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_ACPI_H_
#define _BAYTRAIL_ACPI_H_

#include <acpi/acpi.h>
#include <soc/nvs.h>

void acpi_fill_in_fadt(acpi_fadt_t *fadt);
unsigned long acpi_madt_irq_overrides(unsigned long current);
void acpi_init_gnvs(global_nvs_t *gnvs);

#endif /* _BAYTRAIL_ACPI_H_ */
