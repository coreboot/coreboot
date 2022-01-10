/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <bootstate.h>
#include <fsp/api.h>

static void start_fsps_preload(void *unused)
{
	preload_fsps();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, start_fsps_preload, NULL);
