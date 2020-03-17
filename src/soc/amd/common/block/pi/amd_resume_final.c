/*
 * This file is part of the coreboot project.
 *
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

#include <bootstate.h>
#include <amdblocks/agesawrapper_call.h>

static void agesawrapper_s3finalrestore(void *unused)
{
	do_agesawrapper(AMD_S3FINAL_RESTORE, "amds3finalrestore");
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY,
			agesawrapper_s3finalrestore, NULL);
