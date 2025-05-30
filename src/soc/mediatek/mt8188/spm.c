/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/mcu_common.h>
#include <soc/pll.h>
#include <soc/spm.h>

static struct pwr_ctrl spm_init_ctrl = {
	/* For SPM, this flag is not auto-gen. */
	.pcm_flags = SPM_FLAG_DISABLE_VCORE_DVS |
		     SPM_FLAG_DISABLE_VCORE_DFS |
		     SPM_FLAG_RUN_COMMON_SCENARIO,
	/* SPM_AP_STANDBY_CON */
	/* [0] */
	.reg_wfi_op = 0,
	/* [1] */
	.reg_wfi_type = 0,
	/* [2] */
	.reg_mp0_cputop_idle_mask = 0,
	/* [3] */
	.reg_mp1_cputop_idle_mask = 0,
	/* [4] */
	.reg_mcusys_idle_mask = 0,
	/* [25] */
	.reg_md_apsrc_1_sel = 0,
	/* [26] */
	.reg_md_apsrc_0_sel = 0,
	/* [29] */
	.reg_conn_apsrc_sel = 0,

	/* SPM_SRC_REQ */
	/* [0] */
	.reg_spm_apsrc_req = 0,
	/* [1] */
	.reg_spm_f26m_req = 0,
	/* [3] */
	.reg_spm_infra_req = 0,
	/* [4] */
	.reg_spm_vrf18_req = 0,
	/* [7] */
	.reg_spm_ddr_en_req = 0,
	/* [8] */
	.reg_spm_dvfs_req = 0,
	/* [9] */
	.reg_spm_sw_mailbox_req = 0,
	/* [10] */
	.reg_spm_sspm_mailbox_req = 0,
	/* [11] */
	.reg_spm_adsp_mailbox_req = 0,
	/* [12] */
	.reg_spm_scp_mailbox_req = 0,


	/* SPM_SRC_MASK */
	/* [0] */
	.reg_sspm_srcclkena_0_mask_b = 1,
	/* [1] */
	.reg_sspm_infra_req_0_mask_b = 1,
	/* [2] */
	.reg_sspm_apsrc_req_0_mask_b = 1,
	/* [3] */
	.reg_sspm_vrf18_req_0_mask_b = 1,
	/* [4] */
	.reg_sspm_ddr_en_0_mask_b = 1,
	/* [5] */
	.reg_scp_srcclkena_mask_b = 1,
	/* [6] */
	.reg_scp_infra_req_mask_b = 1,
	/* [7] */
	.reg_scp_apsrc_req_mask_b = 1,
	/* [8] */
	.reg_scp_vrf18_req_mask_b = 1,
	/* [9] */
	.reg_scp_ddr_en_mask_b = 1,
	/* [10] */
	.reg_audio_dsp_srcclkena_mask_b = 1,
	/* [11] */
	.reg_audio_dsp_infra_req_mask_b = 1,
	/* [12] */
	.reg_audio_dsp_apsrc_req_mask_b = 1,
	/* [13] */
	.reg_audio_dsp_vrf18_req_mask_b = 1,
	/* [14] */
	.reg_audio_dsp_ddr_en_mask_b = 1,
	/* [15] */
	.reg_apu_srcclkena_mask_b = 1,
	/* [16] */
	.reg_apu_infra_req_mask_b = 1,
	/* [17] */
	.reg_apu_apsrc_req_mask_b = 1,
	/* [18] */
	.reg_apu_vrf18_req_mask_b = 1,
	/* [19] */
	.reg_apu_ddr_en_mask_b = 1,
	/* [20] */
	.reg_cpueb_srcclkena_mask_b = 1,
	/* [21] */
	.reg_cpueb_infra_req_mask_b = 1,
	/* [22] */
	.reg_cpueb_apsrc_req_mask_b = 1,
	/* [23] */
	.reg_cpueb_vrf18_req_mask_b = 1,
	/* [24] */
	.reg_cpueb_ddr_en_mask_b = 1,
	/* [25] */
	.reg_bak_psri_srcclkena_mask_b = 0,
	/* [26] */
	.reg_bak_psri_infra_req_mask_b = 0,
	/* [27] */
	.reg_bak_psri_apsrc_req_mask_b = 0,
	/* [28] */
	.reg_bak_psri_vrf18_req_mask_b = 0,
	/* [29] */
	.reg_bak_psri_ddr_en_mask_b = 0,
	/* [30] */
	.reg_cam_ddren_req_mask_b = 1,
	/* [31] */
	.reg_img_ddren_req_mask_b = 1,

	/* SPM_SRC2_MASK */
	/* [0] */
	.reg_msdc0_srcclkena_mask_b = 1,
	/* [1] */
	.reg_msdc0_infra_req_mask_b = 1,
	/* [2] */
	.reg_msdc0_apsrc_req_mask_b = 1,
	/* [3] */
	.reg_msdc0_vrf18_req_mask_b = 1,
	/* [4] */
	.reg_msdc0_ddr_en_mask_b = 1,
	/* [5] */
	.reg_msdc1_srcclkena_mask_b = 1,
	/* [6] */
	.reg_msdc1_infra_req_mask_b = 1,
	/* [7] */
	.reg_msdc1_apsrc_req_mask_b = 1,
	/* [8] */
	.reg_msdc1_vrf18_req_mask_b = 1,
	/* [9] */
	.reg_msdc1_ddr_en_mask_b = 1,
	/* [10] */
	.reg_msdc2_srcclkena_mask_b = 1,
	/* [11] */
	.reg_msdc2_infra_req_mask_b = 1,
	/* [12] */
	.reg_msdc2_apsrc_req_mask_b = 1,
	/* [13] */
	.reg_msdc2_vrf18_req_mask_b = 1,
	/* [14] */
	.reg_msdc2_ddr_en_mask_b = 1,
	/* [15] */
	.reg_ufs_srcclkena_mask_b = 1,
	/* [16] */
	.reg_ufs_infra_req_mask_b = 1,
	/* [17] */
	.reg_ufs_apsrc_req_mask_b = 1,
	/* [18] */
	.reg_ufs_vrf18_req_mask_b = 1,
	/* [19] */
	.reg_ufs_ddr_en_mask_b = 1,
	/* [20] */
	.reg_usb_srcclkena_mask_b = 1,
	/* [21] */
	.reg_usb_infra_req_mask_b = 1,
	/* [22] */
	.reg_usb_apsrc_req_mask_b = 1,
	/* [23] */
	.reg_usb_vrf18_req_mask_b = 1,
	/* [24] */
	.reg_usb_ddr_en_mask_b = 1,
	/* [25] */
	.reg_pextp_p0_srcclkena_mask_b = 1,
	/* [26] */
	.reg_pextp_p0_infra_req_mask_b = 1,
	/* [27] */
	.reg_pextp_p0_apsrc_req_mask_b = 1,
	/* [28] */
	.reg_pextp_p0_vrf18_req_mask_b = 1,
	/* [29] */
	.reg_pextp_p0_ddr_en_mask_b = 1,

	/* SPM_SRC3_MASK */
	/* [0] */
	.reg_pextp_p1_srcclkena_mask_b = 1,
	/* [1] */
	.reg_pextp_p1_infra_req_mask_b = 1,
	/* [2] */
	.reg_pextp_p1_apsrc_req_mask_b = 1,
	/* [3] */
	.reg_pextp_p1_vrf18_req_mask_b = 1,
	/* [4] */
	.reg_pextp_p1_ddr_en_mask_b = 1,
	/* [5] */
	.reg_gce0_infra_req_mask_b = 1,
	/* [6] */
	.reg_gce0_apsrc_req_mask_b = 1,
	/* [7] */
	.reg_gce0_vrf18_req_mask_b = 1,
	/* [8] */
	.reg_gce0_ddr_en_mask_b = 1,
	/* [9] */
	.reg_gce1_infra_req_mask_b = 1,
	/* [10] */
	.reg_gce1_apsrc_req_mask_b = 1,
	/* [11] */
	.reg_gce1_vrf18_req_mask_b = 1,
	/* [12] */
	.reg_gce1_ddr_en_mask_b = 1,
	/* [13] */
	.reg_spm_srcclkena_reserved_mask_b = 1,
	/* [14] */
	.reg_spm_infra_req_reserved_mask_b = 1,
	/* [15] */
	.reg_spm_apsrc_req_reserved_mask_b = 1,
	/* [16] */
	.reg_spm_vrf18_req_reserved_mask_b = 1,
	/* [17] */
	.reg_spm_ddr_en_reserved_mask_b = 1,
	/* [18] */
	.reg_disp0_apsrc_req_mask_b = 1,
	/* [19] */
	.reg_disp0_ddr_en_mask_b = 1,
	/* [20] */
	.reg_disp1_apsrc_req_mask_b = 1,
	/* [21] */
	.reg_disp1_ddr_en_mask_b = 1,
	/* [22] */
	.reg_disp2_apsrc_req_mask_b = 1,
	/* [23] */
	.reg_disp2_ddr_en_mask_b = 1,
	/* [24] */
	.reg_disp3_apsrc_req_mask_b = 1,
	/* [25] */
	.reg_disp3_ddr_en_mask_b = 1,
	/* [26] */
	.reg_infrasys_apsrc_req_mask_b = 0,
	/* [27] */
	.reg_infrasys_ddr_en_mask_b = 1,

	/* [28] */
	.reg_cg_check_srcclkena_mask_b = 1,
	/* [29] */
	.reg_cg_check_apsrc_req_mask_b = 1,
	/* [30] */
	.reg_cg_check_vrf18_req_mask_b = 1,
	/* [31] */
	.reg_cg_check_ddr_en_mask_b = 1,

	/* SPM_SRC4_MASK */
	/* [8:0] */
	.reg_mcusys_merge_apsrc_req_mask_b = 0,
	/* [17:9] */
	.reg_mcusys_merge_ddr_en_mask_b = 0,
	/* [19:18] */
	.reg_dramc_md32_infra_req_mask_b = 3,
	/* [21:20] */
	.reg_dramc_md32_vrf18_req_mask_b = 3,
	/* [23:22] */
	.reg_dramc_md32_ddr_en_mask_b = 0,
	/* [24] */
	.reg_dvfsrc_event_trigger_mask_b = 1,

	/* SPM_WAKEUP_EVENT_MASK2 */
	/* [3:0] */
	.reg_sc_sw2spm_wakeup_mask_b = 0,
	/* [4] */
	.reg_sc_adsp2spm_wakeup_mask_b = 0,
	/* [8:5] */
	.reg_sc_sspm2spm_wakeup_mask_b = 0,
	/* [9] */
	.reg_sc_scp2spm_wakeup_mask_b = 0,
	/* [10] */
	.reg_csyspwrup_ack_mask = 0,
	/* [11] */
	.reg_csyspwrup_req_mask = 1,

	/* SPM_WAKEUP_EVENT_MASK */
	/* [31:0] */
	.reg_wakeup_event_mask = 0xC1382213,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	/* [31:0] */
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
	/* SPM_AP_STANDBY_CON */
	write32(&mtk_spm->spm_ap_standby_con,
		((pwrctrl->reg_wfi_op & 0x1) << 0) |
		((pwrctrl->reg_wfi_type & 0x1) << 1) |
		((pwrctrl->reg_mp0_cputop_idle_mask & 0x1) << 2) |
		((pwrctrl->reg_mp1_cputop_idle_mask & 0x1) << 3) |
		((pwrctrl->reg_mcusys_idle_mask & 0x1) << 4) |
		((pwrctrl->reg_md_apsrc_1_sel & 0x1) << 25) |
		((pwrctrl->reg_md_apsrc_0_sel & 0x1) << 26) |
		((pwrctrl->reg_conn_apsrc_sel & 0x1) << 29));

	/* SPM_SRC_REQ */
	write32(&mtk_spm->spm_src_req,
		((pwrctrl->reg_spm_apsrc_req & 0x1) << 0) |
		((pwrctrl->reg_spm_f26m_req & 0x1) << 1) |
		((pwrctrl->reg_spm_infra_req & 0x1) << 3) |
		((pwrctrl->reg_spm_vrf18_req & 0x1) << 4) |
		((pwrctrl->reg_spm_ddr_en_req & 0x1) << 7) |
		((pwrctrl->reg_spm_dvfs_req & 0x1) << 8) |
		((pwrctrl->reg_spm_sw_mailbox_req & 0x1) << 9) |
		((pwrctrl->reg_spm_sspm_mailbox_req & 0x1) << 10) |
		((pwrctrl->reg_spm_adsp_mailbox_req & 0x1) << 11) |
		((pwrctrl->reg_spm_scp_mailbox_req & 0x1) << 12));

	/* SPM_SRC_MASK */
	write32(&mtk_spm->spm_src_mask,
		((pwrctrl->reg_sspm_srcclkena_0_mask_b & 0x1) << 0) |
		((pwrctrl->reg_sspm_infra_req_0_mask_b & 0x1) << 1) |
		((pwrctrl->reg_sspm_apsrc_req_0_mask_b & 0x1) << 2) |
		((pwrctrl->reg_sspm_vrf18_req_0_mask_b & 0x1) << 3) |
		((pwrctrl->reg_sspm_ddr_en_0_mask_b & 0x1) << 4) |
		((pwrctrl->reg_scp_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_scp_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_scp_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_scp_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_scp_ddr_en_mask_b & 0x1) << 9) |
		((pwrctrl->reg_audio_dsp_srcclkena_mask_b & 0x1) << 10) |
		((pwrctrl->reg_audio_dsp_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_audio_dsp_apsrc_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_audio_dsp_vrf18_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_audio_dsp_ddr_en_mask_b & 0x1) << 14) |
		((pwrctrl->reg_apu_srcclkena_mask_b & 0x1) << 15) |
		((pwrctrl->reg_apu_infra_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_apu_apsrc_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_apu_vrf18_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_apu_ddr_en_mask_b & 0x1) << 19) |
		((pwrctrl->reg_cpueb_srcclkena_mask_b & 0x1) << 20) |
		((pwrctrl->reg_cpueb_infra_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_cpueb_apsrc_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_cpueb_vrf18_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_cpueb_ddr_en_mask_b & 0x1) << 24) |
		((pwrctrl->reg_bak_psri_srcclkena_mask_b & 0x1) << 25) |
		((pwrctrl->reg_bak_psri_infra_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_bak_psri_apsrc_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_bak_psri_vrf18_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_bak_psri_ddr_en_mask_b & 0x1) << 29) |
		((pwrctrl->reg_cam_ddren_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_img_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC2_MASK */
	write32(&mtk_spm->spm_src2_mask,
		((pwrctrl->reg_msdc0_srcclkena_mask_b & 0x1) << 0) |
		((pwrctrl->reg_msdc0_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_msdc0_apsrc_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_msdc0_vrf18_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_msdc0_ddr_en_mask_b & 0x1) << 4) |
		((pwrctrl->reg_msdc1_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_msdc1_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_msdc1_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_msdc1_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_msdc1_ddr_en_mask_b & 0x1) << 9) |
		((pwrctrl->reg_msdc2_srcclkena_mask_b & 0x1) << 10) |
		((pwrctrl->reg_msdc2_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_msdc2_apsrc_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_msdc2_vrf18_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_msdc2_ddr_en_mask_b & 0x1) << 14) |
		((pwrctrl->reg_ufs_srcclkena_mask_b & 0x1) << 15) |
		((pwrctrl->reg_ufs_infra_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_ufs_apsrc_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_ufs_vrf18_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_ufs_ddr_en_mask_b & 0x1) << 19) |
		((pwrctrl->reg_usb_srcclkena_mask_b & 0x1) << 20) |
		((pwrctrl->reg_usb_infra_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_usb_apsrc_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_usb_vrf18_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_usb_ddr_en_mask_b & 0x1) << 24) |
		((pwrctrl->reg_pextp_p0_srcclkena_mask_b & 0x1) << 25) |
		((pwrctrl->reg_pextp_p0_infra_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_pextp_p0_apsrc_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_pextp_p0_vrf18_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_pextp_p0_ddr_en_mask_b & 0x1) << 29));

	/* SPM_SRC3_MASK */
	write32(&mtk_spm->spm_src3_mask,
		((pwrctrl->reg_pextp_p1_srcclkena_mask_b & 0x1) << 0) |
		((pwrctrl->reg_pextp_p1_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_pextp_p1_apsrc_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_pextp_p1_vrf18_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_pextp_p1_ddr_en_mask_b & 0x1) << 4) |
		((pwrctrl->reg_gce0_infra_req_mask_b & 0x1) << 5) |
		((pwrctrl->reg_gce0_apsrc_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_gce0_vrf18_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_gce0_ddr_en_mask_b & 0x1) << 8) |
		((pwrctrl->reg_gce1_infra_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_gce1_apsrc_req_mask_b & 0x1) << 10) |
		((pwrctrl->reg_gce1_vrf18_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_gce1_ddr_en_mask_b & 0x1) << 12) |
		((pwrctrl->reg_spm_srcclkena_reserved_mask_b & 0x1) << 13) |
		((pwrctrl->reg_spm_infra_req_reserved_mask_b & 0x1) << 14) |
		((pwrctrl->reg_spm_apsrc_req_reserved_mask_b & 0x1) << 15) |
		((pwrctrl->reg_spm_vrf18_req_reserved_mask_b & 0x1) << 16) |
		((pwrctrl->reg_spm_ddr_en_reserved_mask_b & 0x1) << 17) |
		((pwrctrl->reg_disp0_ddr_en_mask_b & 0x1) << 18) |
		((pwrctrl->reg_disp0_ddr_en_mask_b & 0x1) << 19) |
		((pwrctrl->reg_disp1_apsrc_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_disp1_ddr_en_mask_b & 0x1) << 21) |
		((pwrctrl->reg_disp2_apsrc_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_disp2_ddr_en_mask_b & 0x1) << 23) |
		((pwrctrl->reg_disp3_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_disp3_ddr_en_mask_b & 0x1) << 25) |
		((pwrctrl->reg_infrasys_apsrc_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_infrasys_ddr_en_mask_b & 0x1) << 27));

	/* SPM_SRC4_MASK */
	write32(&mtk_spm->spm_src4_mask,
		((pwrctrl->reg_mcusys_merge_apsrc_req_mask_b & 0x1ff) << 0) |
		((pwrctrl->reg_mcusys_merge_ddr_en_mask_b & 0x1ff) << 9) |
		((pwrctrl->reg_dramc_md32_infra_req_mask_b & 0x3) << 18) |
		((pwrctrl->reg_dramc_md32_vrf18_req_mask_b & 0x3) << 20) |
		((pwrctrl->reg_dramc_md32_ddr_en_mask_b & 0x3) << 22) |
		((pwrctrl->reg_dvfsrc_event_trigger_mask_b & 0x1) << 24));

	/* SPM_WAKEUP_EVENT_MASK */
	write32(&mtk_spm->spm_wakeup_event_mask,
		((pwrctrl->reg_wakeup_event_mask & 0xffffffff) << 0));

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	write32(&mtk_spm->spm_wakeup_event_ext_mask,
		((pwrctrl->reg_ext_wakeup_event_mask & 0xffffffff) << 0));
}

void spm_register_init(void)
{
	/* set clock path for SPM */
	setbits32(&mtk_topckgen->clk_scp_cfg_0, 0x7ff);
	/* enable register control */
	write32(&mtk_spm->poweron_config_set, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);
	/* init power control register, dram will set this register */
	write32(&mtk_spm->spm_power_on_val1, POWER_ON_VAL1_DEF);
	write32(&mtk_spm->pcm_pwr_io_en, 0);
	/* reset PCM */
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB |
				    PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
	write32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | REG_EVENT_LOCK_EN_LSB |
				    REG_SPM_SRAM_ISOINT_B_LSB |
				    RG_AHBMIF_APBEN_LSB |
				    REG_MD32_APB_INTERNAL_EN_LSB);
	/* clean wakeup event raw status */
	write32(&mtk_spm->spm_wakeup_event_mask, SPM_WAKEUP_EVENT_MASK_DEF);
	/* clean ISR status */
	write32(&mtk_spm->spm_irq_mask, ISRM_ALL);
	write32(&mtk_spm->spm_irq_sta, ISRC_ALL);
	write32(&mtk_spm->spm_swint_clr, PCM_SW_INT_ALL);
	/* init r7 with POWER_ON_VAL1 */
	write32(&mtk_spm->pcm_reg_data_ini, read32(&mtk_spm->spm_power_on_val1));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R7);
	write32(&mtk_spm->pcm_pwr_io_en, 0);
	/* DDR EN de-bounce length to 5us */
	write32(&mtk_spm->ddren_dbc_con, 0x154 | REG_ALL_DDR_EN_DBC_EN_LSB);
	/* configure ARMPLL Control Mode for MCDI */
	write32(&mtk_spm->armpll_clk_sel, 0x3FF);
	/* init for SPM Resource ACK */
	write32(&mtk_spm->spm_resource_ack_con0, 0xFFFFFFFF);
	write32(&mtk_spm->spm_resource_ack_con1, 0xFFFFFFFF);
	write32(&mtk_spm->spm_resource_ack_con2, 0xFFFFFFFF);
	write32(&mtk_spm->spm_resource_ack_con3, 0xFFFFFFFF);
	/* init VCORE DVFS Status */
	write32(&mtk_spm->spm_dvfs_level, 0x00000001);
	write32(&mtk_spm->spm_dvs_dfs_level, 0x00010001);
	SET32_BITFIELDS(&mtk_spm->spm_dvfs_misc,
			SPM_DVFS_FORCE_ENABLE_LSB, 0,
			SPM_DVFSRC_ENABLE_LSB, 1);
	write32(&mtk_spm->spm_dvfs_level, 0x00000001);
	write32(&mtk_spm->spm_dvs_dfs_level, 0x00010001);
	/* HW S1 state monitor */
	write32(&mtk_spm->spm_ack_chk_sel_3, SPM_ACK_CHK_3_SEL_HW_S1);
	write32(&mtk_spm->spm_ack_chk_timer_3, SPM_ACK_CHK_3_HW_S1_CNT);
	spm_hw_s1_state_monitor_pause();
}

void spm_reset_and_init_pcm(void)
{
	uint32_t val;
	bool first_load_fw = true;

	/* check the SPM FW is run or not */
	if (read32(&mtk_spm->md32pcm_cfgreg_sw_rstn) &
	    MD32PCM_CFGREG_SW_RSTN_RUN)
		first_load_fw = false;

	if (!first_load_fw) {
		/* SPM code swapping */
		spm_code_swapping();
		/* backup PCM r0 -> SPM_POWER_ON_VAL0 before `reset PCM` */
		write32(&mtk_spm->spm_power_on_val0,
			read32(&mtk_spm->pcm_reg0_data));
	}

	/* disable r0 and r7 to control power */
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* disable pcm timer after leaving FW */
	clrsetbits32(&mtk_spm->pcm_con1, RG_PCM_TIMER_EN_LSB, SPM_REGWR_CFG_KEY);

	/* reset PCM */
	write32(&mtk_spm->pcm_con0,
		SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* clear SPM EVENT count */
	setbits32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | SPM_EVENT_COUNTER_CLR_LSB);
	clrsetbits32(&mtk_spm->pcm_con1, SPM_EVENT_COUNTER_CLR_LSB, SPM_REGWR_CFG_KEY);

	/* init PCM_CON1 (disable PCM timer but keep PCM WDT setting) */
	val = read32(&mtk_spm->pcm_con1) & RG_PCM_WDT_WAKE_LSB;
	write32(&mtk_spm->pcm_con1, val | SPM_REGWR_CFG_KEY |
				    REG_EVENT_LOCK_EN_LSB |
				    REG_SPM_SRAM_ISOINT_B_LSB |
				    RG_AHBMIF_APBEN_LSB |
				    REG_MD32_APB_INTERNAL_EN_LSB);
}

void spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl)
{
	uint32_t val, mask;

	/* toggle event counter clear */
	setbits32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | SPM_EVENT_COUNTER_CLR_LSB);
	/* toggle for reset SYS TIMER start point */
	SET32_BITFIELDS(&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB, 1);

	if (pwrctrl->timer_val_cust == 0)
		val = pwrctrl->timer_val ? pwrctrl->timer_val : PCM_TIMER_MAX;
	else
		val = pwrctrl->timer_val_cust;

	write32(&mtk_spm->pcm_timer_val, val);
	setbits32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | RG_PCM_TIMER_EN_LSB);

	/* unmask AP wakeup source */
	if (pwrctrl->wake_src_cust == 0)
		mask = pwrctrl->wake_src;
	else
		mask = pwrctrl->wake_src_cust;

	write32(&mtk_spm->spm_wakeup_event_mask, ~mask);

	/* unmask SPM ISR (keep TWAM setting) */
	setbits32(&mtk_spm->spm_irq_mask, ISRM_RET_IRQ_AUX);
	/* toggle event counter clear */
	clrsetbits32(&mtk_spm->pcm_con1, SPM_EVENT_COUNTER_CLR_LSB, SPM_REGWR_CFG_KEY);
	/* toggle for reset SYS TIMER start point */
	SET32_BITFIELDS(&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB, 0);
}

const struct pwr_ctrl *get_pwr_ctrl(void)
{
	return &spm_init_ctrl;
}
