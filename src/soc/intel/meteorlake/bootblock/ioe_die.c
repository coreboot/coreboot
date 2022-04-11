/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/p2sb.h>
#include <soc/bootblock.h>

void bootblock_ioe_die_early_init(void)
{
	ioe_p2sb_enable_bar();
}
