/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/fsp.h>
#include <fsp/api.h>

void amd_fsp_early_init(void)
{
	fsp_memory_init(acpi_is_wakeup_s3());
}
