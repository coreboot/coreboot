/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/dell/mec5035/mec5035.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_superio(void)
{
	mec5035_early_init();
}
