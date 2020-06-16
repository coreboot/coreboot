/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/mcucfg.h>
#include <soc/mmu_operations.h>

DEFINE_BIT(MP0_CLUSTER_CFG0_L3_SHARE_EN, 9)
DEFINE_BIT(MP0_CLUSTER_CFG0_L3_SHARE_PRE_EN, 8)

void mtk_soc_disable_l2c_sram(void)
{
	unsigned long v;

	SET32_BITFIELDS(&mt8192_mcucfg->mp0_cluster_cfg0,
			MP0_CLUSTER_CFG0_L3_SHARE_EN, 0);
	dsb();

	__asm__ volatile ("mrs %0, S3_0_C15_C3_5" : "=r" (v));
	v |= (0xf << 4);
	__asm__ volatile ("msr S3_0_C15_C3_5, %0" : : "r" (v));
	dsb();

	do {
		__asm__ volatile ("mrs %0, S3_0_C15_C3_7" : "=r" (v));
	} while (((v >> 0x4) & 0xf) != 0xf);

	SET32_BITFIELDS(&mt8192_mcucfg->mp0_cluster_cfg0,
			MP0_CLUSTER_CFG0_L3_SHARE_PRE_EN, 0);
	dsb();
}
