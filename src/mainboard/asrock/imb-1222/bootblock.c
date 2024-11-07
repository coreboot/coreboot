/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <mainboard/superio.h>

void bootblock_mainboard_early_init(void)
{
	mainboard_superio_init();
}
