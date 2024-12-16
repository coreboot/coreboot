/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/fw_cfg.h>

#include "../qemu-i440fx/acpi.h"

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	/* Do not advertise SMI even if installed. */
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
}
