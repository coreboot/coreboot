/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <soc/mcucfg.h>
#include <soc/mmu_operations.h>

void mtk_soc_disable_l2c_sram(void)
{
	/* Turn off L2C SRAM and return it to L2 cache. */
	write32(&mt8183_mcucfg->l2c_cfg_mp0, 7 << 8);
}
