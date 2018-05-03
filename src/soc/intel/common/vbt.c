/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cbfs.h>
#include <compiler.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <bootmode.h>
#include <bootstate.h>

#include "vbt.h"
#include <drivers/intel/gma/opregion.h>

void *vbt_get(void)
{
	if (!IS_ENABLED(CONFIG_RUN_FSP_GOP))
		return NULL;

	/* Normal mode and S3 resume path PEIM GFX init is not needed.
	 * Passing NULL as VBT will not make PEIM GFX to execute. */
	if (acpi_is_wakeup_s3())
		return NULL;
	if (!display_init_required())
		return NULL;
	return locate_vbt(NULL);
}
