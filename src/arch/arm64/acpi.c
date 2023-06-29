/* SPDX-License-Identifier: GPL-2.0-only */
#include <acpi/acpi.h>

void arch_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->flags |= ACPI_FADT_HW_REDUCED_ACPI | ACPI_FADT_LOW_PWR_IDLE_S0;
}
