/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/cpu_id.h>
#include <soc/pll.h>
#include <soc/pmif_spmi.h>

const struct spmi_device spmi_dev[] = {
	{
		.slvid = SPMI_SLAVE_7,
		.type = SUB_PMIC,
		.type_id = SUB_PMIC_ID,
	},
	{
		.slvid = SPMI_SLAVE_8,
		.type = BUCK_CPU,
		.type_id = BUCK_CPU_ID,
	},
};

int spmi_config_master(void)
{
	/* Enable SPMI */
	write32(&mtk_spmi_mst->mst_req_en, 1);

	return 0;
}

void pmif_spmi_config(struct pmif *arb)
{
	/* Clear all cmd permission for per channel */
	write32(&arb->mtk_pmif->inf_cmd_per_0, 0x888888AA);
	write32(&arb->mtk_pmif->inf_cmd_per_1, 0x88888888);
	write32(&arb->mtk_pmif->inf_cmd_per_2, 0x88888888);
	write32(&arb->mtk_pmif->inf_cmd_per_3, 0x88888888);
}

void pmif_spmi_iocfg(void)
{
	gpio_set_driving(GPIO(SPMI_P_SCL), GPIO_DRV_10_MA);
	gpio_set_driving(GPIO(SPMI_P_SDA), GPIO_DRV_10_MA);
}

size_t spmi_dev_cnt(void)
{
	static size_t cached_cnt;

	if (cached_cnt)
		return cached_cnt;

	cached_cnt = ARRAY_SIZE(spmi_dev);
	if (get_cpu_id() == MTK_CPU_ID_MT8189 &&
	    get_cpu_segment_id() == MTK_CPU_SEG_ID_MT8189G)
		cached_cnt = 1;

	return cached_cnt;
}
