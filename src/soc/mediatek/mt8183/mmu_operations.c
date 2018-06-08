/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <soc/mcucfg.h>
#include <soc/mmu_operations.h>

void mtk_soc_disable_l2c_sram(void)
{
	/* Turn off L2C SRAM and return it to L2 cache. */
	write32(&mt8183_mcucfg->l2c_cfg_mp0, 7 << 8);
}
