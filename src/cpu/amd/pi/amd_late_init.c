/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <arch/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/pi/agesawrapper_call.h>

static void agesawrapper_post_device(void *unused)
{
	if (acpi_is_wakeup_s3())
		return;

	AGESAWRAPPER(amdinitlate);

	if (!acpi_s3_resume_allowed())
		return;

	AGESAWRAPPER(amdS3Save);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT,
			agesawrapper_post_device, NULL);
