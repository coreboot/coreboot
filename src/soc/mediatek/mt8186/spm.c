/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.5
 */

#include <assert.h>
#include <soc/mcu_common.h>
#include <soc/spm.h>

static const struct pwr_ctrl spm_init_ctrl = {
	/* For SPM, this flag is not auto-gen. */
	.pcm_flags = SPM_FLAG_DISABLE_VCORE_DVS |
		     SPM_FLAG_DISABLE_VCORE_DFS |
		     SPM_FLAG_RUN_COMMON_SCENARIO,

	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	.reg_wfi_op = 0,
	.reg_wfi_type = 0,
	.reg_mp0_cputop_idle_mask = 0,
	.reg_mp1_cputop_idle_mask = 0,
	.reg_mcusys_idle_mask = 0,
	.reg_md_apsrc_1_sel = 0,
	.reg_md_apsrc_0_sel = 0,
	.reg_conn_apsrc_sel = 0,

	/* SPM_SRC6_MASK */
	.reg_ccif_event_infra_req_mask_b = 0xFFFF,
	.reg_ccif_event_apsrc_req_mask_b = 0xFFFF,

	/* SPM_SRC_REQ */
	.reg_spm_apsrc_req = 0,
	.reg_spm_f26m_req = 0,
	.reg_spm_infra_req = 0,
	.reg_spm_vrf18_req = 0,
	.reg_spm_ddren_req = 0,
	.reg_spm_dvfs_req = 0,
	.reg_spm_sw_mailbox_req = 0,
	.reg_spm_sspm_mailbox_req = 0,
	.reg_spm_adsp_mailbox_req = 0,
	.reg_spm_scp_mailbox_req = 0,

	/* SPM_SRC_MASK */
	.reg_md_0_srcclkena_mask_b = 1,
	.reg_md_0_infra_req_mask_b = 1,
	.reg_md_0_apsrc_req_mask_b = 1,
	.reg_md_0_vrf18_req_mask_b = 1,
	.reg_md_0_ddren_req_mask_b = 1,
	.reg_md_1_srcclkena_mask_b = 0,
	.reg_md_1_infra_req_mask_b = 0,
	.reg_md_1_apsrc_req_mask_b = 0,
	.reg_md_1_vrf18_req_mask_b = 0,
	.reg_md_1_ddren_req_mask_b = 0,
	.reg_conn_srcclkena_mask_b = 1,
	.reg_conn_srcclkenb_mask_b = 0,
	.reg_conn_infra_req_mask_b = 1,
	.reg_conn_apsrc_req_mask_b = 1,
	.reg_conn_vrf18_req_mask_b = 1,
	.reg_conn_ddren_req_mask_b = 1,
	.reg_conn_vfe28_mask_b = 0,
	.reg_srcclkeni_srcclkena_mask_b = 1,
	.reg_srcclkeni_infra_req_mask_b = 1,
	.reg_infrasys_apsrc_req_mask_b = 0,
	.reg_infrasys_ddren_req_mask_b = 1,
	.reg_sspm_srcclkena_mask_b = 1,
	.reg_sspm_infra_req_mask_b = 1,
	.reg_sspm_apsrc_req_mask_b = 1,
	.reg_sspm_vrf18_req_mask_b = 1,
	.reg_sspm_ddren_req_mask_b = 1,

	/* SPM_SRC2_MASK */
	.reg_scp_srcclkena_mask_b = 1,
	.reg_scp_infra_req_mask_b = 1,
	.reg_scp_apsrc_req_mask_b = 1,
	.reg_scp_vrf18_req_mask_b = 1,
	.reg_scp_ddren_req_mask_b = 1,
	.reg_audio_dsp_srcclkena_mask_b = 1,
	.reg_audio_dsp_infra_req_mask_b = 1,
	.reg_audio_dsp_apsrc_req_mask_b = 1,
	.reg_audio_dsp_vrf18_req_mask_b = 1,
	.reg_audio_dsp_ddren_req_mask_b = 1,
	.reg_ufs_srcclkena_mask_b = 1,
	.reg_ufs_infra_req_mask_b = 1,
	.reg_ufs_apsrc_req_mask_b = 1,
	.reg_ufs_vrf18_req_mask_b = 1,
	.reg_ufs_ddren_req_mask_b = 1,
	.reg_disp0_apsrc_req_mask_b = 1,
	.reg_disp0_ddren_req_mask_b = 1,
	.reg_disp1_apsrc_req_mask_b = 1,
	.reg_disp1_ddren_req_mask_b = 1,
	.reg_gce_infra_req_mask_b = 1,
	.reg_gce_apsrc_req_mask_b = 1,
	.reg_gce_vrf18_req_mask_b = 1,
	.reg_gce_ddren_req_mask_b = 1,
	.reg_apu_srcclkena_mask_b = 0,
	.reg_apu_infra_req_mask_b = 0,
	.reg_apu_apsrc_req_mask_b = 0,
	.reg_apu_vrf18_req_mask_b = 0,
	.reg_apu_ddren_req_mask_b = 0,
	.reg_cg_check_srcclkena_mask_b = 0,
	.reg_cg_check_apsrc_req_mask_b = 0,
	.reg_cg_check_vrf18_req_mask_b = 0,
	.reg_cg_check_ddren_req_mask_b = 0,

	/* SPM_SRC3_MASK */
	.reg_dvfsrc_event_trigger_mask_b = 1,
	.reg_sw2spm_wakeup_mask_b = 0,
	.reg_adsp2spm_wakeup_mask_b = 0,
	.reg_sspm2spm_wakeup_mask_b = 0,
	.reg_scp2spm_wakeup_mask_b = 0,
	.reg_csyspwrup_ack_mask = 1,
	.reg_spm_reserved_srcclkena_mask_b = 0,
	.reg_spm_reserved_infra_req_mask_b = 0,
	.reg_spm_reserved_apsrc_req_mask_b = 0,
	.reg_spm_reserved_vrf18_req_mask_b = 0,
	.reg_spm_reserved_ddren_req_mask_b = 0,
	.reg_mcupm_srcclkena_mask_b = 1,
	.reg_mcupm_infra_req_mask_b = 1,
	.reg_mcupm_apsrc_req_mask_b = 1,
	.reg_mcupm_vrf18_req_mask_b = 1,
	.reg_mcupm_ddren_req_mask_b = 1,
	.reg_msdc0_srcclkena_mask_b = 1,
	.reg_msdc0_infra_req_mask_b = 1,
	.reg_msdc0_apsrc_req_mask_b = 1,
	.reg_msdc0_vrf18_req_mask_b = 1,
	.reg_msdc0_ddren_req_mask_b = 1,
	.reg_msdc1_srcclkena_mask_b = 1,
	.reg_msdc1_infra_req_mask_b = 1,
	.reg_msdc1_apsrc_req_mask_b = 1,
	.reg_msdc1_vrf18_req_mask_b = 1,
	.reg_msdc1_ddren_req_mask_b = 1,

	/* SPM_SRC4_MASK */
	.reg_ccif_event_srcclkena_mask_b = 0x3FF,
	.reg_bak_psri_srcclkena_mask_b = 0,
	.reg_bak_psri_infra_req_mask_b = 0,
	.reg_bak_psri_apsrc_req_mask_b = 0,
	.reg_bak_psri_vrf18_req_mask_b = 0,
	.reg_bak_psri_ddren_req_mask_b = 0,
	.reg_dramc_md32_infra_req_mask_b = 1,
	.reg_dramc_md32_vrf18_req_mask_b = 0,
	.reg_conn_srcclkenb2pwrap_mask_b = 0,
	.reg_dramc_md32_apsrc_req_mask_b = 0,

	/* SPM_SRC5_MASK */
	.reg_mcusys_merge_apsrc_req_mask_b = 0x14,
	.reg_mcusys_merge_ddren_req_mask_b = 0x14,
	.reg_afe_srcclkena_mask_b = 0,
	.reg_afe_infra_req_mask_b = 0,
	.reg_afe_apsrc_req_mask_b = 0,
	.reg_afe_vrf18_req_mask_b = 0,
	.reg_afe_ddren_req_mask_b = 0,
	.reg_msdc2_srcclkena_mask_b = 0,
	.reg_msdc2_infra_req_mask_b = 0,
	.reg_msdc2_apsrc_req_mask_b = 0,
	.reg_msdc2_vrf18_req_mask_b = 0,
	.reg_msdc2_ddren_req_mask_b = 0,

	/* SPM_WAKEUP_EVENT_MASK */
	.reg_wakeup_event_mask = 0xEFFFFFFF,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.reg_ext_wakeup_event_mask = 0xFFFFFFFF,

	/* SPM_SRC7_MASK */
	.reg_pcie_srcclkena_mask_b = 0,
	.reg_pcie_infra_req_mask_b = 0,
	.reg_pcie_apsrc_req_mask_b = 0,
	.reg_pcie_vrf18_req_mask_b = 0,
	.reg_pcie_ddren_req_mask_b = 0,
	.reg_dpmaif_srcclkena_mask_b = 1,
	.reg_dpmaif_infra_req_mask_b = 1,
	.reg_dpmaif_apsrc_req_mask_b = 1,
	.reg_dpmaif_vrf18_req_mask_b = 1,
	.reg_dpmaif_ddren_req_mask_b = 1,

	/* Auto-gen End */
};

void spm_set_power_control(const struct pwr_ctrl *pwrctrl)
{
	/* Auto-gen Start */

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

	/* SPM_SRC6_MASK */
	write32(&mtk_spm->spm_src6_mask,
		((pwrctrl->reg_ccif_event_infra_req_mask_b & 0xffff) << 0) |
		((pwrctrl->reg_ccif_event_apsrc_req_mask_b & 0xffff) << 16));

	/* SPM_SRC_REQ */
	write32(&mtk_spm->spm_src_req,
		((pwrctrl->reg_spm_apsrc_req & 0x1) << 0) |
		((pwrctrl->reg_spm_f26m_req & 0x1) << 1) |
		((pwrctrl->reg_spm_infra_req & 0x1) << 3) |
		((pwrctrl->reg_spm_vrf18_req & 0x1) << 4) |
		((pwrctrl->reg_spm_ddren_req & 0x1) << 7) |
		((pwrctrl->reg_spm_dvfs_req & 0x1) << 8) |
		((pwrctrl->reg_spm_sw_mailbox_req & 0x1) << 9) |
		((pwrctrl->reg_spm_sspm_mailbox_req & 0x1) << 10) |
		((pwrctrl->reg_spm_adsp_mailbox_req & 0x1) << 11) |
		((pwrctrl->reg_spm_scp_mailbox_req & 0x1) << 12));

	/* SPM_SRC_MASK */
	write32(&mtk_spm->spm_src_mask,
		((pwrctrl->reg_md_0_srcclkena_mask_b & 0x1) << 0) |
		((pwrctrl->reg_md_0_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_md_0_apsrc_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_md_0_vrf18_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_md_0_ddren_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_md_1_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_md_1_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_md_1_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_md_1_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_md_1_ddren_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_conn_srcclkena_mask_b & 0x1) << 10) |
		((pwrctrl->reg_conn_srcclkenb_mask_b & 0x1) << 11) |
		((pwrctrl->reg_conn_infra_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_conn_apsrc_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_conn_vrf18_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_conn_ddren_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_conn_vfe28_mask_b & 0x1) << 16) |
		((pwrctrl->reg_srcclkeni_srcclkena_mask_b & 0x7) << 17) |
		((pwrctrl->reg_srcclkeni_infra_req_mask_b & 0x7) << 20) |
		((pwrctrl->reg_infrasys_apsrc_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_infrasys_ddren_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_sspm_srcclkena_mask_b & 0x1) << 27) |
		((pwrctrl->reg_sspm_infra_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_sspm_apsrc_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_sspm_vrf18_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_sspm_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC2_MASK */
	write32(&mtk_spm->spm_src2_mask,
		((pwrctrl->reg_scp_srcclkena_mask_b & 0x1) << 0) |
		((pwrctrl->reg_scp_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_scp_apsrc_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_scp_vrf18_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_scp_ddren_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_audio_dsp_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_audio_dsp_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_audio_dsp_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_audio_dsp_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_audio_dsp_ddren_req_mask_b & 0x1) << 9) |
		((pwrctrl->reg_ufs_srcclkena_mask_b & 0x1) << 10) |
		((pwrctrl->reg_ufs_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->reg_ufs_apsrc_req_mask_b & 0x1) << 12) |
		((pwrctrl->reg_ufs_vrf18_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_ufs_ddren_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_disp0_apsrc_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_disp0_ddren_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_disp1_apsrc_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_disp1_ddren_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_gce_infra_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_gce_apsrc_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_gce_vrf18_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_gce_ddren_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_apu_srcclkena_mask_b & 0x1) << 23) |
		((pwrctrl->reg_apu_infra_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_apu_apsrc_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_apu_vrf18_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_apu_ddren_req_mask_b & 0x1) << 27) |
		((pwrctrl->reg_cg_check_srcclkena_mask_b & 0x1) << 28) |
		((pwrctrl->reg_cg_check_apsrc_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_cg_check_vrf18_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_cg_check_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC3_MASK */
	write32(&mtk_spm->spm_src3_mask,
		((pwrctrl->reg_dvfsrc_event_trigger_mask_b & 0x1) << 0) |
		((pwrctrl->reg_sw2spm_wakeup_mask_b & 0xf) << 1) |
		((pwrctrl->reg_adsp2spm_wakeup_mask_b & 0x1) << 5) |
		((pwrctrl->reg_sspm2spm_wakeup_mask_b & 0xf) << 6) |
		((pwrctrl->reg_scp2spm_wakeup_mask_b & 0x1) << 10) |
		((pwrctrl->reg_csyspwrup_ack_mask & 0x1) << 11) |
		((pwrctrl->reg_spm_reserved_srcclkena_mask_b & 0x1) << 12) |
		((pwrctrl->reg_spm_reserved_infra_req_mask_b & 0x1) << 13) |
		((pwrctrl->reg_spm_reserved_apsrc_req_mask_b & 0x1) << 14) |
		((pwrctrl->reg_spm_reserved_vrf18_req_mask_b & 0x1) << 15) |
		((pwrctrl->reg_spm_reserved_ddren_req_mask_b & 0x1) << 16) |
		((pwrctrl->reg_mcupm_srcclkena_mask_b & 0x1) << 17) |
		((pwrctrl->reg_mcupm_infra_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_mcupm_apsrc_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_mcupm_vrf18_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_mcupm_ddren_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_msdc0_srcclkena_mask_b & 0x1) << 22) |
		((pwrctrl->reg_msdc0_infra_req_mask_b & 0x1) << 23) |
		((pwrctrl->reg_msdc0_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_msdc0_vrf18_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_msdc0_ddren_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_msdc1_srcclkena_mask_b & 0x1) << 27) |
		((pwrctrl->reg_msdc1_infra_req_mask_b & 0x1) << 28) |
		((pwrctrl->reg_msdc1_apsrc_req_mask_b & 0x1) << 29) |
		((pwrctrl->reg_msdc1_vrf18_req_mask_b & 0x1) << 30) |
		((pwrctrl->reg_msdc1_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC4_MASK */
	write32(&mtk_spm->spm_src4_mask,
		((pwrctrl->reg_ccif_event_srcclkena_mask_b & 0xffff) << 0) |
		((pwrctrl->reg_bak_psri_srcclkena_mask_b & 0x1) << 16) |
		((pwrctrl->reg_bak_psri_infra_req_mask_b & 0x1) << 17) |
		((pwrctrl->reg_bak_psri_apsrc_req_mask_b & 0x1) << 18) |
		((pwrctrl->reg_bak_psri_vrf18_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_bak_psri_ddren_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_dramc_md32_infra_req_mask_b & 0x3) << 21) |
		((pwrctrl->reg_dramc_md32_vrf18_req_mask_b & 0x3) << 23) |
		((pwrctrl->reg_conn_srcclkenb2pwrap_mask_b & 0x1) << 25) |
		((pwrctrl->reg_dramc_md32_apsrc_req_mask_b & 0x3) << 26));

	/* SPM_SRC5_MASK */
	write32(&mtk_spm->spm_src5_mask,
		((pwrctrl->reg_mcusys_merge_apsrc_req_mask_b & 0x1ff) << 0) |
		((pwrctrl->reg_mcusys_merge_ddren_req_mask_b & 0x1ff) << 9) |
		((pwrctrl->reg_afe_srcclkena_mask_b & 0x1) << 18) |
		((pwrctrl->reg_afe_infra_req_mask_b & 0x1) << 19) |
		((pwrctrl->reg_afe_apsrc_req_mask_b & 0x1) << 20) |
		((pwrctrl->reg_afe_vrf18_req_mask_b & 0x1) << 21) |
		((pwrctrl->reg_afe_ddren_req_mask_b & 0x1) << 22) |
		((pwrctrl->reg_msdc2_srcclkena_mask_b & 0x1) << 23) |
		((pwrctrl->reg_msdc2_infra_req_mask_b & 0x1) << 24) |
		((pwrctrl->reg_msdc2_apsrc_req_mask_b & 0x1) << 25) |
		((pwrctrl->reg_msdc2_vrf18_req_mask_b & 0x1) << 26) |
		((pwrctrl->reg_msdc2_ddren_req_mask_b & 0x1) << 27));

	/* SPM_WAKEUP_EVENT_MASK */
	write32(&mtk_spm->spm_wakeup_event_mask,
		((pwrctrl->reg_wakeup_event_mask & 0xffffffff) << 0));

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	write32(&mtk_spm->spm_wakeup_event_ext_mask,
		((pwrctrl->reg_ext_wakeup_event_mask & 0xffffffff) << 0));

	/* SPM_SRC7_MASK */
	write32(&mtk_spm->spm_src7_mask,
		((pwrctrl->reg_pcie_srcclkena_mask_b & 0x1) << 0) |
		((pwrctrl->reg_pcie_infra_req_mask_b & 0x1) << 1) |
		((pwrctrl->reg_pcie_apsrc_req_mask_b & 0x1) << 2) |
		((pwrctrl->reg_pcie_vrf18_req_mask_b & 0x1) << 3) |
		((pwrctrl->reg_pcie_ddren_req_mask_b & 0x1) << 4) |
		((pwrctrl->reg_dpmaif_srcclkena_mask_b & 0x1) << 5) |
		((pwrctrl->reg_dpmaif_infra_req_mask_b & 0x1) << 6) |
		((pwrctrl->reg_dpmaif_apsrc_req_mask_b & 0x1) << 7) |
		((pwrctrl->reg_dpmaif_vrf18_req_mask_b & 0x1) << 8) |
		((pwrctrl->reg_dpmaif_ddren_req_mask_b & 0x1) << 9));
	/* Auto-gen End */

	/* Disable unused optional components */
	write32(&mtk_spm->nna_pwr_con, BIT(1) | BIT(4) | BIT(8));
}

static void spm_hw_s1_state_monitor(int en)
{
	if (en)
		SET32_BITFIELDS(&mtk_spm->spm_ack_chk_con_3,
				SPM_ACK_CHK_3_CON_CLR_ALL, 0,
				SPM_ACK_CHK_3_CON_EN_0, 1,
				SPM_ACK_CHK_3_CON_EN_1, 1);
	else
		SET32_BITFIELDS(&mtk_spm->spm_ack_chk_con_3,
				SPM_ACK_CHK_3_CON_HW_MODE_TRIG_0, 1,
				SPM_ACK_CHK_3_CON_HW_MODE_TRIG_1, 1,
				SPM_ACK_CHK_3_CON_CLR_ALL, 1,
				SPM_ACK_CHK_3_CON_EN_0, 0,
				SPM_ACK_CHK_3_CON_EN_1, 0);
}

void spm_register_init(void)
{
	/* Enable register control */
	write32(&mtk_spm->poweron_config_set,
		SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Init power control register */
	write32(&mtk_spm->spm_power_on_val1, POWER_ON_VAL1_DEF);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* Reset PCM */
	write32(&mtk_spm->pcm_con0,
		SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
	write32(&mtk_spm->pcm_con1,
		SPM_REGWR_CFG_KEY | RG_AHBMIF_APBEN_LSB |
		REG_MD32_APB_INTERNAL_EN_LSB);

	/* Initial SPM CLK control register */
	SET32_BITFIELDS(&mtk_spm->spm_clk_con,
			REG_SYSCLK1_SRC_MD2_SRCCLKENA, 1);

	/* Clean wakeup event raw status */
	write32(&mtk_spm->spm_wakeup_event_mask, SPM_WAKEUP_EVENT_MASK_DEF);

	/* Clean ISR status */
	write32(&mtk_spm->spm_irq_mask, ISRM_ALL);
	write32(&mtk_spm->spm_irq_sta, ISRC_ALL);
	write32(&mtk_spm->spm_swint_clr, PCM_SW_INT_ALL);

	/* Init r7 with POWER_ON_VAL1 */
	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val1));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R7);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* DDR EN de-bounce length to 5us */
	write32(&mtk_spm->ddren_dbc_con, DDREN_DBC_EN_VAL | REG_DDREN_DBC_EN_LSB);

	/* Configure ARMPLL Control Mode for MCDI */
	write32(&mtk_spm->armpll_clk_sel, ARMPLL_CLK_SEL_DEF);

	/* Init for SPM Resource ACK */
	write32(&mtk_spm->spm_resource_ack_con0, SPM_RESOURCE_ACK_CON0_DEF);
	write32(&mtk_spm->spm_resource_ack_con1, SPM_RESOURCE_ACK_CON1_DEF);
	write32(&mtk_spm->spm_resource_ack_con2, SPM_RESOURCE_ACK_CON2_DEF);
	write32(&mtk_spm->spm_resource_ack_con3, SPM_RESOURCE_ACK_CON3_DEF);

	/* Enable Side-Band */
	write32((void *)AP_PLL_CON3, APMIX_CON3_DEF);
	write32((void *)AP_PLL_CON4, APMIX_CON4_DEF);
	write32((void *)CLK_SCP_CFG_0, SCP_CFG0_DEF);
	write32((void *)CLK_SCP_CFG_1, SCP_CFG1_DEF);

	/* Init VCORE DVFS Status */
	SET32_BITFIELDS(&mtk_spm->spm_dvfs_misc,
			SPM_DVFS_FORCE_ENABLE_LSB, 0,
			SPM_DVFSRC_ENABLE_LSB, 1);
	write32(&mtk_spm->spm_dvfs_level, SPM_DVFS_LEVEL_DEF);
	write32(&mtk_spm->spm_dvs_dfs_level, SPM_DVS_DFS_LEVEL_DEF);

	write32(&mtk_spm->spm_ack_chk_sel_3, SPM_ACK_CHK_3_SEL_HW_S1);
	write32(&mtk_spm->spm_ack_chk_timer_3, SPM_ACK_CHK_3_HW_S1_CNT);

	spm_hw_s1_state_monitor(0);
}

void spm_extern_initialize(void)
{
	SET32_BITFIELDS(&mtk_spm->spm_dvfs_misc,
			INFRA_AO_RES_CTRL_MASK_EMI_IDLE, 1,
			INFRA_AO_RES_CTRL_MASK_MPU_IDLE, 1);
}

void spm_reset_and_init_pcm(void)
{
	bool first_load_fw = true;

	/* Check whether the SPM FW is running */
	if (read32(&mtk_spm->md32pcm_cfgreg_sw_rstn) & MD32PCM_CFGREG_SW_RSTN_RUN)
		first_load_fw = false;

	if (!first_load_fw) {
		spm_code_swapping();
		/* Backup PCM r0 -> SPM_POWER_ON_VAL0 before reset PCM */
		write32(&mtk_spm->spm_power_on_val0,
			read32(&mtk_spm->pcm_reg0_data));
	}

	/* Disable r0 and r7 to control power */
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* Disable pcm timer after leaving FW */
	clrsetbits32(&mtk_spm->pcm_con1,
		     RG_PCM_TIMER_EN_LSB, SPM_REGWR_CFG_KEY);

	/* Reset PCM */
	write32(&mtk_spm->pcm_con0,
		SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* Init PCM_CON1 (disable PCM timer but keep PCM WDT setting) */
	clrsetbits32(&mtk_spm->pcm_con1, ~RG_PCM_WDT_WAKE_LSB,
		     SPM_REGWR_CFG_KEY | RG_AHBMIF_APBEN_LSB | REG_MD32_APB_INTERNAL_EN_LSB);
}

void spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl)
{
	u32 val, mask;

	/* Toggle event counter clear */
	setbits32(&mtk_spm->pcm_con1,
		  SPM_REGWR_CFG_KEY | REG_SPM_EVENT_COUNTER_CLR_LSB);

	/* Toggle for reset SYS TIMER start point */
	SET32_BITFIELDS(&mtk_spm->sys_timer_con,
			SYS_TIMER_START_EN_LSB, 1);

	if (pwrctrl->timer_val_cust == 0)
		val = pwrctrl->timer_val ? pwrctrl->timer_val : PCM_TIMER_MAX;
	else
		val = pwrctrl->timer_val_cust;

	write32(&mtk_spm->pcm_timer_val, val);

	setbits32(&mtk_spm->pcm_con1, RG_PCM_TIMER_EN_LSB | SPM_REGWR_CFG_KEY);

	/* Unmask AP wakeup source */
	if (pwrctrl->wake_src_cust == 0)
		mask = pwrctrl->wake_src;
	else
		mask = pwrctrl->wake_src_cust;

	if (pwrctrl->reg_csyspwrup_ack_mask)
		mask &= ~R12_CSYSPWREQ_B;
	write32(&mtk_spm->spm_wakeup_event_mask, ~mask);

	/* Unmask SPM ISR */
	setbits32(&mtk_spm->spm_irq_mask, ISRM_RET_IRQ_AUX);

	/* Toggle event counter clear */
	clrsetbits32(&mtk_spm->pcm_con1, REG_SPM_EVENT_COUNTER_CLR_LSB, SPM_REGWR_CFG_KEY);

	/* Toggle for reset SYS TIMER start point */
	SET32_BITFIELDS(&mtk_spm->sys_timer_con,
			SYS_TIMER_START_EN_LSB, 0);
}

const struct pwr_ctrl *get_pwr_ctrl(void)
{
	return &spm_init_ctrl;
}
