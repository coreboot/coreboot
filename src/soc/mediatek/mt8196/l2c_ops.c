/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 9
 */

#include <device/mmio.h>
#include <soc/mmu_operations.h>

DEFINE_BIT(MP0_CLUSTER_CFG0_L3_SHARE_FULLNHALF, 0)
DEFINE_BIT(MP0_CLUSTER_CFG0_L3_SHARE_EN, 1)
DEFINE_BIT(MP0_CLUSTER_CFG0_L3_SHARE_PRE_EN, 2)

#define MP0_CLUSTER_CFG0	0x0C000060
#define CLUST_DIS_VAL		0x3
#define CLUST_DIS_SHIFT		0x4

void mtk_soc_disable_l2c_sram(void)
{
	unsigned long v;

	uint32_t *mp0_cluster_cfg0 = (void *)(MP0_CLUSTER_CFG0);

	SET32_BITFIELDS(mp0_cluster_cfg0,
			MP0_CLUSTER_CFG0_L3_SHARE_EN, 0);
	dsb();

	__asm__ volatile ("mrs %0, S3_0_C15_C3_5" : "=r" (v));
	v |= (CLUST_DIS_VAL << CLUST_DIS_SHIFT);
	__asm__ volatile ("msr S3_0_C15_C3_5, %0" : : "r" (v));
	dsb();

	do {
		__asm__ volatile ("mrs %0, S3_0_C15_C3_7" : "=r" (v));
	} while (((v >> CLUST_DIS_SHIFT) & CLUST_DIS_VAL) != CLUST_DIS_VAL);

	SET32_BITFIELDS(mp0_cluster_cfg0,
			MP0_CLUSTER_CFG0_L3_SHARE_PRE_EN, 0);

	SET32_BITFIELDS(mp0_cluster_cfg0,
			MP0_CLUSTER_CFG0_L3_SHARE_FULLNHALF, 0);

	dsb();
}
