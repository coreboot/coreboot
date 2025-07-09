/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/lenovo/mec1653/mec1653.h>

void bootblock_mainboard_early_init(void)
{
	bootblock_ec_init();
}
