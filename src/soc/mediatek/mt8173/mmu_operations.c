/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/mmu_operations.h>

void mtk_soc_after_dram(void)
{
	mtk_mmu_disable_l2c_sram();
}

void mtk_soc_disable_l2c_sram(void)
{
	/* Return L2C SRAM back to L2 cache. Set it to 512KiB which is the max
	 * available L2 cache for A53 in MT8173. */
	write32(&mt8173_mcucfg->mp0_ca7l_cache_config, 3 << 8);
	/* turn off the l2c sram clock */
	write32(&mt8173_infracfg->infra_pdn0, L2C_SRAM_PDN);
}
