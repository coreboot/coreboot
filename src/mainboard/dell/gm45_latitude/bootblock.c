/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/dell/mec5035/mec5035.h>

void bootblock_mainboard_early_init(void)
{
	mec5035_early_init();
}
