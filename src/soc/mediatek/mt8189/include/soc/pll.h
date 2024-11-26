/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 8.1
 */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_PLL_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_PLL_H__

#include <soc/addressmap.h>
#include <soc/pll_common.h>

static struct mtk_infra_ao_bcrm_regs *const
	mtk_infra_ao_bcrm = (void *)INFRA_AO_BCRM_BASE;
static struct mtk_cpu_plldiv_cfg_regs *const
	mtk_cpu_plldiv_cfg = (void *)CPU_PLLDIV_CTRL_BASE;
static struct mtk_bus_plldiv_cfg_regs *const
	mtk_bus_plldiv_cfg = (void *)BUS_PLLDIV_CTRL_BASE;
static struct mtk_peri_ao_bcrm_regs *const
	mtk_peri_ao_bcrm = (void *)PERI_AO_BCRM_BASE;
static struct mtk_vlp_regs *const
	mtk_vlpsys = (void *)VLP_CK_BASE;
static struct mtk_pericfg_ao_regs *const
	mtk_pericfg_ao = (void *)PERICFG_AO_BASE;
static struct mtk_afe_regs *const
	mtk_afe = (void *)AUDIO_BASE;
static struct mtk_ufscfg_ao_regs *const
	mtk_ufscfg_ao = (void *)UFS0_AO_CFG_BASE;
static struct mtk_ufscfg_pdn_regs *const
	mtk_ufscfg_pdn = (void *)UFS0_PDN_CFG_BASE;
static struct mtk_imp_iic_wrap_ws_regs *const
	mtk_imp_iic_wrap_ws = (void *)IMP_IIC_WRAP_WS_BASE;
static struct mtk_imp_iic_wrap_e_regs *const
	mtk_imp_iic_wrap_e = (void *)IMP_IIC_WRAP_E_BASE;
static struct mtk_imp_iic_wrap_s_regs *const
	mtk_imp_iic_wrap_s = (void *)IMP_IIC_WRAP_S_BASE;
static struct mtk_imp_iic_wrap_en_regs *const
	mtk_imp_iic_wrap_en = (void *)IMP_IIC_WRAP_EN_BASE;
static struct mtk_mfg_regs *const
	mtk_mfg = (void *)MFGCFG_BASE;
static struct mtk_mmsys_config_regs *const
	mtk_mmsys_config = (void *)MMSYS_CONFIG_BASE;
static struct mtk_imgsys1_regs *const
	mtk_imgsys1 = (void *)IMGSYS1_BASE;
static struct mtk_imgsys2_regs *const
	mtk_imgsys2 = (void *)IMGSYS2_BASE;
static struct mtk_vdec_core_regs *const
	mtk_vdec_core = (void *)VDEC_CORE_BASE;
static struct mtk_venc_gcon_regs *const
	mtk_venc_gcon = (void *)VENC_GCON_BASE;
static struct mtk_scp_iic_regs *const
	mtk_scp_iic = (void *)SCP_IIC_BASE;
static struct mtk_scp_regs *const
	mtk_scp = (void *)SCP_BASE;
static struct mtk_vadsys_regs *const
	mtk_vadsys = (void *)VAD_BASE;
static struct mtk_camsys_main_regs *const
	mtk_camsys_main = (void *)CAMSYS_MAIN_BASE;
static struct mtk_camsys_rawa_regs *const
	mtk_camsys_rawa = (void *)CAMSYS_RAWA_BASE;
static struct mtk_camsys_rawb_regs *const
	mtk_camsys_rawb = (void *)CAMSYS_RAWB_BASE;
static struct mtk_ipesys_regs *const
	mtk_ipesys = (void *)IPE_BASE;
static struct mtk_vlpcfg_ao_regs *const
	mtk_vlpcfg_ao = (void *)VLPCFG_AO_REG_BASE;
static struct mtk_dvfsrc_top_regs *const
	mtk_dvfsrc_top = (void *)DVFSRC_TOP_BASE;
static struct mtk_mminfra_config_regs *const
	mtk_mminfra_config = (void *)MMINFRA_CONFIG_BASE;
static struct mtk_gce_d_regs *const
	mtk_gce_d = (void *)GCE_BASE;
static struct mtk_gce_m_regs *const
	mtk_gce_m = (void *)MDP_GCE_BASE;
static struct mtk_mdpsys_config_regs *const
	mtk_mdpsys_config = (void *)MDPSYS_CONFIG_BASE;
static struct mtk_dbgao_regs *const
	mtk_dbgao = (void *)DBGAO_BASE;
static struct mtk_dem_regs *const
	mtk_dem = (void *)DEM_BASE;

struct mtk_infra_ao_bcrm_regs {
	u32 reserved[14];
	u32 vdnr_dcm_infra_par_bus_ctrl_0;
};
check_member(mtk_infra_ao_bcrm_regs, vdnr_dcm_infra_par_bus_ctrl_0, 0x0038);

struct mtk_cpu_plldiv_cfg_regs {
	u32 cpu_plldiv_0_cfg0;
	u32 cpu_plldiv_1_cfg0;
};

struct mtk_bus_plldiv_cfg_regs {
	u32 bus_plldiv_cfg0;
};

struct mtk_peri_ao_bcrm_regs {
	u32 reserved1[8];
	u32 vdnr_dcm_peri_par_bus_ctrl_0;
	u32 reserved2[8];
};
check_member(mtk_peri_ao_bcrm_regs, vdnr_dcm_peri_par_bus_ctrl_0, 0x0020);

struct mtk_clk_cfg {
	u32 cfg;
	u32 set;
	u32 clr;
	u32 reserved;
};

struct mtk_vlp_clk_cfg {
	u32 cfg;
	u32 set;
	u32 clr;
};

struct mtk_vlp_regs {
	u32 reserved1;
	u32 vlp_clk_cfg_update;
	struct mtk_vlp_clk_cfg vlp_clk_cfg[6];
	u32 reserved2[104];
	u32 vlp_clk_cfg_30;
	u32 vlp_clk_cfg_30_set;
	u32 vlp_clk_cfg_30_clr;
	u32 reserved3[3];
	u32 vlp_ulposc1_con0;
	u32 vlp_ulposc1_con1;
	u32 vlp_ulposc1_con2;
	u32 reserved4[7];
	u32 vlp_fqmtr_con[2];
};
check_member(mtk_vlp_regs, vlp_clk_cfg_update, 0x0004);
check_member(mtk_vlp_regs, vlp_clk_cfg[0].set, 0x000C);
check_member(mtk_vlp_regs, vlp_clk_cfg[0].clr, 0x0010);
check_member(mtk_vlp_regs, vlp_clk_cfg[5].set, 0x0048);
check_member(mtk_vlp_regs, vlp_clk_cfg[5].clr, 0x004C);
check_member(mtk_vlp_regs, vlp_clk_cfg_30, 0x01F0);
check_member(mtk_vlp_regs, vlp_ulposc1_con0, 0x0208);
check_member(mtk_vlp_regs, vlp_ulposc1_con1, 0x020C);
check_member(mtk_vlp_regs, vlp_ulposc1_con2, 0x0210);
check_member(mtk_vlp_regs, vlp_fqmtr_con[0], 0x0230);

struct mtk_topckgen_regs {
	u32 reserved1;
	u32 clk_cfg_update[3];
	struct mtk_clk_cfg clk_cfg[17];
	u32 reserved2[8];
	u32 clk_misc_cfg_0;
	u32 reserved3[7];
	u32 clk_mem_dfs_cfg;
	u32 reserved4[6];
	u32 clk_dbg_cfg;
	struct mtk_clk_cfg clk_cfg_17;
	struct mtk_clk_cfg clk_cfg_18;
	u32 reserved5[24];
	u32 clk_scp_cfg_0;
	u32 reserved6[7];
	u32 clk26cali[2];
	u32 reserved7[6];
	struct mtk_clk_cfg clk_cfg_19;
	u32 reserved8[176];
	struct mtk_clk_cfg clk_misc_cfg_3;
};
check_member(mtk_topckgen_regs, clk_cfg_update[0], 0x0004);
check_member(mtk_topckgen_regs, clk_cfg[0].set, 0x0014);
check_member(mtk_topckgen_regs, clk_cfg[0].clr, 0x0018);
check_member(mtk_topckgen_regs, clk_cfg[16].set, 0x0114);
check_member(mtk_topckgen_regs, clk_cfg[16].clr, 0x0118);
check_member(mtk_topckgen_regs, clk_misc_cfg_0, 0x0140);
check_member(mtk_topckgen_regs, clk_mem_dfs_cfg, 0x0160);
check_member(mtk_topckgen_regs, clk_dbg_cfg, 0x017C);
check_member(mtk_topckgen_regs, clk_cfg_17.set, 0x0184);
check_member(mtk_topckgen_regs, clk_cfg_17.clr, 0x0188);
check_member(mtk_topckgen_regs, clk_cfg_18.set, 0x0194);
check_member(mtk_topckgen_regs, clk_cfg_18.clr, 0x0198);
check_member(mtk_topckgen_regs, clk_scp_cfg_0, 0x0200);
check_member(mtk_topckgen_regs, clk26cali, 0x0220);
check_member(mtk_topckgen_regs, clk_cfg_19.set, 0x0244);
check_member(mtk_topckgen_regs, clk_cfg_19.clr, 0x0248);
check_member(mtk_topckgen_regs, clk_misc_cfg_3.cfg, 0x0510);
check_member(mtk_topckgen_regs, clk_misc_cfg_3.set, 0x0514);
check_member(mtk_topckgen_regs, clk_misc_cfg_3.clr, 0x0518);

struct mtk_apmixed_regs {
	u32 reserved1[16];
	u32 apll1_tuner_con0; /* 0x0040 */
	u32 apll2_tuner_con0;
	u32 reserved2[11];
	u32 pllen_all_set; /* 0x0074 */
	u32 pllen_all_clr;
	u32 reserved3[2];
	u32 pll_div_rstb_all_set; /* 0x0084 */
	u32 reserved4[95];
	u32 armpll_ll_con[2]; /* 0x0204 */
	u32 reserved5[2];
	u32 armpll_bl_con[2]; /* 0x0214 */
	u32 reserved6[2];
	u32 ccipll_con[2]; /* 0x0224 */
	u32 reserved7[54];
	u32 mainpll_con[2]; /* 0x0304 */
	u32 reserved8[2];
	u32 univpll_con[2]; /* 0x0314 */
	u32 reserved9[2];
	u32 mmpll_con[2]; /* 0x0324 */
	u32 reserved10[2];
	u32 emipll_con[2]; /* 0x0334 */
	u32 reserved11[50];
	u32 apll1_con[3]; /* 0x0404 */
	u32 reserved12[2];
	u32 apll2_con[3]; /* 0x0418 */
	u32 reserved13[2];
	u32 tvdpll1_con[2]; /* 0x042c */
	u32 reserved14[2];
	u32 tvdpll2_con[2]; /* 0x043c */
	u32 reserved15[48];
	u32 mfgpll_con[2]; /* 0x0504 */
	u32 reserved16[2];
	u32 ethpll_con[2]; /* 0x0514 */
	u32 reserved17[2];
	u32 msdcpll_con[2]; /* 0x0524 */
	u32 reserved18[2];
	u32 ufspll_con[2]; /* 0x0534 */
	u32 reserved19[50];
	u32 apupll_con[2]; /* 0x0604 */
	u32 reserved20[2];
	u32 apupll2_con[2]; /* 0x0614 */
	u32 reserved21[57];
	u32 ap_clksq_con0; /* 0x0700 */
};
check_member(mtk_apmixed_regs, apll1_tuner_con0, 0x0040);
check_member(mtk_apmixed_regs, pllen_all_set, 0x0074);
check_member(mtk_apmixed_regs, pll_div_rstb_all_set, 0x0084);
check_member(mtk_apmixed_regs, armpll_ll_con[0], 0x0204);
check_member(mtk_apmixed_regs, armpll_bl_con[0], 0x0214);
check_member(mtk_apmixed_regs, ccipll_con[0], 0x0224);
check_member(mtk_apmixed_regs, mainpll_con[0], 0x0304);
check_member(mtk_apmixed_regs, univpll_con[0], 0x0314);
check_member(mtk_apmixed_regs, mmpll_con[0], 0x0324);
check_member(mtk_apmixed_regs, emipll_con[0], 0x0334);
check_member(mtk_apmixed_regs, apll1_con[0], 0x0404);
check_member(mtk_apmixed_regs, apll2_con[0], 0x0418);
check_member(mtk_apmixed_regs, tvdpll1_con[0], 0x042C);
check_member(mtk_apmixed_regs, tvdpll2_con[0], 0x043C);
check_member(mtk_apmixed_regs, mfgpll_con[0], 0x0504);
check_member(mtk_apmixed_regs, ethpll_con[0], 0x0514);
check_member(mtk_apmixed_regs, msdcpll_con[0], 0x0524);
check_member(mtk_apmixed_regs, ufspll_con[0], 0x0534);
check_member(mtk_apmixed_regs, apupll_con[0], 0x0604);
check_member(mtk_apmixed_regs, apupll2_con[0], 0x0614);
check_member(mtk_apmixed_regs, ap_clksq_con0, 0x0700);

struct mtk_pericfg_ao_regs {
	u32 reserved1[4];
	u32 pericfg_ao_peri_cg_0;
	u32 pericfg_ao_peri_cg_1;
	u32 pericfg_ao_peri_cg_2;
	u32 reserved2[2];
	u32 pericfg_ao_peri_cg_0_set;
	u32 pericfg_ao_peri_cg_0_clr;
	u32 pericfg_ao_peri_cg_1_set;
	u32 pericfg_ao_peri_cg_1_clr;
	u32 pericfg_ao_peri_cg_2_set;
	u32 pericfg_ao_peri_cg_2_clr;
};
check_member(mtk_pericfg_ao_regs, pericfg_ao_peri_cg_0, 0x0010);
check_member(mtk_pericfg_ao_regs, pericfg_ao_peri_cg_0_set, 0x0024);

struct mtk_afe_regs {
	u32 audio_audio_top[5];
};

struct mtk_ufscfg_ao_regs {
	u32 reserved1;
	u32 ufscfg_ao_reg_ufs_ao_cg_0;
	u32 ufscfg_ao_reg_ufs_ao_cg_0_set;
	u32 ufscfg_ao_reg_ufs_ao_cg_0_clr;
};

struct mtk_ufscfg_pdn_regs {
	u32 reserved1;
	u32 ufscfg_pdn_reg_ufs_pdn_cg_0;
	u32 ufscfg_pdn_reg_ufs_pdn_cg_0_set;
	u32 ufscfg_pdn_reg_ufs_pdn_cg_0_clr;
};

struct mtk_imp_iic_wrap_ws_regs {
	u32 reserved1[896];
	u32 imp_iic_wrap_ws_ap_clock_cg;
	u32 imp_iic_wrap_ws_ap_clock_cg_clr;
	u32 imp_iic_wrap_ws_ap_clock_cg_set;
};
check_member(mtk_imp_iic_wrap_ws_regs, imp_iic_wrap_ws_ap_clock_cg, 0x0E00);

struct mtk_imp_iic_wrap_e_regs {
	u32 reserved1[896];
	u32 imp_iic_wrap_e_ap_clock_cg;
	u32 imp_iic_wrap_e_ap_clock_cg_clr;
	u32 imp_iic_wrap_e_ap_clock_cg_set;
};
check_member(mtk_imp_iic_wrap_e_regs, imp_iic_wrap_e_ap_clock_cg, 0x0E00);

struct mtk_imp_iic_wrap_s_regs {
	u32 reserved1[896];
	u32 imp_iic_wrap_s_ap_clock_cg;
	u32 imp_iic_wrap_s_ap_clock_cg_clr;
	u32 imp_iic_wrap_s_ap_clock_cg_set;
};
check_member(mtk_imp_iic_wrap_s_regs, imp_iic_wrap_s_ap_clock_cg, 0x0E00);

struct mtk_imp_iic_wrap_en_regs {
	u32 reserved1[896];
	u32 imp_iic_wrap_en_ap_clock_cg;
	u32 imp_iic_wrap_en_ap_clock_cg_clr;
	u32 imp_iic_wrap_en_ap_clock_cg_set;
};
check_member(mtk_imp_iic_wrap_en_regs, imp_iic_wrap_en_ap_clock_cg, 0x0E00);

struct mtk_mfg_regs {
	u32 mfg_mfg_cg_con;
	u32 mfg_mfg_cg_con_set;
	u32 mfg_mfg_cg_con_clr;
};

struct mtk_mmsys_config_regs {
	u32 reserved1[64];
	u32 mmsys_config_mmsys_cg_0;
	u32 mmsys_config_mmsys_cg_0_set;
	u32 mmsys_config_mmsys_cg_0_clr;
	u32 reserved2[1];
	u32 mmsys_config_mmsys_cg_1;
	u32 mmsys_config_mmsys_cg_1_set;
	u32 mmsys_config_mmsys_cg_1_clr;
};
check_member(mtk_mmsys_config_regs, mmsys_config_mmsys_cg_0, 0x0100);
check_member(mtk_mmsys_config_regs, mmsys_config_mmsys_cg_1, 0x0110);

struct mtk_imgsys1_regs {
	u32 imgsys1_img_cg;
	u32 imgsys1_img_cg_set;
	u32 imgsys1_img_cg_clr;
};

struct mtk_imgsys2_regs {
	u32 imgsys2_img_cg;
	u32 imgsys2_img_cg_set;
	u32 imgsys2_img_cg_clr;
};

struct mtk_vdec_core_regs {
	u32 vdec_core_vdec_cken;
	u32 vdec_core_vdec_cken_clr;
	u32 vdec_core_larb_cken_con;
	u32 vdec_core_larb_cken_con_clr;
};

struct mtk_venc_gcon_regs {
	u32 venc_gcon_vencsys_cg;
	u32 venc_gcon_vencsys_cg_set;
	u32 venc_gcon_vencsys_cg_clr;
};

struct mtk_scp_iic_regs {
	u32 reserved[900];
	u32 scp_iic_ccu_clock_cg;
	u32 scp_iic_ccu_clock_cg_clr;
	u32 scp_iic_ccu_clock_cg_set;
};
check_member(mtk_scp_iic_regs, scp_iic_ccu_clock_cg, 0x0E10);

struct mtk_scp_regs {
	u32 reserved[85];
	u32 scp_ap_spi_cg;
	u32 scp_ap_spi_cg_clr;
};
check_member(mtk_scp_regs, scp_ap_spi_cg, 0x0154);

struct mtk_vadsys_regs {
	u32 vad_vadsys_ck_en;
	u32 reserved1[95];
	u32 vad_vow_audiodsp_sw_cg;
};
check_member(mtk_vadsys_regs, vad_vow_audiodsp_sw_cg, 0x0180);

struct mtk_camsys_main_regs {
	u32 camsys_main_camsys_cg;
	u32 camsys_main_camsys_cg_set;
	u32 camsys_main_camsys_cg_clr;
};

struct mtk_camsys_rawa_regs {
	u32 camsys_rawa_camsys_cg;
	u32 camsys_rawa_camsys_cg_set;
	u32 camsys_rawa_camsys_cg_clr;
};

struct mtk_camsys_rawb_regs {
	u32 camsys_rawb_camsys_cg;
	u32 camsys_rawb_camsys_cg_set;
	u32 camsys_rawb_camsys_cg_clr;
};

struct mtk_ipesys_regs {
	u32 ipe_img_cg;
	u32 ipe_img_cg_set;
	u32 ipe_img_cg_clr;
};

struct mtk_vlpcfg_ao_regs {
	u32 reserved[512];
	u32 vlpcfg_ao_reg_debugtop_vlpao_ctrl;
};
check_member(mtk_vlpcfg_ao_regs, vlpcfg_ao_reg_debugtop_vlpao_ctrl, 0x0800);

struct mtk_dvfsrc_top_regs {
	u32 dvfsrc_top_dvfsrc_basic_control;
};

struct mtk_mminfra_config_regs {
	u32 reserved1[64];
	u32 mminfra_config_mminfra_cg_0;
	u32 mminfra_config_mminfra_cg_0_set;
	u32 mminfra_config_mminfra_cg_0_clr;
	u32 reserved2[1];
	u32 mminfra_config_mminfra_cg_1;
	u32 mminfra_config_mminfra_cg_1_set;
	u32 mminfra_config_mminfra_cg_1_clr;
};
check_member(mtk_mminfra_config_regs, mminfra_config_mminfra_cg_0, 0x0100);
check_member(mtk_mminfra_config_regs, mminfra_config_mminfra_cg_1, 0x0110);

struct mtk_gce_d_regs {
	u32 reserved[60];
	u32 gce_gce_ctl_int0;
};
check_member(mtk_gce_d_regs, gce_gce_ctl_int0, 0x00F0);

struct mtk_gce_m_regs {
	u32 reserved[60];
	u32 mdp_gce_gce_ctl_int0;
};
check_member(mtk_gce_m_regs, mdp_gce_gce_ctl_int0, 0x00F0);

struct mtk_mdpsys_config_regs {
	u32 reserved1[64];
	u32 mdpsys_config_mdpsys_cg_0;
	u32 mdpsys_config_mdpsys_cg_0_set;
	u32 mdpsys_config_mdpsys_cg_0_clr;
	u32 reserved2[1];
	u32 mdpsys_config_mdpsys_cg_1;
	u32 mdpsys_config_mdpsys_cg_1_set;
	u32 mdpsys_config_mdpsys_cg_1_clr;
};
check_member(mtk_mdpsys_config_regs, mdpsys_config_mdpsys_cg_0, 0x0100);
check_member(mtk_mdpsys_config_regs, mdpsys_config_mdpsys_cg_1, 0x0110);

struct mtk_dbgao_regs {
	u32 reserved[28];
	u32 dbgao_atb;
};
check_member(mtk_dbgao_regs, dbgao_atb, 0x0070);

struct mtk_dem_regs {
	u32 reserved1[11];
	u32 dem_dbgbusclk_en;
	u32 dem_dbgsysclk_en;
	u32 reserved2[15];
	u32 dem_atb;
};
check_member(mtk_dem_regs, dem_dbgbusclk_en, 0x002C);
check_member(mtk_dem_regs, dem_atb, 0x0070);

/* Fmeter Type */
enum fmeter_id {
	APLL1_CTRL,
	APLL2_CTRL,
	ARMPLL_BL_CTRL,
	ARMPLL_LL_CTRL,
	CCIPLL_CTRL,
	MAINPLL_CTRL,
	MMPLL_CTRL,
	MSDCPLL_CTRL,
	UFSPLL_CTRL,
	UNIVPLL_CTRL,
	EMIPLL_CTRL,
	TVDPLL1_CTRL,
	TVDPLL2_CTRL,
	MFGPLL_CTRL,
	ETHPLL_CTRL,
	APUPLL_CTRL,
	APUPLL2_CTRL,
	VLP_CKSYS_TOP_CTRL,
};

/* PLL set rate list */
enum pll_rate_type {
	ARMPLL_LL_RATE = 0,
	ARMPLL_BL_RATE,
	CCIPLL_RATE,
	PLL_RATE_NUM,
};

#define ARMPLL_BL_ID	6
#define ARMPLL_LL_ID	8
#define CCIPLL_ID	10

enum {
	ARMPLL_LL_HZ	= 1600UL * MHz,
	ARMPLL_BL_HZ	= 1700UL * MHz,
	CCIPLL_HZ	= 1140UL * MHz,
	MAINPLL_HZ	= 2184UL * MHz,
	UNIVPLL_HZ	= 2496UL * MHz,
	MMPLL_HZ	= 2750UL * MHz,
	MFGPLL_HZ	= 390 * MHz,
	APLL1_HZ	= 180633600UL,
	APLL2_HZ	= 196608UL * KHz,
	EMIPLL_HZ	= 387 * MHz,
	APUPLL2_HZ	= 230 * MHz,
	APUPLL_HZ	= 330 * MHz,
	TVDPLL1_HZ	= 594 * MHz,
	TVDPLL2_HZ	= 594 * MHz,
	ETHPLL_HZ	= 500 * MHz,
	MSDCPLL_HZ	= 416 * MHz,
	UFSPLL_HZ	= 594 * MHz,
};

enum {
	CLK26M_HZ		= 26 * MHz,
	UNIVPLL_D6_D2_HZ	= UNIVPLL_HZ / 6 / 2,
};

enum {
	PCW_INTEGER_BITS = 8,
};

enum {
	SPI_HZ	= UNIVPLL_D6_D2_HZ,
	UART_HZ	= CLK26M_HZ,
};

enum {
	PLL_EN_DELAY = 20,
};

enum {
	MCU_DIV_MASK = 0x1f << 17,
	MCU_DIV_1 = 0x8 << 17,

	MCU_MUX_MASK = 0x3 << 9,
	MCU_MUX_SRC_PLL = 0x1 << 9,
	MCU_MUX_SRC_26M = 0x0 << 9,
};

/*
 * Clock manager functions
 */
void mt_pll_post_init(void);
u32 mt_get_vlpck_freq(u32 id);
void mt_set_topck_default(void);

#endif /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_PLL_H__ */
