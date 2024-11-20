/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <delay.h>
#include <soc/mcu_common.h>
#include <soc/pll.h>
#include <soc/spm.h>

struct mtk_vlpcksys_regs {
	u32 reserved0[136];
	u32 vlp_clksq_con1;
	u32 reserved1[15];
	u32 vlp_ap_pll_con3;
};
check_member(mtk_vlpcksys_regs, vlp_clksq_con1, 0x220);
check_member(mtk_vlpcksys_regs, vlp_ap_pll_con3, 0x260);

struct mtk_vlpdevapc_regs {
	u32 vlp_ao_mas_sec_0;
	u32 reserved0[319];
	u32 vlp_ao_apc_con;
};
check_member(mtk_vlpdevapc_regs, vlp_ao_apc_con, 0x500);

struct mtk_vlpcfg_regs {
	u32 reserved0[66];
	u32 debug_mon_reg[2];
};
check_member(mtk_vlpcfg_regs, debug_mon_reg[1], 0x10c);

static struct mtk_vlpcksys_regs *const mtk_vlpcksys = (void *)VLP_CKSYS_BASE;
static struct mtk_vlpdevapc_regs *const mtk_vlpdevapc = (void *)VLP_DEVAPC_BASE;
static struct mtk_vlpcfg_regs *const mtk_vlpcfg = (void *)VLP_CFG_BASE;

unsigned int mt_spm_version;

#define SPM_INIT_PCM_FLAG	(SPM_FLAG_DISABLE_VCORE_DVS\
				| SPM_FLAG_DISABLE_DDR_DFS\
				| SPM_FLAG_DISABLE_EMI_DFS\
				| SPM_FLAG_DISABLE_BUS_DFS\
				| SPM_FLAG_RUN_COMMON_SCENARIO\
				| SPM_FLAG_ENABLE_MT8196_ES_WA\
				| SPM_FLAG_ENABLE_MT8196_EMI_ES_WA)

static struct pwr_ctrl spm_init_ctrl = {
	/* SPM_SRC_REQ */
	.reg_spm_adsp_mailbox_req = 0,
	.reg_spm_apsrc_req = 1,
	.reg_spm_ddren_req = 0,
	.reg_spm_dvfs_req = 0,
	.reg_spm_emi_req = 0,
	.reg_spm_f26m_req = 0,
	.reg_spm_infra_req = 0,
	.reg_spm_pmic_req = 0,
	.reg_spm_scp_mailbox_req = 0,
	.reg_spm_sspm_mailbox_req = 0,
	.reg_spm_sw_mailbox_req = 0,
	.reg_spm_vcore_req = 1,
	.reg_spm_vrf18_req = 0,
	.adsp_mailbox_state = 0,
	.apsrc_state = 0,
	.ddren_state = 0,
	.dvfs_state = 0,
	.emi_state = 0,
	.f26m_state = 0,
	.infra_state = 0,
	.pmic_state = 0,
	.scp_mailbox_state = 0,
	.sspm_mailbox_state = 0,
	.sw_mailbox_state = 0,
	.vcore_state = 0,
	.vrf18_state = 0,

	/* SPM_SRC_MASK_0 */
	.reg_apu_apsrc_req_mask_b = 1,
	.reg_apu_ddren_req_mask_b = 1,
	.reg_apu_emi_req_mask_b = 1,
	.reg_apu_infra_req_mask_b = 1,
	.reg_apu_pmic_req_mask_b = 1,
	.reg_apu_srcclkena_mask_b = 1,
	.reg_apu_vrf18_req_mask_b = 1,
	.reg_audio_dsp_apsrc_req_mask_b = 1,
	.reg_audio_dsp_ddren_req_mask_b = 1,
	.reg_audio_dsp_emi_req_mask_b = 1,
	.reg_audio_dsp_infra_req_mask_b = 1,
	.reg_audio_dsp_pmic_req_mask_b = 1,
	.reg_audio_dsp_srcclkena_mask_b = 1,
	.reg_audio_dsp_vcore_req_mask_b = 1,
	.reg_audio_dsp_vrf18_req_mask_b = 1,
	.reg_cam_apsrc_req_mask_b = 1,
	.reg_cam_ddren_req_mask_b = 1,
	.reg_cam_emi_req_mask_b = 1,
	.reg_cam_infra_req_mask_b = 1,
	.reg_cam_pmic_req_mask_b = 1,
	.reg_cam_srcclkena_mask_b = 1,
	.reg_cam_vrf18_req_mask_b = 1,

	/* SPM_SRC_MASK_1 */
	.reg_ccif_apsrc_req_mask_b = 0xfff,
	.reg_ccif_emi_req_mask_b = 0xfff,
	.reg_vlpcfg_rsv0_apsrc_req_mask_b = 1,
	.reg_vlpcfg_rsv0_ddren_req_mask_b = 1,
	.reg_vlpcfg_rsv0_emi_req_mask_b = 1,
	.reg_vlpcfg_rsv0_infra_req_mask_b = 1,
	.reg_vlpcfg_rsv0_pmic_req_mask_b = 1,
	.reg_vlpcfg_rsv0_srcclkena_mask_b = 1,
	.reg_vlpcfg_rsv0_vcore_req_mask_b = 1,
	.reg_vlpcfg_rsv0_vrf18_req_mask_b = 1,

	/* SPM_SRC_MASK_2 */
	.reg_ccif_infra_req_mask_b = 0xfff,
	.reg_ccif_pmic_req_mask_b = 0xfff,
	.reg_vlpcfg_rsv1_apsrc_req_mask_b = 1,
	.reg_vlpcfg_rsv1_ddren_req_mask_b = 1,
	.reg_vlpcfg_rsv1_emi_req_mask_b = 1,
	.reg_vlpcfg_rsv1_infra_req_mask_b = 1,
	.reg_vlpcfg_rsv1_pmic_req_mask_b = 1,
	.reg_vlpcfg_rsv1_srcclkena_mask_b = 1,
	.reg_vlpcfg_rsv1_vcore_req_mask_b = 1,
	.reg_vlpcfg_rsv1_vrf18_req_mask_b = 1,

	/* SPM_SRC_MASK_3 */
	.reg_ccif_srcclkena_mask_b = 0xfff,
	.reg_ccif_vrf18_req_mask_b = 0xfff,
	.reg_ccu_apsrc_req_mask_b = 1,
	.reg_ccu_ddren_req_mask_b = 1,
	.reg_ccu_emi_req_mask_b = 1,
	.reg_ccu_infra_req_mask_b = 1,
	.reg_ccu_pmic_req_mask_b = 1,
	.reg_ccu_srcclkena_mask_b = 1,
	.reg_ccu_vrf18_req_mask_b = 1,
	.reg_cg_check_apsrc_req_mask_b = 1,

	/* SPM_SRC_MASK_4 */
	.reg_cg_check_ddren_req_mask_b = 1,
	.reg_cg_check_emi_req_mask_b = 1,
	.reg_cg_check_infra_req_mask_b = 1,
	.reg_cg_check_pmic_req_mask_b = 1,
	.reg_cg_check_srcclkena_mask_b = 1,
	.reg_cg_check_vcore_req_mask_b = 1,
	.reg_cg_check_vrf18_req_mask_b = 1,
	.reg_conn_apsrc_req_mask_b = 1,
	.reg_conn_ddren_req_mask_b = 1,
	.reg_conn_emi_req_mask_b = 1,
	.reg_conn_infra_req_mask_b = 1,
	.reg_conn_pmic_req_mask_b = 1,
	.reg_conn_srcclkena_mask_b = 1,
	.reg_conn_srcclkenb_mask_b = 1,
	.reg_conn_vcore_req_mask_b = 1,
	.reg_conn_vrf18_req_mask_b = 1,
	.reg_cpueb_apsrc_req_mask_b = 1,
	.reg_cpueb_ddren_req_mask_b = 1,
	.reg_cpueb_emi_req_mask_b = 1,
	.reg_cpueb_infra_req_mask_b = 1,
	.reg_cpueb_pmic_req_mask_b = 1,
	.reg_cpueb_srcclkena_mask_b = 1,
	.reg_cpueb_vrf18_req_mask_b = 1,
	.reg_disp0_apsrc_req_mask_b = 1,
	.reg_disp0_ddren_req_mask_b = 1,
	.reg_disp0_emi_req_mask_b = 1,
	.reg_disp0_infra_req_mask_b = 1,
	.reg_disp0_pmic_req_mask_b = 1,
	.reg_disp0_srcclkena_mask_b = 1,
	.reg_disp0_vrf18_req_mask_b = 1,
	.reg_disp1_apsrc_req_mask_b = 1,
	.reg_disp1_ddren_req_mask_b = 1,

	/* SPM_SRC_MASK_5 */
	.reg_disp1_emi_req_mask_b = 1,
	.reg_disp1_infra_req_mask_b = 1,
	.reg_disp1_pmic_req_mask_b = 1,
	.reg_disp1_srcclkena_mask_b = 1,
	.reg_disp1_vrf18_req_mask_b = 1,
	.reg_dpm_apsrc_req_mask_b = 0xf,
	.reg_dpm_ddren_req_mask_b = 0xf,
	.reg_dpm_emi_req_mask_b = 0xf,
	.reg_dpm_infra_req_mask_b = 0xf,
	.reg_dpm_pmic_req_mask_b = 0xf,
	.reg_dpm_srcclkena_mask_b = 0xf,

	/* SPM_SRC_MASK_6 */
	.reg_dpm_vcore_req_mask_b = 0xf,
	.reg_dpm_vrf18_req_mask_b = 0xf,
	.reg_dpmaif_apsrc_req_mask_b = 1,
	.reg_dpmaif_ddren_req_mask_b = 1,
	.reg_dpmaif_emi_req_mask_b = 1,
	.reg_dpmaif_infra_req_mask_b = 1,
	.reg_dpmaif_pmic_req_mask_b = 1,
	.reg_dpmaif_srcclkena_mask_b = 1,
	.reg_dpmaif_vrf18_req_mask_b = 1,
	.reg_dvfsrc_level_req_mask_b = 1,
	.reg_emisys_apsrc_req_mask_b = 0,
	.reg_emisys_ddren_req_mask_b = 1,
	.reg_emisys_emi_req_mask_b = 0,
	.reg_gce_apsrc_req_mask_b = 1,
	.reg_gce_ddren_req_mask_b = 1,
	.reg_gce_emi_req_mask_b = 1,
	.reg_gce_infra_req_mask_b = 1,
	.reg_gce_pmic_req_mask_b = 1,
	.reg_gce_srcclkena_mask_b = 1,
	.reg_gce_vrf18_req_mask_b = 1,
	.reg_gpueb_apsrc_req_mask_b = 1,
	.reg_gpueb_ddren_req_mask_b = 1,
	.reg_gpueb_emi_req_mask_b = 1,
	.reg_gpueb_infra_req_mask_b = 1,
	.reg_gpueb_pmic_req_mask_b = 1,
	.reg_gpueb_srcclkena_mask_b = 1,

	/* SPM_SRC_MASK_7 */
	.reg_gpueb_vrf18_req_mask_b = 1,
	.reg_hwccf_apsrc_req_mask_b = 1,
	.reg_hwccf_ddren_req_mask_b = 1,
	.reg_hwccf_emi_req_mask_b = 1,
	.reg_hwccf_infra_req_mask_b = 1,
	.reg_hwccf_pmic_req_mask_b = 1,
	.reg_hwccf_srcclkena_mask_b = 1,
	.reg_hwccf_vcore_req_mask_b = 1,
	.reg_hwccf_vrf18_req_mask_b = 1,
	.reg_img_apsrc_req_mask_b = 1,
	.reg_img_ddren_req_mask_b = 1,
	.reg_img_emi_req_mask_b = 1,
	.reg_img_infra_req_mask_b = 1,
	.reg_img_pmic_req_mask_b = 1,
	.reg_img_srcclkena_mask_b = 1,
	.reg_img_vrf18_req_mask_b = 1,
	.reg_infrasys_apsrc_req_mask_b = 0,
	.reg_infrasys_ddren_req_mask_b = 1,
	.reg_infrasys_emi_req_mask_b = 0,
	.reg_infrasys_infra_req_mask_b = 1,
	.reg_infrasys_vrf18_req_mask_b = 1,
	.reg_ipic_infra_req_mask_b = 1,
	.reg_ipic_vrf18_req_mask_b = 1,
	.reg_mcu_apsrc_req_mask_b = 1,
	.reg_mcu_ddren_req_mask_b = 1,
	.reg_mcu_emi_req_mask_b = 1,
	.reg_mcu_infra_req_mask_b = 1,
	.reg_mcu_pmic_req_mask_b = 1,
	.reg_mcu_srcclkena_mask_b = 1,
	.reg_mcu_vrf18_req_mask_b = 1,
	.reg_md_apsrc_req_mask_b = 1,
	.reg_md_ddren_req_mask_b = 1,

	/* SPM_SRC_MASK_8 */
	.reg_md_emi_req_mask_b = 1,
	.reg_md_infra_req_mask_b = 1,
	.reg_md_pmic_req_mask_b = 1,
	.reg_md_srcclkena_mask_b = 1,
	.reg_md_srcclkena1_mask_b = 1,
	.reg_md_vcore_req_mask_b = 1,
	.reg_md_vrf18_req_mask_b = 1,
	.reg_mm_proc_apsrc_req_mask_b = 1,
	.reg_mm_proc_ddren_req_mask_b = 1,
	.reg_mm_proc_emi_req_mask_b = 1,
	.reg_mm_proc_infra_req_mask_b = 1,
	.reg_mm_proc_pmic_req_mask_b = 1,
	.reg_mm_proc_srcclkena_mask_b = 1,
	.reg_mm_proc_vrf18_req_mask_b = 1,
	.reg_mml0_apsrc_req_mask_b = 1,
	.reg_mml0_ddren_req_mask_b = 1,
	.reg_mml0_emi_req_mask_b = 1,
	.reg_mml0_infra_req_mask_b = 1,
	.reg_mml0_pmic_req_mask_b = 1,
	.reg_mml0_srcclkena_mask_b = 1,
	.reg_mml0_vrf18_req_mask_b = 1,
	.reg_mml1_apsrc_req_mask_b = 1,
	.reg_mml1_ddren_req_mask_b = 1,
	.reg_mml1_emi_req_mask_b = 1,
	.reg_mml1_infra_req_mask_b = 1,
	.reg_mml1_pmic_req_mask_b = 1,
	.reg_mml1_srcclkena_mask_b = 1,
	.reg_mml1_vrf18_req_mask_b = 1,
	.reg_ovl0_apsrc_req_mask_b = 1,
	.reg_ovl0_ddren_req_mask_b = 1,
	.reg_ovl0_emi_req_mask_b = 1,
	.reg_ovl0_infra_req_mask_b = 1,

	/* SPM_SRC_MASK_9 */
	.reg_ovl0_pmic_req_mask_b = 1,
	.reg_ovl0_srcclkena_mask_b = 1,
	.reg_ovl0_vrf18_req_mask_b = 1,
	.reg_ovl1_apsrc_req_mask_b = 1,
	.reg_ovl1_ddren_req_mask_b = 1,
	.reg_ovl1_emi_req_mask_b = 1,
	.reg_ovl1_infra_req_mask_b = 1,
	.reg_ovl1_pmic_req_mask_b = 1,
	.reg_ovl1_srcclkena_mask_b = 1,
	.reg_ovl1_vrf18_req_mask_b = 1,
	.reg_pcie0_apsrc_req_mask_b = 1,
	.reg_pcie0_ddren_req_mask_b = 1,
	.reg_pcie0_emi_req_mask_b = 1,
	.reg_pcie0_infra_req_mask_b = 1,
	.reg_pcie0_pmic_req_mask_b = 1,
	.reg_pcie0_srcclkena_mask_b = 1,
	.reg_pcie0_vcore_req_mask_b = 1,
	.reg_pcie0_vrf18_req_mask_b = 1,
	.reg_perisys_apsrc_req_mask_b = 1,
	.reg_perisys_ddren_req_mask_b = 1,
	.reg_perisys_emi_req_mask_b = 1,
	.reg_perisys_infra_req_mask_b = 1,
	.reg_perisys_pmic_req_mask_b = 1,
	.reg_perisys_srcclkena_mask_b = 1,
	.reg_perisys_vrf18_req_mask_b = 1,
	.reg_scp_apsrc_req_mask_b = 1,
	.reg_scp_ddren_req_mask_b = 1,
	.reg_scp_emi_req_mask_b = 1,
	.reg_scp_infra_req_mask_b = 1,
	.reg_scp_pmic_req_mask_b = 1,
	.reg_scp_srcclkena_mask_b = 1,
	.reg_scp_vcore_req_mask_b = 1,

	/* SPM_SRC_MASK_10 */
	.reg_scp_vrf18_req_mask_b = 1,
	.reg_spu_hwrot_apsrc_req_mask_b = 1,
	.reg_spu_hwrot_ddren_req_mask_b = 1,
	.reg_spu_hwrot_emi_req_mask_b = 1,
	.reg_spu_hwrot_infra_req_mask_b = 1,
	.reg_spu_hwrot_pmic_req_mask_b = 1,
	.reg_spu_hwrot_srcclkena_mask_b = 1,
	.reg_spu_hwrot_vrf18_req_mask_b = 1,
	.reg_spu_ise_apsrc_req_mask_b = 1,
	.reg_spu_ise_ddren_req_mask_b = 1,
	.reg_spu_ise_emi_req_mask_b = 1,
	.reg_spu_ise_infra_req_mask_b = 1,
	.reg_spu_ise_pmic_req_mask_b = 1,
	.reg_spu_ise_srcclkena_mask_b = 1,
	.reg_spu_ise_vrf18_req_mask_b = 1,
	.reg_srcclkeni_infra_req_mask_b = 3,
	.reg_srcclkeni_pmic_req_mask_b = 3,
	.reg_srcclkeni_srcclkena_mask_b = 3,
	.reg_sspm_apsrc_req_mask_b = 1,
	.reg_sspm_ddren_req_mask_b = 1,
	.reg_sspm_emi_req_mask_b = 1,
	.reg_sspm_infra_req_mask_b = 1,
	.reg_sspm_pmic_req_mask_b = 1,
	.reg_sspm_srcclkena_mask_b = 1,
	.reg_sspm_vrf18_req_mask_b = 1,
	.reg_ssrsys_apsrc_req_mask_b = 1,
	.reg_ssrsys_ddren_req_mask_b = 1,
	.reg_ssrsys_emi_req_mask_b = 1,
	.reg_ssrsys_infra_req_mask_b = 1,

	/* SPM_SRC_MASK_11 */
	.reg_ssrsys_pmic_req_mask_b = 1,
	.reg_ssrsys_srcclkena_mask_b = 1,
	.reg_ssrsys_vrf18_req_mask_b = 1,
	.reg_uart_hub_infra_req_mask_b = 1,
	.reg_uart_hub_pmic_req_mask_b = 1,
	.reg_uart_hub_srcclkena_mask_b = 1,
	.reg_uart_hub_vcore_req_mask_b = 1,
	.reg_uart_hub_vrf18_req_mask_b = 1,
	.reg_ufs_apsrc_req_mask_b = 1,
	.reg_ufs_ddren_req_mask_b = 1,
	.reg_ufs_emi_req_mask_b = 1,
	.reg_ufs_infra_req_mask_b = 1,
	.reg_ufs_pmic_req_mask_b = 1,
	.reg_ufs_srcclkena_mask_b = 1,
	.reg_ufs_vrf18_req_mask_b = 1,
	.reg_vdec_apsrc_req_mask_b = 1,
	.reg_vdec_ddren_req_mask_b = 1,
	.reg_vdec_emi_req_mask_b = 1,
	.reg_vdec_infra_req_mask_b = 1,
	.reg_vdec_pmic_req_mask_b = 1,
	.reg_vdec_srcclkena_mask_b = 1,
	.reg_vdec_vrf18_req_mask_b = 1,
	.reg_venc_apsrc_req_mask_b = 1,
	.reg_venc_ddren_req_mask_b = 1,
	.reg_venc_emi_req_mask_b = 1,
	.reg_venc_infra_req_mask_b = 1,
	.reg_venc_pmic_req_mask_b = 1,
	.reg_venc_srcclkena_mask_b = 1,
	.reg_venc_vrf18_req_mask_b = 1,

	/* SPM_EVENT_CON_MISC */
	.reg_srcclken_fast_resp = 0,
	.reg_csyspwrup_ack_mask = 0,

	/* SPM_WAKEUP_EVENT_MASK */
	.reg_wakeup_event_mask = 0xEFFFFFF7,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.reg_ext_wakeup_event_mask = 0xFFFFFFFF,

	/*sw flag setting */
	.pcm_flags = SPM_INIT_PCM_FLAG,
};

void spm_set_power_control(const struct pwr_ctrl *pwrctrl)
{
	/* SPM_SRC_REQ */
	write32(&mtk_spm->spm_src_req,
		((pwrctrl->reg_spm_adsp_mailbox_req & 0x1) << 0) |
		((pwrctrl->reg_spm_apsrc_req & 0x1) << 1) |
		((pwrctrl->reg_spm_ddren_req & 0x1) << 2) |
		((pwrctrl->reg_spm_dvfs_req & 0x1) << 3) |
		((pwrctrl->reg_spm_emi_req & 0x1) << 4) |
		((pwrctrl->reg_spm_f26m_req & 0x1) << 5) |
		((pwrctrl->reg_spm_infra_req & 0x1) << 6) |
		((pwrctrl->reg_spm_pmic_req & 0x1) << 7) |
		((pwrctrl->reg_spm_scp_mailbox_req & 0x1) << 8) |
		((pwrctrl->reg_spm_sspm_mailbox_req & 0x1) << 9) |
		((pwrctrl->reg_spm_sw_mailbox_req & 0x1) << 10) |
		((pwrctrl->reg_spm_vcore_req & 0x1) << 11) |
		((pwrctrl->reg_spm_vrf18_req & 0x1) << 12) |
		((pwrctrl->adsp_mailbox_state & 0x1) << 16) |
		((pwrctrl->apsrc_state & 0x1) << 17) |
		((pwrctrl->ddren_state & 0x1) << 18) |
		((pwrctrl->dvfs_state & 0x1) << 19) |
		((pwrctrl->emi_state & 0x1) << 20) |
		((pwrctrl->f26m_state & 0x1) << 21) |
		((pwrctrl->infra_state & 0x1) << 22) |
		((pwrctrl->pmic_state & 0x1) << 23) |
		((pwrctrl->scp_mailbox_state & 0x1) << 24) |
		((pwrctrl->sspm_mailbox_state & 0x1) << 25) |
		((pwrctrl->sw_mailbox_state & 0x1) << 26) |
		((pwrctrl->vcore_state & 0x1) << 27) |
		((pwrctrl->vrf18_state & 0x1) << 28));

	/* SPM_SRC_MASK_0 */
	write32(&mtk_spm->spm_src_mask,
		((pwrctrl->reg_apu_apsrc_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_apu_ddren_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_apu_emi_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_apu_infra_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_apu_pmic_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_apu_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_apu_vrf18_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_audio_dsp_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_audio_dsp_ddren_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_audio_dsp_emi_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_audio_dsp_infra_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_audio_dsp_pmic_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_audio_dsp_srcclkena_mask_b & 0x1) << 12) |
		((pwrctrl->reg_audio_dsp_vcore_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_audio_dsp_vrf18_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_cam_apsrc_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_cam_ddren_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_cam_emi_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_cam_infra_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_cam_pmic_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_cam_srcclkena_mask_b & 0x1) << 20) |
		((pwrctrl->reg_cam_vrf18_req_mask_b & 0x1) << 21));

	/* SPM_SRC_MASK_1 */
	write32(&mtk_spm->spm_src_mask[1],
		((pwrctrl->reg_ccif_apsrc_req_mask_b & 0xfff) << 0) |
		((pwrctrl->reg_ccif_emi_req_mask_b & 0xfff) << 12) |
		((pwrctrl->reg_vlpcfg_rsv0_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_vlpcfg_rsv0_ddren_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_vlpcfg_rsv0_emi_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_vlpcfg_rsv0_infra_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_vlpcfg_rsv0_pmic_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_vlpcfg_rsv0_srcclkena_mask_b & 0x1) << 29) |
		((pwrctrl->reg_vlpcfg_rsv0_vcore_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_vlpcfg_rsv0_vrf18_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_2 */
	write32(&mtk_spm->spm_src_mask[2],
		((pwrctrl->reg_ccif_infra_req_mask_b & 0xfff) << 0) |
		((pwrctrl->reg_ccif_pmic_req_mask_b & 0xfff) << 12) |
		((pwrctrl->reg_vlpcfg_rsv1_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_vlpcfg_rsv1_ddren_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_vlpcfg_rsv1_emi_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_vlpcfg_rsv1_infra_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_vlpcfg_rsv1_pmic_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_vlpcfg_rsv1_srcclkena_mask_b & 0x1) << 29) |
		((pwrctrl->reg_vlpcfg_rsv1_vcore_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_vlpcfg_rsv1_vrf18_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_3 */
	write32(&mtk_spm->spm_src_mask[3],
		((pwrctrl->reg_ccif_srcclkena_mask_b & 0xfff) << 0) |
		((pwrctrl->reg_ccif_vrf18_req_mask_b & 0xfff) << 12) |
		((pwrctrl->reg_ccu_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_ccu_ddren_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_ccu_emi_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_ccu_infra_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_ccu_pmic_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_ccu_srcclkena_mask_b & 0x1) << 29) |
		((pwrctrl->reg_ccu_vrf18_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_cg_check_apsrc_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_4 */
	write32(&mtk_spm->spm_src_mask[4],
		((pwrctrl->reg_cg_check_ddren_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_cg_check_emi_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_cg_check_infra_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_cg_check_pmic_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_cg_check_srcclkena_mask_b & 0x1) << 4) |
		((pwrctrl->reg_cg_check_vcore_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_cg_check_vrf18_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_conn_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_conn_ddren_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_conn_emi_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_conn_infra_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_conn_pmic_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_conn_srcclkena_mask_b & 0x1) << 12) |
		((pwrctrl->reg_conn_srcclkenb_mask_b & 0x1) << 13) |
		((pwrctrl->reg_conn_vcore_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_conn_vrf18_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_cpueb_apsrc_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_cpueb_ddren_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_cpueb_emi_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_cpueb_infra_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_cpueb_pmic_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_cpueb_srcclkena_mask_b & 0x1) << 21) |
		((pwrctrl->reg_cpueb_vrf18_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_disp0_apsrc_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_disp0_ddren_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_disp0_emi_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_disp0_infra_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_disp0_pmic_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_disp0_srcclkena_mask_b & 0x1) << 28) |
		((pwrctrl->reg_disp0_vrf18_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_disp1_apsrc_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_disp1_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_5 */
	write32(&mtk_spm->spm_src_mask[5],
		((pwrctrl->reg_disp1_emi_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_disp1_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_disp1_pmic_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_disp1_srcclkena_mask_b & 0x1) << 3) |
		((pwrctrl->reg_disp1_vrf18_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_dpm_apsrc_req_mask_b & 0xf) << 5) |
		((pwrctrl->reg_dpm_ddren_req_mask_b & 0xf) << 9) |
		((pwrctrl->reg_dpm_emi_req_mask_b & 0xf) << 13) |
		((pwrctrl->reg_dpm_infra_req_mask_b & 0xf) << 17) |
		((pwrctrl->reg_dpm_pmic_req_mask_b & 0xf) << 21) |
		((pwrctrl->reg_dpm_srcclkena_mask_b & 0xf) << 25));

	/* SPM_SRC_MASK_6 */
	write32(&mtk_spm->spm_src_mask[6],
		((pwrctrl->reg_dpm_vcore_req_mask_b & 0xf) << 0) |
		((pwrctrl->reg_dpm_vrf18_req_mask_b & 0xf) << 4) |
		((pwrctrl->reg_dpmaif_apsrc_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_dpmaif_ddren_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_dpmaif_emi_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_dpmaif_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_dpmaif_pmic_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_dpmaif_srcclkena_mask_b & 0x1) << 13) |
		((pwrctrl->reg_dpmaif_vrf18_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_dvfsrc_level_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_emisys_apsrc_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_emisys_ddren_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_emisys_emi_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_gce_apsrc_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_gce_ddren_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_gce_emi_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_gce_infra_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_gce_pmic_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_gce_srcclkena_mask_b & 0x1) << 24) |
		((pwrctrl->reg_gce_vrf18_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_gpueb_apsrc_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_gpueb_ddren_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_gpueb_emi_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_gpueb_infra_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_gpueb_pmic_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_gpueb_srcclkena_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_7 */
	write32(&mtk_spm->spm_src_mask[7],
		((pwrctrl->reg_gpueb_vrf18_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_hwccf_apsrc_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_hwccf_ddren_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_hwccf_emi_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_hwccf_infra_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_hwccf_pmic_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_hwccf_srcclkena_mask_b & 0x1) << 6) |
		((pwrctrl->reg_hwccf_vcore_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_hwccf_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_img_apsrc_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_img_ddren_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_img_emi_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_img_infra_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_img_pmic_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_img_srcclkena_mask_b & 0x1) << 14) |
		((pwrctrl->reg_img_vrf18_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_infrasys_apsrc_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_infrasys_ddren_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_infrasys_emi_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_infrasys_infra_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_infrasys_vrf18_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_ipic_infra_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_ipic_vrf18_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_mcu_apsrc_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_mcu_ddren_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_mcu_emi_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_mcu_infra_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_mcu_pmic_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_mcu_srcclkena_mask_b & 0x1) << 28) |
		((pwrctrl->reg_mcu_vrf18_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_md_apsrc_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_md_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_8 */
	write32(&mtk_spm->spm_src_mask[8],
		((pwrctrl->reg_md_emi_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_md_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_md_pmic_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_md_srcclkena_mask_b & 0x1) << 3) |
		((pwrctrl->reg_md_srcclkena1_mask_b & 0x1) << 4) |
		((pwrctrl->reg_md_vcore_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_md_vrf18_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_mm_proc_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_mm_proc_ddren_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_mm_proc_emi_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_mm_proc_infra_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_mm_proc_pmic_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_mm_proc_srcclkena_mask_b & 0x1) << 12) |
		((pwrctrl->reg_mm_proc_vrf18_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_mml0_apsrc_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_mml0_ddren_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_mml0_emi_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_mml0_infra_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_mml0_pmic_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_mml0_srcclkena_mask_b & 0x1) << 19) |
		((pwrctrl->reg_mml0_vrf18_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_mml1_apsrc_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_mml1_ddren_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_mml1_emi_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_mml1_infra_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_mml1_pmic_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_mml1_srcclkena_mask_b & 0x1) << 26) |
		((pwrctrl->reg_mml1_vrf18_req_mask_b & 0x1) << 27) |
		(((u32)pwrctrl->reg_ovl0_apsrc_req_mask_b & 0x1) << 28) |
		(((u32)pwrctrl->reg_ovl0_ddren_req_mask_b & 0x1) << 29) |
		(((u32)pwrctrl->reg_ovl0_emi_req_mask_b & 0x1) << 30) |
		(((u32)pwrctrl->reg_ovl0_infra_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_9 */
	write32(&mtk_spm->spm_src_mask[9],
		((pwrctrl->reg_ovl0_pmic_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_ovl0_srcclkena_mask_b & 0x1) << 1) |
		((pwrctrl->reg_ovl0_vrf18_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_ovl1_apsrc_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_ovl1_ddren_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_ovl1_emi_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_ovl1_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_ovl1_pmic_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_ovl1_srcclkena_mask_b & 0x1) << 8) |
		((pwrctrl->reg_ovl1_vrf18_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_pcie0_apsrc_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_pcie0_ddren_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_pcie0_emi_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_pcie0_infra_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_pcie0_pmic_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_pcie0_srcclkena_mask_b & 0x1) << 15) |
		((pwrctrl->reg_pcie0_vcore_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_pcie0_vrf18_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_perisys_apsrc_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_perisys_ddren_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_perisys_emi_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_perisys_infra_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_perisys_pmic_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_perisys_srcclkena_mask_b & 0x1) << 23) |
		((pwrctrl->reg_perisys_vrf18_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_scp_apsrc_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_scp_ddren_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_scp_emi_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_scp_infra_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_scp_pmic_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_scp_srcclkena_mask_b & 0x1) << 30) |
		((pwrctrl->reg_scp_vcore_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_10 */
	write32(&mtk_spm->spm_src_mask[10],
		((pwrctrl->reg_scp_vrf18_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_spu_hwrot_apsrc_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_spu_hwrot_ddren_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_spu_hwrot_emi_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_spu_hwrot_infra_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_spu_hwrot_pmic_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_spu_hwrot_srcclkena_mask_b & 0x1) << 6) |
		((pwrctrl->reg_spu_hwrot_vrf18_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_spu_ise_apsrc_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_spu_ise_ddren_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_spu_ise_emi_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_spu_ise_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_spu_ise_pmic_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_spu_ise_srcclkena_mask_b & 0x1) << 13) |
		((pwrctrl->reg_spu_ise_vrf18_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_srcclkeni_infra_req_mask_b & 0x3) << 15) |
		((pwrctrl->reg_srcclkeni_pmic_req_mask_b & 0x3) << 17) |
		((pwrctrl->reg_srcclkeni_srcclkena_mask_b & 0x3) << 19) |
		((pwrctrl->reg_sspm_apsrc_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_sspm_ddren_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_sspm_emi_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_sspm_infra_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_sspm_pmic_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_sspm_srcclkena_mask_b & 0x1) << 26) |
		((pwrctrl->reg_sspm_vrf18_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_ssrsys_apsrc_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_ssrsys_ddren_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_ssrsys_emi_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_ssrsys_infra_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_11 */
	write32(&mtk_spm->spm_src_mask[11],
		((pwrctrl->reg_ssrsys_pmic_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_ssrsys_srcclkena_mask_b & 0x1) << 1) |
		((pwrctrl->reg_ssrsys_vrf18_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_uart_hub_infra_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_uart_hub_pmic_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_uart_hub_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_uart_hub_vcore_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_uart_hub_vrf18_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_ufs_apsrc_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_ufs_ddren_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_ufs_emi_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_ufs_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_ufs_pmic_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_ufs_srcclkena_mask_b & 0x1) << 13) |
		((pwrctrl->reg_ufs_vrf18_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_vdec_apsrc_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_vdec_ddren_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_vdec_emi_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_vdec_infra_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_vdec_pmic_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_vdec_srcclkena_mask_b & 0x1) << 20) |
		((pwrctrl->reg_vdec_vrf18_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_venc_apsrc_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_venc_ddren_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_venc_emi_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_venc_infra_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_venc_pmic_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_venc_srcclkena_mask_b & 0x1) << 27) |
		((pwrctrl->reg_venc_vrf18_req_mask_b & 0x1) << 28));

	/* SPM_EVENT_CON_MISC */
	write32(&mtk_spm->spm_event_con_misc,
		((pwrctrl->reg_srcclken_fast_resp & 0x1) << 0) |
		((pwrctrl->reg_csyspwrup_ack_mask & 0x1) << 1));

	/* SPM_WAKEUP_EVENT_MASK */
	write32(&mtk_spm->spm_wakeup_event_mask,
		(pwrctrl->reg_wakeup_event_mask & 0xffffffff) << 0);

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	write32(&mtk_spm->spm_wakeup_event_ext_mask,
		(pwrctrl->reg_ext_wakeup_event_mask & 0xffffffff) << 0);
}

static void spm_hw_s1_state_monitor(bool en)
{
	if (en)
		clrsetbits32(&mtk_spm->spm_ack_chk[3].con, SPM_ACK_CHK_3_CON_CLR_ALL,
			     SPM_ACK_CHK_3_CON_EN);
	else
		clrsetbits32(&mtk_spm->spm_ack_chk[3].con,
			     SPM_ACK_CHK_3_CON_HW_MODE_TRIG | SPM_ACK_CHK_3_CON_CLR_ALL,
			     SPM_ACK_CHK_3_CON_EN);
}

static void spm_init_ulposc_ctrl(void)
{
	write32(&mtk_spm->spm_reosource_ulposc_mask, 0x0);

	clrbits32(&mtk_spm->spm_ulposc_en_cg_con, BIT(16));

	write32(&mtk_spm->spm_resource_ulposc_ack_con, 0x185);

	setbits32(&mtk_spm->spm_reosource_ulposc_mask,
		  BIT(2) | BIT(18) | BIT(5) | BIT(21) | BIT(6) | BIT(22) | BIT(1) | BIT(17));

	setbits32(&mtk_spm->spm_ulposc_en_cg_con, BIT(5) | BIT(0) | BIT(16));

	setbits32(&mtk_spm->spm_rsv_ulposc_req, (0x1));

	udelay(100);

	write32(&mtk_spm->spm_ulposc_off_mode_con, 0x1);
}

void spm_register_init(void)
{
	/* Enable scpsys (SPM) clock off control, 0: Topck AO */
	setbits32(&mtk_topckgen->clk_scp_cfg, 0x3FF);
	setbits32(&mtk_vlpcksys->vlp_clksq_con1, 0x1);
	clrbits32(&mtk_vlpcksys->vlp_ap_pll_con3, BIT(1));
	clrbits32(&mtk_vlpcksys->vlp_ap_pll_con3, BIT(6));

	/* Enable register control */
	write32(&mtk_spm->poweron_config_en, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Init power control register */
	write32(&mtk_spm->spm_power_on_val[1], POWER_ON_VAL1_DEF);

	clrbits32(&mtk_spm->spm_sram_con, 0xF << 16);

	/* SRAM Workaround for Sleep FSM */
	setbits32(&mtk_spm->spm_sram_sleep_ctrl, 0x2 << 8);

	write32(&mtk_spm->spm_power_on_val[1], POWER_ON_VAL1_DEF);

	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
	write32(&mtk_spm->pcm_con1,
		SPM_REGWR_CFG_KEY | REG_SPM_APB_INTERNAL_EN_LSB | REG_SSPM_APB_P2P_EN_LSB);

	write32(&mtk_spm->spm_clk_con, (0x10 << 24) | (0x7 << 4));

	/* clean wakeup event raw status */
	write32(&mtk_spm->spm_wakeup_event_mask, SPM_WAKEUP_EVENT_MASK_DEF);

	/* clean ISR status */
	write32(&mtk_spm->spm_irq_mask, ISRM_ALL);
	write32(&mtk_spm->spm_irq_sta, ISRC_ALL);
	write32(&mtk_spm->spm_swint_clr, PCM_SW_INT_ALL);

	setbits32(&mtk_spm->spm_ap_standby_con, BIT(1));

	write32(&mtk_spm->spm_resource_ack_con[0], 0xCC4E4ECC);
	write32(&mtk_spm->spm_resource_ack_con[1], 0x00CCCCCC);

	/* Init Other Reg */
	write32(&mtk_spm->spm_ack_chk[3].sel, SPM_ACK_CHK_3_SEL_HW_S1);
	write32(&mtk_spm->spm_ack_chk[3].timer, SPM_ACK_CHK_3_HW_S1_CNT);
	spm_hw_s1_state_monitor(0);

	/* Init SPM DEBUG Output enable */
	write32(&mtk_spm->spm_debug_con, 0x1);
	/* Init for SPM Resource ACK */
	write32(&mtk_spm->spm_resource_ack_con[0], 0xCC4E4ECC);
	write32(&mtk_spm->spm_resource_ack_con[1], 0x00CCCCCC);

	clrbits32(&mtk_vlpdevapc->vlp_ao_apc_con, BIT(2));
	setbits32(&mtk_vlpdevapc->vlp_ao_mas_sec_0, 0x3);
	/* infra sw cg mask for all bit */
	write32p(INFRA_SW_CG, 0xFFFFFFFF);
	/* debug_mon for spm pwr_status_msb mtcmos ack check */
	write32(&mtk_vlpcfg->debug_mon_reg[0], (0x79U << 16) | (0x78U << 0));
	write32(&mtk_vlpcfg->debug_mon_reg[1], (0x7BU << 16) | (0x7AU << 0));

	spm_init_ulposc_ctrl();

	spm_set_power_control(&spm_init_ctrl);
}

void spm_reset_and_init_pcm(void)
{
	/* disable r0 and r7 to control power */
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* disable pcm timer after leaving FW */
	clrsetbits32(&mtk_spm->pcm_con1,
		     REG_PCM_TIMER_EN_LSB, SPM_REGWR_CFG_KEY);

	/* reset PCM */
	write32(&mtk_spm->pcm_con0,
		SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* init PCM_CON1 (disable PCM timer but keep PCM WDT setting) */
	clrsetbits32(&mtk_spm->pcm_con1, ~REG_PCM_WDT_WAKE_LSB,
		     SPM_REGWR_CFG_KEY | REG_SPM_APB_INTERNAL_EN_LSB |
		     REG_SSPM_APB_P2P_EN_LSB);
}

void spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl)
{
	u32 val, mask;

	/* toggle event counter clear */
	write32(&mtk_spm->spm_event_counter_clear, REG_SPM_EVENT_COUNTER_CLR_LSB);
	/* toggle for reset SYS TIMER start point */
	setbits32(&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB);

	if (pwrctrl->timer_val_cust == 0)
		val = pwrctrl->timer_val ? pwrctrl->timer_val : PCM_TIMER_SUSPEND;
	else
		val = pwrctrl->timer_val_cust;

	write32(&mtk_spm->pcm_timer_val, val);
	setbits32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | REG_PCM_TIMER_EN_LSB);

	/* unmask AP wakeup source */
	if (pwrctrl->wake_src_cust == 0)
		mask = pwrctrl->wake_src;
	else
		mask = pwrctrl->wake_src_cust;

	if (pwrctrl->reg_csyspwrup_ack_mask)
		mask &= ~R12_CSYSPWREQ_B;
	write32(&mtk_spm->spm_wakeup_event_mask, ~mask);

	/* unmask SPM ISR (keep TWAM setting) */
	setbits32(&mtk_spm->spm_irq_mask, ISRM_RET_IRQ_AUX);

	/* toggle event counter clear */
	write32(&mtk_spm->spm_event_counter_clear, 0);
	/* toggle for reset SYS TIMER start point */
	clrbits32(&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB);
}

void spm_init_pcm_register(void)
{
	write32(&mtk_spm->pcm_pwr_io_en, 0);
}

void spm_kick_pcm_to_run(const struct pwr_ctrl *pwrctrl)
{
	/* Waiting for loading SPMFW done*/
	while (read32(&mtk_spm->md32pcm_dma0_rlct) != 0x0)
		;

	/* In the new SOC design, this part has been simplified */
	spm_set_pcm_flags(pwrctrl);

	/* Kick PCM to run (only toggle PCM_KICK) */
	setbits32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* Reset md32pcm */
	SET32_BITFIELDS(&mtk_spm->md32pcm_cfgreg_sw_rstn,
			MD32PCM_CFGREG_SW_RSTN_RESET, 1);

	/* Waiting for SPM init done */
	udelay(SPM_INIT_DONE_US);
}

const struct pwr_ctrl *get_pwr_ctrl(void)
{
	return &spm_init_ctrl;
}
