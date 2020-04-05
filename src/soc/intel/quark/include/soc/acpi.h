/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <arch/acpi.h>
#include <arch/acpigen.h>

void acpi_fill_in_fadt(acpi_fadt_t *fadt);

#endif /* _SOC_ACPI_H_ */
