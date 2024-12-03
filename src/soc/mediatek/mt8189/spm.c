/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <delay.h>
#include <soc/mcu_common.h>
#include <soc/pll.h>
#include <soc/spm.h>
#include <soc/spm_mtcmos.h>

static const struct pwr_ctrl spm_init_ctrl = {
	/* For SPM, this flag is not auto-gen. */
	.pcm_flags = SPM_FLAG_DISABLE_VCORE_DVS
		   | SPM_FLAG_DISABLE_DDR_DFS
		   | SPM_FLAG_DISABLE_EMI_DFS
		   | SPM_FLAG_DISABLE_BUS_DFS
		   | SPM_FLAG_RUN_COMMON_SCENARIO,

	/* SPM_SRC_REQ */
	.reg_spm_adsp_mailbox_req = 0,
	.reg_spm_apsrc_req = 0,
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
	.reg_apu_apsrc_req_mask_b = 0x1,
	.reg_apu_ddren_req_mask_b = 0x1,
	.reg_apu_emi_req_mask_b = 0x1,
	.reg_apu_infra_req_mask_b = 0x1,
	.reg_apu_pmic_req_mask_b = 0x1,
	.reg_apu_srcclkena_mask_b = 0x1,
	.reg_apu_vrf18_req_mask_b = 0x1,
	.reg_audio_dsp_apsrc_req_mask_b = 0x0,
	.reg_audio_dsp_ddren_req_mask_b = 0x0,
	.reg_audio_dsp_emi_req_mask_b = 0x0,
	.reg_audio_dsp_infra_req_mask_b = 0x0,
	.reg_audio_dsp_pmic_req_mask_b = 0x0,
	.reg_audio_dsp_srcclkena_mask_b = 0x0,
	.reg_audio_dsp_vcore_req_mask_b = 0x0,
	.reg_audio_dsp_vrf18_req_mask_b = 0x0,
	.reg_cam_apsrc_req_mask_b = 0x1,
	.reg_cam_ddren_req_mask_b = 0x1,
	.reg_cam_emi_req_mask_b = 0x1,
	.reg_cam_infra_req_mask_b = 0x0,
	.reg_cam_pmic_req_mask_b = 0x0,
	.reg_cam_srcclkena_mask_b = 0x0,
	.reg_cam_vrf18_req_mask_b = 0x0,
	.reg_mdp_emi_req_mask_b = 0x1,

	/* SPM_SRC_MASK_1 */
	.reg_ccif_apsrc_req_mask_b = 0x0,
	.reg_ccif_emi_req_mask_b = 0xfff,

	/* SPM_SRC_MASK_2 */
	.reg_ccif_infra_req_mask_b = 0x0,
	.reg_ccif_pmic_req_mask_b = 0xfff,

	/* SPM_SRC_MASK_3 */
	.reg_ccif_srcclkena_mask_b = 0x0,
	.reg_ccif_vrf18_req_mask_b = 0xfff,
	.reg_ccu_apsrc_req_mask_b = 0x0,
	.reg_ccu_ddren_req_mask_b = 0x0,
	.reg_ccu_emi_req_mask_b = 0x0,
	.reg_ccu_infra_req_mask_b = 0x0,
	.reg_ccu_pmic_req_mask_b = 0x0,
	.reg_ccu_srcclkena_mask_b = 0x0,
	.reg_ccu_vrf18_req_mask_b = 0x0,
	.reg_cg_check_apsrc_req_mask_b = 0x1,

	/* SPM_SRC_MASK_4 */
	.reg_cg_check_ddren_req_mask_b = 0x1,
	.reg_cg_check_emi_req_mask_b = 0x1,
	.reg_cg_check_infra_req_mask_b = 0x1,
	.reg_cg_check_pmic_req_mask_b = 0x1,
	.reg_cg_check_srcclkena_mask_b = 0x1,
	.reg_cg_check_vcore_req_mask_b = 0x1,
	.reg_cg_check_vrf18_req_mask_b = 0x1,
	.reg_conn_apsrc_req_mask_b = 0x1,
	.reg_conn_ddren_req_mask_b = 0x1,
	.reg_conn_emi_req_mask_b = 0x1,
	.reg_conn_infra_req_mask_b = 0x1,
	.reg_conn_pmic_req_mask_b = 0x1,
	.reg_conn_srcclkena_mask_b = 0x1,
	.reg_conn_srcclkenb_mask_b = 0x1,
	.reg_conn_vcore_req_mask_b = 0x1,
	.reg_conn_vrf18_req_mask_b = 0x1,
	.reg_cpueb_apsrc_req_mask_b = 0x1,
	.reg_cpueb_ddren_req_mask_b = 0x1,
	.reg_cpueb_emi_req_mask_b = 0x1,
	.reg_cpueb_infra_req_mask_b = 0x1,
	.reg_cpueb_pmic_req_mask_b = 0x1,
	.reg_cpueb_srcclkena_mask_b = 0x1,
	.reg_cpueb_vrf18_req_mask_b = 0x1,
	.reg_disp0_apsrc_req_mask_b = 0x1,
	.reg_disp0_ddren_req_mask_b = 0x1,
	.reg_disp0_emi_req_mask_b = 0x1,
	.reg_disp0_infra_req_mask_b = 0x1,
	.reg_disp0_pmic_req_mask_b = 0x0,
	.reg_disp0_srcclkena_mask_b = 0x0,
	.reg_disp0_vrf18_req_mask_b = 0x1,
	.reg_disp1_apsrc_req_mask_b = 0x0,
	.reg_disp1_ddren_req_mask_b = 0x0,

	/* SPM_SRC_MASK_5 */
	.reg_disp1_emi_req_mask_b = 0x0,
	.reg_disp1_infra_req_mask_b = 0x0,
	.reg_disp1_pmic_req_mask_b = 0x0,
	.reg_disp1_srcclkena_mask_b = 0x0,
	.reg_disp1_vrf18_req_mask_b = 0x0,
	.reg_dpm_apsrc_req_mask_b = 0xf,
	.reg_dpm_ddren_req_mask_b = 0xf,
	.reg_dpm_emi_req_mask_b = 0xf,
	.reg_dpm_infra_req_mask_b = 0xf,
	.reg_dpm_pmic_req_mask_b = 0xf,
	.reg_dpm_srcclkena_mask_b = 0xf,

	/* SPM_SRC_MASK_6 */
	.reg_dpm_vcore_req_mask_b = 0xf,
	.reg_dpm_vrf18_req_mask_b = 0xf,
	.reg_dpmaif_apsrc_req_mask_b = 0x1,
	.reg_dpmaif_ddren_req_mask_b = 0x1,
	.reg_dpmaif_emi_req_mask_b = 0x1,
	.reg_dpmaif_infra_req_mask_b = 0x1,
	.reg_dpmaif_pmic_req_mask_b = 0x1,
	.reg_dpmaif_srcclkena_mask_b = 0x1,
	.reg_dpmaif_vrf18_req_mask_b = 0x1,
	.reg_dvfsrc_level_req_mask_b = 0x1,
	.reg_emisys_apsrc_req_mask_b = 0x0,
	.reg_emisys_ddren_req_mask_b = 0x1,
	.reg_emisys_emi_req_mask_b = 0x0,
	.reg_gce_d_apsrc_req_mask_b = 0x1,
	.reg_gce_d_ddren_req_mask_b = 0x1,
	.reg_gce_d_emi_req_mask_b = 0x1,
	.reg_gce_d_infra_req_mask_b = 0x0,
	.reg_gce_d_pmic_req_mask_b = 0x0,
	.reg_gce_d_srcclkena_mask_b = 0x0,
	.reg_gce_d_vrf18_req_mask_b = 0x0,
	.reg_gce_m_apsrc_req_mask_b = 0x1,
	.reg_gce_m_ddren_req_mask_b = 0x1,
	.reg_gce_m_emi_req_mask_b = 0x1,
	.reg_gce_m_infra_req_mask_b = 0x0,
	.reg_gce_m_pmic_req_mask_b = 0x0,
	.reg_gce_m_srcclkena_mask_b = 0x0,

	/* SPM_SRC_MASK_7 */
	.reg_gce_m_vrf18_req_mask_b = 0x0,
	.reg_gpueb_apsrc_req_mask_b = 0x0,
	.reg_gpueb_ddren_req_mask_b = 0x0,
	.reg_gpueb_emi_req_mask_b = 0x0,
	.reg_gpueb_infra_req_mask_b = 0x0,
	.reg_gpueb_pmic_req_mask_b = 0x0,
	.reg_gpueb_srcclkena_mask_b = 0x0,
	.reg_gpueb_vrf18_req_mask_b = 0x0,
	.reg_hwccf_apsrc_req_mask_b = 0x1,
	.reg_hwccf_ddren_req_mask_b = 0x1,
	.reg_hwccf_emi_req_mask_b = 0x1,
	.reg_hwccf_infra_req_mask_b = 0x1,
	.reg_hwccf_pmic_req_mask_b = 0x1,
	.reg_hwccf_srcclkena_mask_b = 0x1,
	.reg_hwccf_vcore_req_mask_b = 0x1,
	.reg_hwccf_vrf18_req_mask_b = 0x1,
	.reg_img_apsrc_req_mask_b = 0x1,
	.reg_img_ddren_req_mask_b = 0x1,
	.reg_img_emi_req_mask_b = 0x1,
	.reg_img_infra_req_mask_b = 0x0,
	.reg_img_pmic_req_mask_b = 0x0,
	.reg_img_srcclkena_mask_b = 0x0,
	.reg_img_vrf18_req_mask_b = 0x0,
	.reg_infrasys_apsrc_req_mask_b = 0x1,
	.reg_infrasys_ddren_req_mask_b = 0x1,
	.reg_infrasys_emi_req_mask_b = 0x1,
	.reg_ipic_infra_req_mask_b = 0x1,
	.reg_ipic_vrf18_req_mask_b = 0x1,
	.reg_mcu_apsrc_req_mask_b = 0x0,
	.reg_mcu_ddren_req_mask_b = 0x0,
	.reg_mcu_emi_req_mask_b = 0x0,

	/* SPM_SRC_MASK_8 */
	.reg_mcusys_apsrc_req_mask_b = 0x7,
	.reg_mcusys_ddren_req_mask_b = 0x7,
	.reg_mcusys_emi_req_mask_b = 0x7,
	.reg_mcusys_infra_req_mask_b = 0x0,

	/* SPM_SRC_MASK_9 */
	.reg_mcusys_pmic_req_mask_b = 0x0,
	.reg_mcusys_srcclkena_mask_b = 0x0,
	.reg_mcusys_vrf18_req_mask_b = 0x0,
	.reg_md_apsrc_req_mask_b = 0x0,
	.reg_md_ddren_req_mask_b = 0x0,
	.reg_md_emi_req_mask_b = 0x0,
	.reg_md_infra_req_mask_b = 0x0,
	.reg_md_pmic_req_mask_b = 0x0,
	.reg_md_srcclkena_mask_b = 0x0,
	.reg_md_srcclkena1_mask_b = 0x0,
	.reg_md_vcore_req_mask_b = 0x0,

	/* SPM_SRC_MASK_10 */
	.reg_md_vrf18_req_mask_b = 0x0,
	.reg_mdp_apsrc_req_mask_b = 0x0,
	.reg_mdp_ddren_req_mask_b = 0x0,
	.reg_mm_proc_apsrc_req_mask_b = 0x0,
	.reg_mm_proc_ddren_req_mask_b = 0x0,
	.reg_mm_proc_emi_req_mask_b = 0x0,
	.reg_mm_proc_infra_req_mask_b = 0x0,
	.reg_mm_proc_pmic_req_mask_b = 0x0,
	.reg_mm_proc_srcclkena_mask_b = 0x0,
	.reg_mm_proc_vrf18_req_mask_b = 0x0,
	.reg_mmsys_apsrc_req_mask_b = 0x0,
	.reg_mmsys_ddren_req_mask_b = 0x0,
	.reg_mmsys_vrf18_req_mask_b = 0x0,
	.reg_pcie0_apsrc_req_mask_b = 0x0,
	.reg_pcie0_ddren_req_mask_b = 0x0,
	.reg_pcie0_infra_req_mask_b = 0x0,
	.reg_pcie0_srcclkena_mask_b = 0x0,
	.reg_pcie0_vrf18_req_mask_b = 0x0,
	.reg_pcie1_apsrc_req_mask_b = 0x0,
	.reg_pcie1_ddren_req_mask_b = 0x0,
	.reg_pcie1_infra_req_mask_b = 0x0,
	.reg_pcie1_srcclkena_mask_b = 0x0,
	.reg_pcie1_vrf18_req_mask_b = 0x0,
	.reg_perisys_apsrc_req_mask_b = 0x1,
	.reg_perisys_ddren_req_mask_b = 0x1,
	.reg_perisys_emi_req_mask_b = 0x1,
	.reg_perisys_infra_req_mask_b = 0x1,
	.reg_perisys_pmic_req_mask_b = 0x1,
	.reg_perisys_srcclkena_mask_b = 0x1,
	.reg_perisys_vcore_req_mask_b = 0x1,
	.reg_perisys_vrf18_req_mask_b = 0x1,
	.reg_scp_apsrc_req_mask_b = 0x1,

	/* SPM_SRC_MASK_11 */
	.reg_scp_ddren_req_mask_b = 0x1,
	.reg_scp_emi_req_mask_b = 0x1,
	.reg_scp_infra_req_mask_b = 0x1,
	.reg_scp_pmic_req_mask_b = 0x1,
	.reg_scp_srcclkena_mask_b = 0x1,
	.reg_scp_vcore_req_mask_b = 0x1,
	.reg_scp_vrf18_req_mask_b = 0x1,
	.reg_srcclkeni_infra_req_mask_b = 0x1,
	.reg_srcclkeni_pmic_req_mask_b = 0x1,
	.reg_srcclkeni_srcclkena_mask_b = 0x1,
	.reg_sspm_apsrc_req_mask_b = 0x1,
	.reg_sspm_ddren_req_mask_b = 0x1,
	.reg_sspm_emi_req_mask_b = 0x1,
	.reg_sspm_infra_req_mask_b = 0x1,
	.reg_sspm_pmic_req_mask_b = 0x1,
	.reg_sspm_srcclkena_mask_b = 0x1,
	.reg_sspm_vrf18_req_mask_b = 0x1,
	.reg_ssr_apsrc_req_mask_b = 0x0,
	.reg_ssr_ddren_req_mask_b = 0x0,
	.reg_ssr_emi_req_mask_b = 0x0,
	.reg_ssr_infra_req_mask_b = 0x0,
	.reg_ssr_pmic_req_mask_b = 0x0,
	.reg_ssr_srcclkena_mask_b = 0x0,
	.reg_ssr_vrf18_req_mask_b = 0x0,
	.reg_ufs_apsrc_req_mask_b = 0x1,
	.reg_ufs_ddren_req_mask_b = 0x1,
	.reg_ufs_emi_req_mask_b = 0x1,
	.reg_ufs_infra_req_mask_b = 0x1,
	.reg_ufs_pmic_req_mask_b = 0x1,

	/* SPM_SRC_MASK_12 */
	.reg_ufs_srcclkena_mask_b = 0x1,
	.reg_ufs_vrf18_req_mask_b = 0x1,
	.reg_vdec_apsrc_req_mask_b = 0x1,
	.reg_vdec_ddren_req_mask_b = 0x1,
	.reg_vdec_emi_req_mask_b = 0x1,
	.reg_vdec_infra_req_mask_b = 0x0,
	.reg_vdec_pmic_req_mask_b = 0x0,
	.reg_vdec_srcclkena_mask_b = 0x0,
	.reg_vdec_vrf18_req_mask_b = 0x0,
	.reg_venc_apsrc_req_mask_b = 0x1,
	.reg_venc_ddren_req_mask_b = 0x1,
	.reg_venc_emi_req_mask_b = 0x1,
	.reg_venc_infra_req_mask_b = 0x0,
	.reg_venc_pmic_req_mask_b = 0x0,
	.reg_venc_srcclkena_mask_b = 0x0,
	.reg_venc_vrf18_req_mask_b = 0x0,
	.reg_ipe_apsrc_req_mask_b = 0x1,
	.reg_ipe_ddren_req_mask_b = 0x1,
	.reg_ipe_emi_req_mask_b = 0x1,
	.reg_ipe_infra_req_mask_b = 0x1,
	.reg_ipe_pmic_req_mask_b = 0x1,
	.reg_ipe_srcclkena_mask_b = 0x1,
	.reg_ipe_vrf18_req_mask_b = 0x1,
	.reg_ufs_vcore_req_mask_b = 0x1,

	/* SPM_EVENT_CON_MISC */
	.reg_srcclken_fast_resp = 0,
	.reg_csyspwrup_ack_mask = 0,

	/* Auto-gen End */

	/* SPM_WAKEUP_EVENT_MASK */
	.reg_wakeup_event_mask = 0xEFFFFFFF,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.reg_ext_wakeup_event_mask = 0xFFFFFFFF,
};

static void spm_hw_s1_state_monitor_pause(void)
{
	SET32_BITFIELDS(&mtk_spm->spm_ack_chk_con_3,
			SPM_ACK_CHK_3_CON_HW_MODE_TRIG, 1,
			SPM_ACK_CHK_3_CON_CLR_ALL, 1,
			SPM_ACK_CHK_3_CON_EN_0, 0,
			SPM_ACK_CHK_3_CON_EN_1, 0);
}

void spm_set_power_control(const struct pwr_ctrl *pwrctrl)
{
	/* SPM_SRC_REQ */
	write32(&mtk_spm->spm_src_req,
		((pwrctrl->reg_spm_adsp_mailbox_req & 0x1) << 0) |
		(((pwrctrl->reg_spm_apsrc_req) & 0x1) << 1) |
		(((pwrctrl->reg_spm_ddren_req) & 0x1) << 2) |
		((pwrctrl->reg_spm_dvfs_req & 0x1) << 3) |
		(((pwrctrl->reg_spm_emi_req) & 0x1) << 4) |
		(((pwrctrl->reg_spm_f26m_req) & 0x1) << 5) |
		(((pwrctrl->reg_spm_infra_req) & 0x1) << 6) |
		(((pwrctrl->reg_spm_pmic_req) & 0x1) << 7) |
		((pwrctrl->reg_spm_scp_mailbox_req & 0x1) << 8) |
		((pwrctrl->reg_spm_sspm_mailbox_req & 0x1) << 9) |
		((pwrctrl->reg_spm_sw_mailbox_req & 0x1) << 10) |
		(((pwrctrl->reg_spm_vcore_req) & 0x1) << 11) |
		(((pwrctrl->reg_spm_vrf18_req) & 0x1) << 12) |
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
	write32(&mtk_spm->spm_src_mask_0,
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
		((pwrctrl->reg_cam_vrf18_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_mdp_emi_req_mask_b & 0x1) << 22));

	/* SPM_SRC_MASK_1 */
	write32(&mtk_spm->spm_src_mask_1,
		((pwrctrl->reg_ccif_apsrc_req_mask_b & 0xfff) << 0) |
		((pwrctrl->reg_ccif_emi_req_mask_b & 0xfff) << 12));

	/* SPM_SRC_MASK_2 */
	write32(&mtk_spm->spm_src_mask_2,
		((pwrctrl->reg_ccif_infra_req_mask_b & 0xfff) << 0) |
		((pwrctrl->reg_ccif_pmic_req_mask_b & 0xfff) << 12));

	/* SPM_SRC_MASK_3 */
	write32(&mtk_spm->spm_src_mask_3,
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
	write32(&mtk_spm->spm_src_mask_4,
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
	write32(&mtk_spm->spm_src_mask_5,
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
	write32(&mtk_spm->spm_src_mask_6,
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
		((pwrctrl->reg_gce_d_apsrc_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_gce_d_ddren_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_gce_d_emi_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_gce_d_infra_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_gce_d_pmic_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_gce_d_srcclkena_mask_b & 0x1) << 24) |
		((pwrctrl->reg_gce_d_vrf18_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_gce_m_apsrc_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_gce_m_ddren_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_gce_m_emi_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_gce_m_infra_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_gce_m_pmic_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_gce_m_srcclkena_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_7 */
	write32(&mtk_spm->spm_src_mask_7,
		((pwrctrl->reg_gce_m_vrf18_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_gpueb_apsrc_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_gpueb_ddren_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_gpueb_emi_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_gpueb_infra_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_gpueb_pmic_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_gpueb_srcclkena_mask_b & 0x1) << 6) |
		((pwrctrl->reg_gpueb_vrf18_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_hwccf_apsrc_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_hwccf_ddren_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_hwccf_emi_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_hwccf_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_hwccf_pmic_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_hwccf_srcclkena_mask_b & 0x1) << 13) |
		((pwrctrl->reg_hwccf_vcore_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_hwccf_vrf18_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_img_apsrc_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_img_ddren_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_img_emi_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_img_infra_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_img_pmic_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_img_srcclkena_mask_b & 0x1) << 21) |
		((pwrctrl->reg_img_vrf18_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_infrasys_apsrc_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_infrasys_ddren_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_infrasys_emi_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_ipic_infra_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_ipic_vrf18_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_mcu_apsrc_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_mcu_ddren_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_mcu_emi_req_mask_b & 0x1) << 30));

	/* SPM_SRC_MASK_8 */
	write32(&mtk_spm->spm_src_mask_8,
		((pwrctrl->reg_mcusys_apsrc_req_mask_b & 0xff) << 0) |
		((pwrctrl->reg_mcusys_ddren_req_mask_b & 0xff) << 8) |
		((pwrctrl->reg_mcusys_emi_req_mask_b & 0xff) << 16) |
		((pwrctrl->reg_mcusys_infra_req_mask_b & 0xff) << 24));

	/* SPM_SRC_MASK_9 */
	write32(&mtk_spm->spm_src_mask_9,
		((pwrctrl->reg_mcusys_pmic_req_mask_b & 0xff) << 0) |
		((pwrctrl->reg_mcusys_srcclkena_mask_b & 0xff) << 8) |
		((pwrctrl->reg_mcusys_vrf18_req_mask_b & 0xff) << 16) |
		((pwrctrl->reg_md_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_md_ddren_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_md_emi_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_md_infra_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_md_pmic_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_md_srcclkena_mask_b & 0x1) << 29) |
		((pwrctrl->reg_md_srcclkena1_mask_b & 0x1) << 30) |
		((pwrctrl->reg_md_vcore_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_10 */
	write32(&mtk_spm->spm_src_mask_10,
		((pwrctrl->reg_md_vrf18_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_mdp_apsrc_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_mdp_ddren_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_mm_proc_apsrc_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_mm_proc_ddren_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_mm_proc_emi_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_mm_proc_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_mm_proc_pmic_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_mm_proc_srcclkena_mask_b & 0x1) << 8) |
		((pwrctrl->reg_mm_proc_vrf18_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_mmsys_apsrc_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_mmsys_ddren_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_mmsys_vrf18_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_pcie0_apsrc_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_pcie0_ddren_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_pcie0_infra_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_pcie0_srcclkena_mask_b & 0x1) << 16) |
		((pwrctrl->reg_pcie0_vrf18_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_pcie1_apsrc_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_pcie1_ddren_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_pcie1_infra_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_pcie1_srcclkena_mask_b & 0x1) << 21) |
		((pwrctrl->reg_pcie1_vrf18_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_perisys_apsrc_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_perisys_ddren_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_perisys_emi_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_perisys_infra_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_perisys_pmic_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_perisys_srcclkena_mask_b & 0x1) << 28) |
		((pwrctrl->reg_perisys_vcore_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_perisys_vrf18_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_scp_apsrc_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_11 */
	write32(&mtk_spm->spm_src_mask_11,
		((pwrctrl->reg_scp_ddren_req_mask_b & 0x1) << 0) |
		((pwrctrl->reg_scp_emi_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_scp_infra_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_scp_pmic_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_scp_srcclkena_mask_b & 0x1) << 4) |
		((pwrctrl->reg_scp_vcore_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_scp_vrf18_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_srcclkeni_infra_req_mask_b & 0x3) << 7) |
		((pwrctrl->reg_srcclkeni_pmic_req_mask_b & 0x3) << 9) |
		((pwrctrl->reg_srcclkeni_srcclkena_mask_b & 0x3) << 11) |
		((pwrctrl->reg_sspm_apsrc_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_sspm_ddren_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_sspm_emi_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_sspm_infra_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_sspm_pmic_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_sspm_srcclkena_mask_b & 0x1) << 18) |
		((pwrctrl->reg_sspm_vrf18_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_ssr_apsrc_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_ssr_ddren_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_ssr_emi_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_ssr_infra_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_ssr_pmic_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_ssr_srcclkena_mask_b & 0x1) << 25) |
		((pwrctrl->reg_ssr_vrf18_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_ufs_apsrc_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_ufs_ddren_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_ufs_emi_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_ufs_infra_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_ufs_pmic_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_12 */
	write32(&mtk_spm->spm_src_mask_12,
		((pwrctrl->reg_ufs_srcclkena_mask_b & 0x1) << 0) |
		((pwrctrl->reg_ufs_vrf18_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_vdec_apsrc_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_vdec_ddren_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_vdec_emi_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_vdec_infra_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_vdec_pmic_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_vdec_srcclkena_mask_b & 0x1) << 7) |
		((pwrctrl->reg_vdec_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_venc_apsrc_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_venc_ddren_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_venc_emi_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_venc_infra_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_venc_pmic_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_venc_srcclkena_mask_b & 0x1) << 14) |
		((pwrctrl->reg_venc_vrf18_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_ipe_apsrc_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_ipe_ddren_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_ipe_emi_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_ipe_infra_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_ipe_pmic_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_ipe_srcclkena_mask_b & 0x1) << 21) |
		((pwrctrl->reg_ipe_vrf18_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_ufs_vcore_req_mask_b & 0x1) << 23));

	/* SPM_EVENT_CON_MISC */
	write32(&mtk_spm->spm_event_con_misc,
		((pwrctrl->reg_srcclken_fast_resp & 0x1) << 0) |
		((pwrctrl->reg_csyspwrup_ack_mask & 0x1) << 1));

	/* SPM_WAKEUP_EVENT_MASK */
	write32(&mtk_spm->spm_wakeup_event_mask,
		((pwrctrl->reg_wakeup_event_mask & 0xffffffff) << 0));

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	write32(&mtk_spm->spm_wakeup_event_ext_mask,
		((pwrctrl->reg_ext_wakeup_event_mask & 0xffffffff) << 0));
}

void spm_register_init(void)
{
	/* SPM related clk setting  */
	setbits32(&mtk_infracfg_ao->infra_bus_dcm_ctrl,
		  RG_AXI_DCM_DIS_EN | RG_PLLCK_SEL_NO_SPM);

	/* Enable scpsys(SPM) clock off control, 0: Topck AO */
	setbits32(&mtk_topckgen->clk_scp_cfg_0, GENMASK(9, 0));
	clrsetbits32(&mtk_topckgen->clk_scp_cfg_1, BIT(12) | GENMASK(3, 2), GENMASK(1, 0));

	/* enable register control */
	write32(&mtk_spm->poweron_config_set, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Power on spm sram */
	clrbits32(&mtk_spm->spm_sram_con, GENMASK(19, 16));

	/* SRAM Workaround for Sleep FSM */
	setbits32(&mtk_spm->spm_sram_sleep_ctrl, BIT(9));

	/* init power control register */
	/* dram will set this register */
	/* write32(SPM_POWER_ON_VAL0, POWER_ON_VAL0_DEF); */
	write32(&mtk_spm->spm_power_on_val[1], POWER_ON_VAL1_DEF);

	/* reset PCM */
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
	write32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY |
		REG_SPM_APB_INTERNAL_EN_LSB | REG_SSPM_APB_P2P_EN_LSB);
	/* initial SPM CLK control register */
	clrsetbits32(&mtk_spm->spm_clk_con, GENMASK(23, 16), BIT(28) | GENMASK(6, 4));
	setbits32(&mtk_spm->spm_clk_con1, BIT(0));

	/* clean wakeup event raw status */
	write32(&mtk_spm->spm_wakeup_event_mask, SPM_WAKEUP_EVENT_MASK_DEF);

	/* clean ISR status */
	write32(&mtk_spm->spm_irq_mask, ISRM_ALL);
	write32(&mtk_spm->spm_irq_sta, ISRC_ALL);
	write32(&mtk_spm->spm_swint_clr, PCM_SW_INT_ALL);

	/* set SPM_AP_STANDBY_CON[5] csyspower_req_mask = 1 */
	setbits32(&mtk_spm->spm_ap_standby_con, BIT(5));

	/* disable r0 and r7 to control power */
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* DDR EN de-bounce length to 5us */
	write32(&mtk_spm->ddren_dbc_con, 0x154 | REG_DDREN_DBC_EN_LSB);

	/* Init for SPM Resource ACK */
	write32(&mtk_spm->spm_resource_ack_con0, 0xCC4E4ECC);
	write32(&mtk_spm->spm_resource_ack_con1, 0x00CCCCCC);
	write32(&mtk_spm->spm_resource_ack_mask0, 0x00000000);
	write32(&mtk_spm->spm_resource_ack_mask1, 0x00000000);
	write32(&mtk_spm->spm_resource_ack_mask2, 0x00000000);
	write32(&mtk_spm->spm_resource_ack_mask3, 0x00000000);
	write32(&mtk_spm->spm_resource_ack_mask4, 0x00000000);
	write32(&mtk_spm->spm_resource_ack_mask5, 0x00000000);
	write32(&mtk_spm->spm_resource_ack_mask6, 0x00000000);

	/* Init VCORE DVFS Status */
	clrsetbits32(&mtk_spm->spm_dvfs_con, SPM_DVFS_FORCE_ENABLE_LSB, SPM_DVFSRC_ENABLE_LSB);
	write32(&mtk_spm->spm_dvfs_level, 0x00000001);
	write32(&mtk_spm->spm_dvs_dfs_level, 0x00010001);

	write32(&mtk_spm->spm_ack_chk_sel_3, SPM_ACK_CHK_3_SEL_HW_S1);
	write32(&mtk_spm->spm_ack_chk_timer_3, SPM_ACK_CHK_3_HW_S1_CNT);

	spm_hw_s1_state_monitor_pause();
}

const struct pwr_ctrl *get_pwr_ctrl(void)
{
	return &spm_init_ctrl;
}
