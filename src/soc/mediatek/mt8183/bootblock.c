/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/pll.h>

void bootblock_soc_init(void)
{
	mt_pll_init();
}
