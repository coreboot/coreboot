/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <amdblocks/agesawrapper_call.h>

static void agesawrapper_s3finalrestore(void *unused)
{
	do_agesawrapper(AMD_S3FINAL_RESTORE, "amds3finalrestore");
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY,
			agesawrapper_s3finalrestore, NULL);
