/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootmode.h>
#include <drivers/intel/gma/opregion.h>
#include <stddef.h>

#include "vbt.h"

void *vbt_get(void)
{
	if (!CONFIG(RUN_FSP_GOP))
		return NULL;

	/* Normal mode and S3 resume path PEIM GFX init is not needed.
	 * Passing NULL as VBT will not make PEIM GFX to execute. */
	if (acpi_is_wakeup_s3())
		return NULL;
	if (!display_init_required())
		return NULL;
	return locate_vbt(NULL);
}
