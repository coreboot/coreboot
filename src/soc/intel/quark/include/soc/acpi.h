/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <acpi/acpi.h>
#include <acpi/acpigen.h>

void acpi_fill_in_fadt(acpi_fadt_t *fadt);

#endif /* _SOC_ACPI_H_ */
