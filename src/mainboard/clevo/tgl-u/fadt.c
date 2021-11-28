/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <intelblocks/acpi.h>

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	/* Control method power button (EC) */
	fadt->flags |= ACPI_FADT_POWER_BUTTON;
}
