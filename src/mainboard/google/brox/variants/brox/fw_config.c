/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>

static void fw_config_handle(void *unused)
{
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
