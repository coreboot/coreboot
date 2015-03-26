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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/acpi.h>
#include <bootstate.h>

#include <northbridge/amd/agesa/agesawrapper.h>

#if CONFIG_AMD_SB_CIMX
#include <sb_cimx.h>
#endif

static void agesawrapper_post_device(void *unused)
{
	if (acpi_is_wakeup_s3())
		return;

	agesawrapper_amdinitlate();

#if CONFIG_AMD_SB_CIMX
	sb_Late_Post();
#endif
	if (!acpi_s3_resume_allowed())
		return;

	agesawrapper_amdS3Save();
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT,
			agesawrapper_post_device, NULL);
