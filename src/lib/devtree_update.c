/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/bsd/compiler.h>
#include <devtree_update.h>

__weak void devtree_update(void)
{
}

static void run_devtree_update(void *unused)
{
	devtree_update();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, run_devtree_update, NULL);
