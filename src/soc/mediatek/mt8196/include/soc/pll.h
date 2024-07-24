/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 14.1
 */

#ifndef SOC_MEDIATEK_MT8196_PLL_H
#define SOC_MEDIATEK_MT8196_PLL_H

#include <soc/addressmap.h>
#include <soc/pll_common.h>

static struct mtk_apinfra_io_noc_regs *const
	mtk_apinfra_io_noc = (void *)APINFRA_IO_NOC_AO_BCRM_BASE;
static struct mtk_apinfra_io_ctrl_regs *const
	mtk_apinfra_io_ctrl = (void *)APINFRA_IO_CTRL_AO_BCRM_BASE;
static struct mtk_apinfra_mem_noc_regs *const
	mtk_apinfra_mem_noc = (void *)APINFRA_MEM_NOC_AO_BCRM_BASE;
static struct mtk_apinfra_mem_ctrl_regs *const
	mtk_apinfra_mem_ctrl = (void *)APINFRA_MEM_CTRL_AO_BCRM_BASE;
static struct mtk_mcusys_regs *const
	mtk_mcusys = (void *)MCUCFG_BASE;
static struct mtk_peri_regs *const
	mtk_peri = (void *)PERI_PAR_BCRM_BASE;
static struct mtk_usb_regs *const
	mtk_usb = (void *)USB_BUS_BCRM_BASE;
static struct mtk_topckgen2_regs *const
	mtk_topckgen2 = (void *)CKSYS_GP2_BASE;
static struct mtk_apmixed2_regs *const
	mtk_apmixed2 = (void *)APMIXEDSYS_GP2_BASE;
static struct mtk_apifrbus_regs *const
	mtk_apifrbus = (void *)APIFRBUS_AO_MEM_REG_BASE;
static struct mtk_pericfg_regs *const
	mtk_pericfg = (void *)PERICFG_AO_BASE;
static struct mtk_ssr_regs *const
	mtk_ssr = (void *)SSR_TOP_BASE;
static struct mtk_spm_mtcmos_regs *const
	mtk_spm_mtcmos = (void *)SCP_BASE;
static struct mtk_vlp_regs *const
	mtk_vlpsys = (void *)VLP_CKSYS_BASE;
static struct mtk_dispsys_regs *const
	mtk_mmsys = (void *)MMSYS_CONFIG_BASE;
static struct mtk_dispsys_regs *const
	mtk_mmsys1 = (void *)MMSYS1_CONFIG_BASE;
static struct mtk_dispsys_regs *const
	mtk_ovlsys = (void *)OVLSYS_CONFIG_BASE;
static struct mtk_dispsys_regs *const
	mtk_ovlsys1 = (void *)OVLSYS1_CONFIG_BASE;
static struct mtk_dispsys_regs *const
	mtk_disp = (void *)DISP_VDISP_AO_CONFIG_BASE;
static struct mtk_mfgpll_regs *const
	mtk_mfgpll = (void *)MFGPLL_PLL_CTRL_BASE;
static struct mtk_mfgpll_regs *const
	mtk_mfgpll_sc0 = (void *)MFGPLL_SC0_PLL_CTRL_BASE;
static struct mtk_mfgpll_regs *const
	mtk_mfgpll_sc1 = (void *)MFGPLL_SC1_PLL_CTRL_BASE;
static struct mtk_mcupll_regs *const
	mtk_ccipll = (void *)CCIPLL_PLL_CTRL_BASE;
static struct mtk_mcupll_regs *const
	mtk_armpll_ll = (void *)ARMPLL_LL_PLL_CTRL_BASE;
static struct mtk_mcupll_regs *const
	mtk_armpll_bl = (void *)ARMPLL_BL_PLL_CTRL_BASE;
static struct mtk_mcupll_regs *const
	mtk_armpll_b = (void *)ARMPLL_B_PLL_CTRL_BASE;
static struct mtk_mcupll_regs *const
	mtk_ptppll = (void *)PTPPLL_PLL_CTRL_BASE;

struct mtk_apinfra_io_noc_regs {
	u32 reserved1[1];
	u32 clk_io_noc_ctrl;
};
check_member(mtk_apinfra_io_noc_regs, clk_io_noc_ctrl, 0x0004);

struct mtk_apinfra_io_ctrl_regs {
	u32 reserved1[2];
	u32 clk_io_intx_bus_ctrl;
};
check_member(mtk_apinfra_io_ctrl_regs, clk_io_intx_bus_ctrl, 0x0008);

struct mtk_apinfra_mem_noc_regs {
	u32 vdnr_mem_intf_par_bus_ctrl;
};
check_member(mtk_apinfra_mem_noc_regs, vdnr_mem_intf_par_bus_ctrl, 0x0000);

struct mtk_apinfra_mem_ctrl_regs {
	u32 reserved1[3];
	u32 vdnr_dcm_mem_intx_bus_ctrl;
	u32 reserved2[1];
	u32 clk_mem_intx_bus_ctrl[6];
};
check_member(mtk_apinfra_mem_ctrl_regs, vdnr_dcm_mem_intx_bus_ctrl, 0x000C);
check_member(mtk_apinfra_mem_ctrl_regs, clk_mem_intx_bus_ctrl[0], 0x0014);
check_member(mtk_apinfra_mem_ctrl_regs, clk_mem_intx_bus_ctrl[5], 0x0028);

struct mtk_mcusys_regs {
	u32 reserved1[64];
	u32 bus_plldiv_cfg;
	u32 reserved2[2];
	u32 cpu_plldiv0_cfg;
	u32 reserved3[2];
	u32 cpu_plldiv1_cfg;
	u32 reserved4[2];
	u32 cpu_plldiv2_cfg;
	u32 reserved5[112];
	u32 cpu_src_clk_config;
	u32 infra_clk_src_config;
	u32 dsu_pcsm_clk_src_config;
	u32 reserved6[323];
	u32 ses_clk_free_ck_en;
};
check_member(mtk_mcusys_regs, bus_plldiv_cfg, 0x0100);
check_member(mtk_mcusys_regs, cpu_src_clk_config, 0x02E8);
check_member(mtk_mcusys_regs, ses_clk_free_ck_en, 0x0800);

struct mtk_peri_regs {
	u32 reserved1[11];
	u32 vdnr_dcm_top_peri_par_bus_u_peri_par_bus_ctrl;
};
check_member(mtk_peri_regs, vdnr_dcm_top_peri_par_bus_u_peri_par_bus_ctrl, 0x002C);

struct mtk_usb_regs {
	u32 reserved1[24];
	u32 vdnr_dcm_top_usb_bus_u_usb_bus_ctrl;
};
check_member(mtk_usb_regs, vdnr_dcm_top_usb_bus_u_usb_bus_ctrl, 0x0060);

struct mtk_clk_cfg {
	u32 cfg;
	u32 set;
	u32 clr;
	u32 reserved;
};

struct mtk_topckgen_regs {
	u32 reserved1[1];
	u32 clk_cfg_update[3];
	struct mtk_clk_cfg clk_cfg[23];
	u32 reserved2[16];
	u32 clk26cali[2];
	u32 reserved3[3];
	u32 clk_misc_cfg;
	u32 reserved4[10];
	u32 clk_dbg_cfg;
	u32 reserved5[13];
	u32 clk_prot_idle_reg;
	u32 clk_fenc_bypass_reg[3];
	u32 clk_prot_idle_all_reg[3];
	u32 clk_prot_idle_all_inv_reg[3];
	u32 clk_prot_vote_ck_en_reg[3];
	u32 clk_prot_spm_ck_en_reg;
};
check_member(mtk_topckgen_regs, clk_cfg_update[0], 0x0004);
check_member(mtk_topckgen_regs, clk_cfg[0], 0x0010);
check_member(mtk_topckgen_regs, clk26cali[0], 0x01C0);
check_member(mtk_topckgen_regs, clk_prot_idle_all_reg[0], 0x0248);
check_member(mtk_topckgen_regs, clk_prot_spm_ck_en_reg, 0x026C);

struct mtk_apmixed_regs {
	u32 reserved1[13];
	u32 fenc_bypass_con;
	u32 fenc_protected_con;
	u32 reserved2[4];
	u32 child_enable_mask_con;
	u32 reserved3[1];
	u32 ref_clk_req_protected_con;
	u32 reserved4[11];
	u32 pllen_all_set;
	u32 pllen_all_clr;
	u32 reserved5[1];
	u32 pll_div_rstb_all_set;
	u32 reserved6[111];
	u32 mainpll_con[2];
	u32 reserved7[3];
	u32 univpll_con[2];
	u32 reserved8[3];
	u32 msdcpll_con[2];
	u32 reserved9[3];
	u32 adsppll_con[2];
	u32 reserved10[3];
	u32 emipll_con[2];
	u32 reserved11[3];
	u32 emipll2_con[2];
	u32 reserved12[3];
	u32 net1pll_con[2];
	u32 reserved13[3];
	u32 sgmiipll_con[2];
};
check_member(mtk_apmixed_regs, fenc_bypass_con, 0x0034);
check_member(mtk_apmixed_regs, mainpll_con[0], 0x0250);
check_member(mtk_apmixed_regs, sgmiipll_con[0], 0x02DC);

struct mtk_topckgen2_regs {
	u32 reserved0[1];
	u32 cksys2_clk_cfg_update;
	u32 reserved1[2];
	struct mtk_clk_cfg cksys2_clk_cfg[7];
	u32 reserved2[28];
	u32 cksys2_clk26cali[2];
	u32 reserved3[3];
	u32 cksys2_clk_misc_cfg;
	u32 reserved4[7];
	u32 cksys2_clk_dbg_cfg;
	u32 reserved5[5];
	u32 cksys2_clk_prot_idle_reg;
	u32 cksys2_clk_fenc_bypass_reg;
	u32 reserved6[2];
	u32 cksys2_clk_prot_idle_all_reg;
	u32 reserved7[2];
	u32 cksys2_clk_prot_idle_all_inv_reg;
	u32 reserved8[2];
	u32 cksys2_clk_prot_vote_ck_en_reg;
	u32 reserved9[2];
	u32 cksys2_clk_prot_spm_ck_en_reg;
};
check_member(mtk_topckgen2_regs, cksys2_clk_cfg_update, 0x0004);
check_member(mtk_topckgen2_regs, cksys2_clk26cali[0], 0x00F0);
check_member(mtk_topckgen2_regs, cksys2_clk_prot_spm_ck_en_reg, 0x0170);

struct mtk_apmixed2_regs {
	u32 reserved1[13];
	u32 gp2_fenc_bypass_con;
	u32 gp2_fenc_protected_con;
	u32 reserved2[4];
	u32 gp2_child_enable_mask_con;
	u32 reserved3[1];
	u32 gp2_ref_clk_req_protected_con;
	u32 reserved4[11];
	u32 gp2_pllen_all_set;
	u32 gp2_pllen_all_clr;
	u32 reserved5[1];
	u32 gp2_pll_div_rstb_all_set;
	u32 reserved6[111];
	u32 mainpll2_con[2];
	u32 reserved7[3];
	u32 univpll2_con[2];
	u32 reserved8[3];
	u32 mmpll2_con[2];
	u32 reserved9[3];
	u32 imgpll_con[2];
	u32 reserved10[3];
	u32 tvdpll1_con[2];
	u32 reserved11[3];
	u32 tvdpll2_con[2];
	u32 reserved12[3];
	u32 tvdpll3_con[2];
};
check_member(mtk_apmixed2_regs, gp2_fenc_bypass_con, 0x0034);
check_member(mtk_apmixed2_regs, mainpll2_con[0], 0x0250);
check_member(mtk_apmixed2_regs, tvdpll3_con[1], 0x02CC);

struct mtk_apifrbus_regs {
	u32 reserved1[832];
	struct mtk_clk_cfg apifrbus_ao_mem_reg_module_cg;
};
check_member(mtk_apifrbus_regs, apifrbus_ao_mem_reg_module_cg.cfg, 0x0D00);
check_member(mtk_apifrbus_regs, apifrbus_ao_mem_reg_module_cg.clr, 0x0D08);

struct mtk_pericfg_regs {
	u32 reserved1[4];
	u32 pericfg_ao_peri_cg[3];
	u32 reserved2[2];
	u32 pericfg_ao_peri_cg_0_set;
	u32 pericfg_ao_peri_cg_0_clr;
	u32 pericfg_ao_peri_cg_1_set;
	u32 pericfg_ao_peri_cg_1_clr;
	u32 pericfg_ao_peri_cg_2_set;
	u32 pericfg_ao_peri_cg_2_clr;
};
check_member(mtk_pericfg_regs, pericfg_ao_peri_cg[0], 0x0010);
check_member(mtk_pericfg_regs, pericfg_ao_peri_cg_2_clr, 0x0038);

struct mtk_ssr_regs {
	u32 ssr_top_ssr_top_clk_cfg[3];
};
check_member(mtk_ssr_regs, ssr_top_ssr_top_clk_cfg[0], 0x0000);

struct mtk_spm_mtcmos_regs {
	u32 reserved1[260];
	u32 spm2gpupm_con;
	u32 reserved2[635];
	u32 md1_pwr_con;
	u32 conn_pwr_con;
	u32 reserved3[4];
	u32 ssusb_dp_phy_p0_pwr_con;
	u32 ssusb_p0_pwr_con;
	u32 ssusb_p1_pwr_con;
	u32 ssusb_p23_pwr_con;
	u32 ssusb_phy_p2_pwr_con;
	u32 ufs0_pwr_con;
	u32 ufs0_phy_pwr_con;
	u32 pextp_mac0_pwr_con;
	u32 pextp_mac1_pwr_con;
	u32 pextp_mac2_pwr_con;
	u32 pextp_phy0_pwr_con;
	u32 pextp_phy1_pwr_con;
	u32 pextp_phy2_pwr_con;
	u32 audio_pwr_con;
	u32 reserved4[1];
	u32 adsp_top_pwr_con;
	u32 adsp_infra_pwr_con;
	u32 adsp_ao_pwr_con;
	u32 mm_proc_pwr_con;
	u32 reserved5[8];
	u32 ssrsys_pwr_con;
	u32 spu_ise_pwr_con;
	u32 spu_hwrot_pwr_con;
	u32 reserved6[1];
	u32 hsgmii0_pwr_con;
	u32 hsgmii1_pwr_con;
	u32 reserved7[24];
	u32 md_buck_iso_con;
	u32 reserved8[5];
	u32 pwr_status;
	u32 pwr_status_2nd;
};
check_member(mtk_spm_mtcmos_regs, spm2gpupm_con, 0x0410);
check_member(mtk_spm_mtcmos_regs, md1_pwr_con, 0x0E00);
check_member(mtk_spm_mtcmos_regs, ufs0_pwr_con, 0x0E2C);
check_member(mtk_spm_mtcmos_regs, hsgmii0_pwr_con, 0x0E94);
check_member(mtk_spm_mtcmos_regs, pwr_status_2nd, 0x0F18);

struct mtk_vlp_regs {
	u32 reserved0[1];
	u32 vlp_clk_cfg_update[2];
	u32 reserved1[1];
	struct mtk_clk_cfg vlp_clk_cfg[12];
	u32 reserved2[76];
	u32 vlp_ulposc1_con[3];
	u32 reserved3[9];
	u32 vlp_fqmtr_con[2];
	u32 reserved4[11];
	u32 vlp_ap_pll_con3;
	u32 reserved5[3];
	u32 vlp_apll1_con[3];
	u32 reserved6[3];
	u32 vlp_apll2_con[3];
	u32 reserved7[6];
	u32 vlp_pllen_apll_set;
	u32 vlp_pllen_apll_clr;
	u32 reserved8[19];
	u32 vlp_clk_prot_idle_reg;
	u32 vlp_clk_prot_idle_all_reg[2];
	u32 vlp_clk_prot_idle_all_inv_reg[2];
	u32 reserved9[3];
	u32 vlp_clk_fenc_bypass_reg[2];
	u32 reserved10[18];
	u32 vlp_child_enable_mask_con;
	u32 vlp_fenc_bypass_con;
	u32 vlp_fenc_protected_con;
	u32 reserved11[5];
	u32 vlp_ref_clk_req_protected_con;
};
check_member(mtk_vlp_regs, vlp_clk_cfg_update, 0x0004);
check_member(mtk_vlp_regs, vlp_ulposc1_con, 0x0200);
check_member(mtk_vlp_regs, vlp_fqmtr_con, 0x0230);
check_member(mtk_vlp_regs, vlp_ap_pll_con3, 0x0264);
check_member(mtk_vlp_regs, vlp_clk_prot_idle_reg, 0x0304);
check_member(mtk_vlp_regs, vlp_child_enable_mask_con, 0x0374);
check_member(mtk_vlp_regs, vlp_ref_clk_req_protected_con, 0x0394);

struct mtk_dispsys_regs {
	u32 reserved1[64];
	struct mtk_clk_cfg clk_cfg[2];
};
check_member(mtk_dispsys_regs, clk_cfg[0].cfg, 0x0100);
check_member(mtk_dispsys_regs, clk_cfg[1].clr, 0x0118);

struct mtk_mfgpll_regs {
	u32 reserved1[2];
	u32 pll_con[6];
	u32 reserved2[8];
	u32 fqmtr_con[2];
};
check_member(mtk_mfgpll_regs, pll_con[0], 0x0008);
check_member(mtk_mfgpll_regs, fqmtr_con[1], 0x0044);

struct mtk_mcupll_regs {
	u32 reserved1[2];
	u32 pll_con[2];
	u32 reserved2[12];
	u32 fqmtr_con[2];
};
check_member(mtk_mcupll_regs, pll_con[0], 0x0008);
check_member(mtk_mcupll_regs, fqmtr_con[1], 0x0044);

/* PLL rate */
enum {
	MAINPLL_HZ	= 2184UL * MHz,
	UNIVPLL_HZ	= 2496UL * MHz,
	MSDCPLL_HZ	= 384 * MHz,
	ADSPPLL_HZ	= 800 * MHz,
	MAINPLL2_HZ	= 2184UL * MHz,
	UNIVPLL2_HZ	= 2496UL * MHz,
	MMPLL2_HZ	= 2750UL * MHz,
	IMGPLL_HZ	= 2640UL * MHz,
	TVDPLL1_HZ	= 594 * MHz,
	TVDPLL2_HZ	= 594 * MHz,
	TVDPLL3_HZ	= 594 * MHz,
	VLP_APLL1_HZ	= 180633600,
	VLP_APLL2_HZ	= 196608 * KHz,
	ARMPLL_LL_HZ	= 700 * MHz,
	ARMPLL_BL_HZ	= 800 * MHz,
	ARMPLL_B_HZ	= 1250UL * MHz,
	CCIPLL_HZ	= 471 * MHz,
	PTPPLL_HZ	= 654 * MHz,
	MFGPLL_HZ	= 338 * MHz,
	MFGPLL_SC0_HZ	= 338 * MHz,
	MFGPLL_SC1_HZ	= 338 * MHz,
};

enum {
	CLK26M_HZ		= 26 * MHz,
	UNIVPLL_D6_D2_HZ	= UNIVPLL_HZ / 6 / 2,
};

enum {
	SPI_HZ	= UNIVPLL_D6_D2_HZ,
	UART_HZ	= CLK26M_HZ,
};

enum {
	PCW_INTEGER_BITS = 8,
};

enum {
	PLL_EN_DELAY = 20,
};

enum {
	MT8196_PLL_EN = BIT(0),
};

u32 mt_get_vlpck_freq(u32 id);
void mt_pll_post_init(void);

#endif	/* SOC_MEDIATEK_MT8196_PLL_H */
