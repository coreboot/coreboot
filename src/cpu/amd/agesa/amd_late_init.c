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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/acpi.h>
#include <bootstate.h>
#include <console/console.h>

#include "agesawrapper.h"

void get_bus_conf(void);

static void agesawrapper_post_device(void *unused)
{
	u32 status;
	if (acpi_is_wakeup_s3())
		return;

	status = agesawrapper_amdinitlate();
	if (status) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitlate failed: %x \n", status);
	}

	if (!acpi_s3_resume_allowed())
		return;

	status = agesawrapper_amdS3Save();
	if (status) {
		printk(BIOS_DEBUG, "agesawrapper_amds3save failed: %x \n", status);
	}
}

BOOT_STATE_INIT_ENTRIES(agesa_bscb) = {
	BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT,
	                      agesawrapper_post_device, NULL),
};
