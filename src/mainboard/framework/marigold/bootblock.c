/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <mainboard/bootblock.h>

void bootblock_mainboard_early_init(void)
{
	mainboard_configure_early_gpios();
}
