/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 8.1
 */

#include <console/console.h>
#include <soc/mtcmos.h>
#include <soc/spm.h>
#include <soc/spm_mtcmos.h>

static const struct bus_protect bp_ufs[] = {
	{
		.clr_addr = &mtk_vlpcfg->bus_vlp_topaxi_protecten_clr,
		.set_addr = &mtk_vlpcfg->bus_vlp_topaxi_protecten_set,
		.rdy_addr = &mtk_vlpcfg->bus_vlp_topaxi_protecten_sta1,
		.mask = BIT(6),
	},
	{
		.clr_addr = &mtk_infracfg_ao->perisys_protect.clr,
		.set_addr = &mtk_infracfg_ao->perisys_protect.set,
		.rdy_addr = &mtk_infracfg_ao->perisys_protect.ready,
		.mask = BIT(4),
	},
	{
		.clr_addr = &mtk_vlpcfg->bus_vlp_topaxi_protecten_clr,
		.set_addr = &mtk_vlpcfg->bus_vlp_topaxi_protecten_set,
		.rdy_addr = &mtk_vlpcfg->bus_vlp_topaxi_protecten_sta1,
		.mask = BIT(5),
	},
};

static const struct bus_protect bp_mminfra[] = {
	{
		.clr_addr = &mtk_infracfg_ao->emisys_protect.clr,
		.mask = BIT(20) | BIT(21),
	},
	{
		.clr_addr = &mtk_infracfg_ao->infrasys_protect[0].clr,
		.mask = BIT(16),
	},
	{
		.clr_addr = &mtk_infracfg_ao->mmsys_protect[1].clr,
		.mask = BIT(0) | BIT(7) | BIT(8) | BIT(9) | BIT(10) |
			BIT(11) | BIT(12) | BIT(13) | BIT(14) | BIT(15),
	},
	{
		.clr_addr = &mtk_infracfg_ao->infrasys_protect[1].clr,
		.mask = BIT(11),
	},
	{
		.clr_addr = &mtk_infracfg_ao->mmsys_protect[1].clr,
		.mask = BIT(1) | BIT(2) | BIT(3),
	},
};

static const struct bus_protect bp_ssusb[] = {
	{
		.clr_addr = &mtk_infracfg_ao->perisys_protect.clr,
		.mask = BIT(7),
	},
};

static const struct power_domain_data pd_plat[] = {
	{
		/* ufs0 */
		.pwr_con = &mtk_spm->ufs0_pwr_con,
		.pwr_sta_mask = BIT(4),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
		.bp_steps = ARRAY_SIZE(bp_ufs),
		.bp_table = bp_ufs,
	},
	{
		/* ufs0_phy */
		.pwr_con = &mtk_spm->ufs0_phy_pwr_con,
		.pwr_sta_mask = BIT(5),
	},
	{
		/* mm_infra */
		.pwr_con = &mtk_spm->mm_infra_pwr_con,
		.pwr_sta_mask = BIT(30),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
		.bp_steps = ARRAY_SIZE(bp_mminfra),
		.bp_table = bp_mminfra,
	},
	{
		/* ssusb */
		.pwr_con = &mtk_spm->ssusb_pwr_con,
		.pwr_status = &mtk_spm->pwr_status_msb,
		.pwr_status_2nd = &mtk_spm->pwr_status_msb_2nd,
		.pwr_sta_mask = BIT(10),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
		.bp_steps = ARRAY_SIZE(bp_ssusb),
		.bp_table = bp_ssusb,
	},
};

void spm_power_on(void)
{
	write32(&mtk_spm->poweron_config_set, SPM_REGWR_CFG_KEY | (0x1 << 0));
}

void mtcmos_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pd_plat); i++)
		mtcmos_power_on(&pd_plat[i]);
}

void mtcmos_ufs_power_off(void)
{
	/* ufs0_phy */
	mtcmos_power_off(&pd_plat[1]);
	/* ufs0 */
	mtcmos_power_off(&pd_plat[0]);
}

void mtcmos_protect_audio_bus(void)
{
	write32(&mtk_infracfg_ao->perisys_protect.clr, BIT(6));

	/* AUDIO CG Clear */
	clrbits32(&mtk_afe->audio_audio_top[0], 0x03364F80);
	clrbits32(&mtk_afe->audio_audio_top[1], 0x00F000FF);
	clrbits32(&mtk_afe->audio_audio_top[2], 0x01323000);
	clrbits32(&mtk_afe->audio_audio_top[3], 0x03F00000);
	clrbits32(&mtk_afe->audio_audio_top[4], 0x0000301F);
}

void mtcmos_protect_display_bus(void)
{
	write32(&mtk_infracfg_ao->mmsys_protect[0].clr, BIT(1) | BIT(0));

	/* MMSYS_CONFIG CG Clear */
	write32(&mtk_mmsys_config->mmsys_config_mmsys_cg_0_clr, 0xFF7FFFFF);
	write32(&mtk_mmsys_config->mmsys_config_mmsys_cg_1_clr, 0x0000007B);
}
