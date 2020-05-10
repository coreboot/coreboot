/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <intelblocks/lpc_lib.h>

void bootblock_mainboard_init(void)
{
	lpc_configure_pads();
}
