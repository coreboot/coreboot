/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_SPM_H
#define SOC_MEDIATEK_MT8196_SPM_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <soc/spm_common.h>
#include <types.h>

#define INFRA_SW_CG	(INFRACFG_AO_BASE + 0x60)

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE			0xb16
#define SPM_REGWR_CFG_KEY			(SPM_PROJECT_CODE << 16)

/* POWERON_CONFIG_EN (0x10006000+0x000) */
#define BCLK_CG_EN_LSB				BIT(0)

#define SPM_ACK_CHK_3_SEL_HW_S1			0x00350098
#define SPM_ACK_CHK_3_HW_S1_CNT			1

/* PCM_CON0 (0x10006000+0x018) */
#define PCM_CK_EN_LSB				BIT(2)
#define PCM_SW_RESET_LSB			BIT(15)

/* PCM_CON1 (0x1C001000+0x01C) */
#define REG_SPM_APB_INTERNAL_EN_LSB		BIT(3)
#define REG_PCM_TIMER_EN_LSB			BIT(5)
#define REG_PCM_WDT_EN_LSB			BIT(8)
#define REG_PCM_WDT_WAKE_LSB			BIT(9)
#define REG_SSPM_APB_P2P_EN_LSB			BIT(10)
#define REG_MCUPM_APB_P2P_EN_LSB		BIT(11)
#define REG_RSV_APB_P2P_EN_LSB			BIT(12)
#define RG_PCM_IRQ_MSK_LSB			BIT(15)
#define PCM_CON1_PROJECT_CODE_LSB		BIT(16)

/* DDREN_DBC_CON (0x1C001000+0x888) */
#define REG_DDREN_DBC_EN_LSB			BIT(16)

/* SPM_WAKEUP_EVENT_MASK (0x10006000+0x0D0) */
#define SPM_WAKEUP_EVENT_MASK_BIT0		(1U << 0)
#define SPM_WAKEUP_EVENT_MASK_CSYSPWREQ_B	(1U << 11)

/* SPM_DVFS_MISC (0x10006000+0x4AC) */
DEFINE_BIT(SPM_DVFS_FORCE_ENABLE_LSB, 2)
DEFINE_BIT(SPM_DVFSRC_ENABLE_LSB, 4)

/* --- SPM Flag Define --- */
#define SPM_FLAG_DISABLE_VCORE_DVS		BIT(8)
#define SPM_FLAG_DISABLE_DDR_DFS		BIT(9)
#define SPM_FLAG_DISABLE_EMI_DFS		BIT(10)
#define SPM_FLAG_DISABLE_BUS_DFS		BIT(11)
#define SPM_FLAG_RUN_COMMON_SCENARIO		BIT(19)

/* SPM_EVENT_COUNTER_CLEAR (0x1C001000+0x8B8) */
#define REG_SPM_EVENT_COUNTER_CLR_LSB		BIT(0)

/* SYS_TIMER_CON (0x10006000+0x98C) */
#define SYS_TIMER_START_EN_LSB			BIT(0)

/* MD32PCM_CFGREG_SW_RSTN (0x10006000+0xA00) */
DEFINE_BIT(MD32PCM_CFGREG_SW_RSTN_RESET, 0)

#define SPM_SYSTEM_BASE_OFFSET		0x0
#define POWER_ON_VAL1_DEF		0x003ffe20
#define SPM_WAKEUP_EVENT_MASK_DEF	0xefffffff
#define SPM_BUS_PROTECT_MASK_B_DEF	0xffffffff
#define SPM_BUS_PROTECT2_MASK_B_DEF	0xffffffff
#define MD32PCM_DMA0_CON_VAL		0x0003820e
#define MD32PCM_DMA0_START_VAL		0x00008000
#define MD32PCM_CFGREG_SW_RSTN_RUN	0x1
#define SPM_DVFS_LEVEL_DEF		0x00000001
#define SPM_DVS_DFS_LEVEL_DEF		0x00010001
#define SPM_RESOURCE_ACK_CON0_DEF	0xffffffff
#define SPM_RESOURCE_ACK_CON1_DEF	0xffffffff
#define SPM_RESOURCE_ACK_CON2_DEF	0xffffffff
#define SPM_RESOURCE_ACK_CON3_DEF	0xffffffff
#define ARMPLL_CLK_SEL_DEF		0x3ff
#define SPM_SYSCLK_SETTLE		0x60fe
#define SPM_INIT_DONE_US		20
#define PCM_WDT_TIMEOUT_S		(30 * 32768)
#define PCM_TIMER_MAX			(0xffffffff - PCM_WDT_TIMEOUT_S)
#define AP_WDT_TIMEOUT_SUSPEND_S	5400
#define PCM_TIMER_SUSPEND		((AP_WDT_TIMEOUT_SUSPEND_S - 30) * 32768)
#define SPM_FLAG_ENABLE_MT8196_ES_WA		BIT(24)
#define SPM_FLAG_ENABLE_MT8196_EMI_ES_WA	BIT(25)
#define MT_SPM_VERSION_ES			0x0
#define MT_SPM_VERSION_CS			0x1

/* SPM_IRQ_MASK */
#define ISRM_TWAM		BIT(2)
#define ISRM_PCM_RETURN		BIT(3)
#define ISRM_RET_IRQ0		BIT(8)
#define ISRM_RET_IRQ1		BIT(9)
#define ISRM_RET_IRQ2		BIT(10)
#define ISRM_RET_IRQ3		BIT(11)
#define ISRM_RET_IRQ4		BIT(12)
#define ISRM_RET_IRQ5		BIT(13)
#define ISRM_RET_IRQ6		BIT(14)
#define ISRM_RET_IRQ7		BIT(15)
#define ISRM_RET_IRQ8		BIT(16)
#define ISRM_RET_IRQ9		BIT(17)
#define ISRM_RET_IRQ_AUX	((ISRM_RET_IRQ9) | (ISRM_RET_IRQ8) | \
				 (ISRM_RET_IRQ7) | (ISRM_RET_IRQ6) | \
				 (ISRM_RET_IRQ5) | (ISRM_RET_IRQ4) | \
				 (ISRM_RET_IRQ3) | (ISRM_RET_IRQ2) | \
				 (ISRM_RET_IRQ1))
#define ISRM_ALL_EXC_TWAM	(ISRM_RET_IRQ_AUX)
#define ISRM_ALL		(ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM		BIT(2)
#define ISRS_PCM_RETURN		BIT(3)
#define ISRC_TWAM		ISRS_TWAM
#define ISRC_ALL_EXC_TWAM	ISRS_PCM_RETURN
#define ISRC_ALL		(ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* SPM_SWINT */
#define PCM_SW_INT_ALL			0x3ff

/* MD32PCM_STA1 define */
#define R12_CSYSPWREQ_B			BIT(24)

#define SPM_INTERNAL_STATUS_HW_S1	BIT(0)
#define SPM_ACK_CHK_3_CON_HW_MODE_TRIG	0x800
#define SPM_ACK_CHK_3_CON_EN		0x110
#define SPM_ACK_CHK_3_CON_CLR_ALL	0x2
#define SPM_ACK_CHK_3_CON_RESULT	0x8000

struct pwr_ctrl {
	/* for SPM */
	u32 pcm_flags;
	u32 pcm_flags_cust;
	u32 pcm_flags_cust_set;
	u32 pcm_flags_cust_clr;
	u32 pcm_flags1;
	u32 pcm_flags1_cust;
	u32 pcm_flags1_cust_set;
	u32 pcm_flags1_cust_clr;
	u32 timer_val;
	u32 timer_val_cust;
	u32 timer_val_ramp_en;
	u32 timer_val_ramp_en_sec;
	u32 wake_src;
	u32 wake_src_cust;
	u32 wakelock_timer_val;
	u8 wdt_disable;
	/* Auto-gen Start */

	/* SPM_CLK_CON */
	u8 reg_spm_lock_infra_dcm_lsb;
	u8 reg_cxo32k_remove_en_lsb;
	u8 reg_spm_leave_suspend_merge_mask_lsb;
	u8 reg_sysclk0_src_mask_b_lsb;
	u8 reg_sysclk1_src_mask_b_lsb;
	u8 reg_sysclk2_src_mask_b_lsb;

	/* SPM_AP_STANDBY_CON */
	u8 reg_wfi_op;
	u8 reg_wfi_type;
	u8 reg_mp0_cputop_idle_mask;
	u8 reg_mp1_cputop_idle_mask;
	u8 reg_mcusys_idle_mask;
	u8 reg_csyspwrup_req_mask_lsb;
	u8 reg_wfi_af_sel;
	u8 reg_cpu_sleep_wfi;

	/* SPM_SRC_REQ */
	u8 reg_spm_adsp_mailbox_req;
	u8 reg_spm_apsrc_req;
	u8 reg_spm_ddren_req;
	u8 reg_spm_dvfs_req;
	u8 reg_spm_emi_req;
	u8 reg_spm_f26m_req;
	u8 reg_spm_infra_req;
	u8 reg_spm_pmic_req;
	u8 reg_spm_scp_mailbox_req;
	u8 reg_spm_sspm_mailbox_req;
	u8 reg_spm_sw_mailbox_req;
	u8 reg_spm_vcore_req;
	u8 reg_spm_vrf18_req;
	u8 adsp_mailbox_state;
	u8 apsrc_state;
	u8 ddren_state;
	u8 dvfs_state;
	u8 emi_state;
	u8 f26m_state;
	u8 infra_state;
	u8 pmic_state;
	u8 scp_mailbox_state;
	u8 sspm_mailbox_state;
	u8 sw_mailbox_state;
	u8 vcore_state;
	u8 vrf18_state;

	/* SPM_SRC_MASK_0 */
	u8 reg_apifr_mem_apsrc_req_mask_b;
	u8 reg_apifr_mem_ddren_req_mask_b;
	u8 reg_apifr_mem_emi_req_mask_b;
	u8 reg_apifr_mem_infra_req_mask_b;
	u8 reg_apifr_mem_pmic_req_mask_b;
	u8 reg_apifr_mem_srcclkena_mask_b;
	u8 reg_apifr_mem_vcore_req_mask_b;
	u8 reg_apifr_mem_vrf18_req_mask_b;
	u8 reg_apu_apsrc_req_mask_b;
	u8 reg_apu_ddren_req_mask_b;
	u8 reg_apu_emi_req_mask_b;
	u8 reg_apu_infra_req_mask_b;
	u8 reg_apu_pmic_req_mask_b;
	u8 reg_apu_srcclkena_mask_b;
	u8 reg_apu_vcore_req_mask_b;
	u8 reg_apu_vrf18_req_mask_b;
	u8 reg_audio_apsrc_req_mask_b;
	u8 reg_audio_ddren_req_mask_b;
	u8 reg_audio_emi_req_mask_b;
	u8 reg_audio_infra_req_mask_b;
	u8 reg_audio_pmic_req_mask_b;
	u8 reg_audio_srcclkena_mask_b;
	u8 reg_audio_vcore_req_mask_b;
	u8 reg_audio_vrf18_req_mask_b;

	/* SPM_SRC_MASK_1 */
	u8 reg_audio_dsp_apsrc_req_mask_b;
	u8 reg_audio_dsp_ddren_req_mask_b;
	u8 reg_audio_dsp_emi_req_mask_b;
	u8 reg_audio_dsp_infra_req_mask_b;
	u8 reg_audio_dsp_pmic_req_mask_b;
	u8 reg_audio_dsp_srcclkena_mask_b;
	u8 reg_audio_dsp_vcore_req_mask_b;
	u8 reg_audio_dsp_vrf18_req_mask_b;
	u8 reg_cam_apsrc_req_mask_b;
	u8 reg_cam_ddren_req_mask_b;
	u8 reg_cam_emi_req_mask_b;
	u8 reg_cam_infra_req_mask_b;
	u8 reg_cam_pmic_req_mask_b;
	u8 reg_cam_srcclkena_mask_b;
	u8 reg_cam_vrf18_req_mask_b;
	u32 reg_ccif_apsrc_req_mask_b;

	/* SPM_SRC_MASK_2 */
	u32 reg_ccif_emi_req_mask_b;
	u32 reg_ccif_infra_req_mask_b;

	/* SPM_SRC_MASK_3 */
	u32 reg_ccif_pmic_req_mask_b;
	u32 reg_ccif_srcclkena_mask_b;

	/* SPM_SRC_MASK_4 */
	u32 reg_ccif_vcore_req_mask_b;
	u32 reg_ccif_vrf18_req_mask_b;
	u8 reg_ccu_apsrc_req_mask_b;
	u8 reg_ccu_ddren_req_mask_b;
	u8 reg_ccu_emi_req_mask_b;
	u8 reg_ccu_infra_req_mask_b;
	u8 reg_ccu_pmic_req_mask_b;
	u8 reg_ccu_srcclkena_mask_b;
	u8 reg_ccu_vrf18_req_mask_b;
	u8 reg_cg_check_apsrc_req_mask_b;

	/* SPM_SRC_MASK_5 */
	u8 reg_cg_check_ddren_req_mask_b;
	u8 reg_cg_check_emi_req_mask_b;
	u8 reg_cg_check_infra_req_mask_b;
	u8 reg_cg_check_pmic_req_mask_b;
	u8 reg_cg_check_srcclkena_mask_b;
	u8 reg_cg_check_vcore_req_mask_b;
	u8 reg_cg_check_vrf18_req_mask_b;
	u8 reg_cksys_apsrc_req_mask_b;
	u8 reg_cksys_ddren_req_mask_b;
	u8 reg_cksys_emi_req_mask_b;
	u8 reg_cksys_infra_req_mask_b;
	u8 reg_cksys_pmic_req_mask_b;
	u8 reg_cksys_srcclkena_mask_b;
	u8 reg_cksys_vcore_req_mask_b;
	u8 reg_cksys_vrf18_req_mask_b;
	u8 reg_cksys_1_apsrc_req_mask_b;
	u8 reg_cksys_1_ddren_req_mask_b;
	u8 reg_cksys_1_emi_req_mask_b;
	u8 reg_cksys_1_infra_req_mask_b;
	u8 reg_cksys_1_pmic_req_mask_b;
	u8 reg_cksys_1_srcclkena_mask_b;
	u8 reg_cksys_1_vcore_req_mask_b;
	u8 reg_cksys_1_vrf18_req_mask_b;

	/* SPM_SRC_MASK_6 */
	u8 reg_cksys_2_apsrc_req_mask_b;
	u8 reg_cksys_2_ddren_req_mask_b;
	u8 reg_cksys_2_emi_req_mask_b;
	u8 reg_cksys_2_infra_req_mask_b;
	u8 reg_cksys_2_pmic_req_mask_b;
	u8 reg_cksys_2_srcclkena_mask_b;
	u8 reg_cksys_2_vcore_req_mask_b;
	u8 reg_cksys_2_vrf18_req_mask_b;
	u8 reg_conn_apsrc_req_mask_b;
	u8 reg_conn_ddren_req_mask_b;
	u8 reg_conn_emi_req_mask_b;
	u8 reg_conn_infra_req_mask_b;
	u8 reg_conn_pmic_req_mask_b;
	u8 reg_conn_srcclkena_mask_b;
	u8 reg_conn_srcclkenb_mask_b;
	u8 reg_conn_vcore_req_mask_b;
	u8 reg_conn_vrf18_req_mask_b;
	u8 reg_corecfg_rsv0_apsrc_req_mask_b;
	u8 reg_corecfg_rsv0_ddren_req_mask_b;
	u8 reg_corecfg_rsv0_emi_req_mask_b;
	u8 reg_corecfg_rsv0_infra_req_mask_b;
	u8 reg_corecfg_rsv0_pmic_req_mask_b;
	u8 reg_corecfg_rsv0_srcclkena_mask_b;
	u8 reg_corecfg_rsv0_vcore_req_mask_b;
	u8 reg_corecfg_rsv0_vrf18_req_mask_b;

	/* SPM_SRC_MASK_7 */
	u8 reg_cpueb_apsrc_req_mask_b;
	u8 reg_cpueb_ddren_req_mask_b;
	u8 reg_cpueb_emi_req_mask_b;
	u8 reg_cpueb_infra_req_mask_b;
	u8 reg_cpueb_pmic_req_mask_b;
	u8 reg_cpueb_srcclkena_mask_b;
	u8 reg_cpueb_vcore_req_mask_b;
	u8 reg_cpueb_vrf18_req_mask_b;
	u8 reg_disp0_apsrc_req_mask_b;
	u8 reg_disp0_ddren_req_mask_b;
	u8 reg_disp0_emi_req_mask_b;
	u8 reg_disp0_infra_req_mask_b;
	u8 reg_disp0_pmic_req_mask_b;
	u8 reg_disp0_srcclkena_mask_b;
	u8 reg_disp0_vrf18_req_mask_b;
	u8 reg_disp1_apsrc_req_mask_b;
	u8 reg_disp1_ddren_req_mask_b;
	u8 reg_disp1_emi_req_mask_b;
	u8 reg_disp1_infra_req_mask_b;
	u8 reg_disp1_pmic_req_mask_b;
	u8 reg_disp1_srcclkena_mask_b;
	u8 reg_disp1_vrf18_req_mask_b;
	u8 reg_dpm_apsrc_req_mask_b;
	u8 reg_dpm_ddren_req_mask_b;

	/* SPM_SRC_MASK_8 */
	u8 reg_dpm_emi_req_mask_b;
	u8 reg_dpm_infra_req_mask_b;
	u8 reg_dpm_pmic_req_mask_b;
	u8 reg_dpm_srcclkena_mask_b;
	u8 reg_dpm_vcore_req_mask_b;
	u8 reg_dpm_vrf18_req_mask_b;
	u8 reg_dpmaif_apsrc_req_mask_b;
	u8 reg_dpmaif_ddren_req_mask_b;
	u8 reg_dpmaif_emi_req_mask_b;
	u8 reg_dpmaif_infra_req_mask_b;
	u8 reg_dpmaif_pmic_req_mask_b;
	u8 reg_dpmaif_srcclkena_mask_b;
	u8 reg_dpmaif_vcore_req_mask_b;
	u8 reg_dpmaif_vrf18_req_mask_b;

	/* SPM_SRC_MASK_9 */
	u8 reg_dvfsrc_level_req_mask_b;
	u8 reg_emisys_apsrc_req_mask_b;
	u8 reg_emisys_ddren_req_mask_b;
	u8 reg_emisys_emi_req_mask_b;
	u8 reg_emisys_infra_req_mask_b;
	u8 reg_emisys_pmic_req_mask_b;
	u8 reg_emisys_srcclkena_mask_b;
	u8 reg_emisys_vcore_req_mask_b;
	u8 reg_emisys_vrf18_req_mask_b;
	u8 reg_gce_apsrc_req_mask_b;
	u8 reg_gce_ddren_req_mask_b;
	u8 reg_gce_emi_req_mask_b;
	u8 reg_gce_infra_req_mask_b;
	u8 reg_gce_pmic_req_mask_b;
	u8 reg_gce_srcclkena_mask_b;
	u8 reg_gce_vcore_req_mask_b;
	u8 reg_gce_vrf18_req_mask_b;
	u8 reg_gpueb_apsrc_req_mask_b;
	u8 reg_gpueb_ddren_req_mask_b;
	u8 reg_gpueb_emi_req_mask_b;
	u8 reg_gpueb_infra_req_mask_b;
	u8 reg_gpueb_pmic_req_mask_b;
	u8 reg_gpueb_srcclkena_mask_b;
	u8 reg_gpueb_vcore_req_mask_b;
	u8 reg_gpueb_vrf18_req_mask_b;
	u8 reg_hwccf_apsrc_req_mask_b;
	u8 reg_hwccf_ddren_req_mask_b;
	u8 reg_hwccf_emi_req_mask_b;
	u8 reg_hwccf_infra_req_mask_b;
	u8 reg_hwccf_pmic_req_mask_b;
	u8 reg_hwccf_srcclkena_mask_b;
	u8 reg_hwccf_vcore_req_mask_b;

	/* SPM_SRC_MASK_10 */
	u8 reg_hwccf_vrf18_req_mask_b;
	u8 reg_img_apsrc_req_mask_b;
	u8 reg_img_ddren_req_mask_b;
	u8 reg_img_emi_req_mask_b;
	u8 reg_img_infra_req_mask_b;
	u8 reg_img_pmic_req_mask_b;
	u8 reg_img_srcclkena_mask_b;
	u8 reg_img_vrf18_req_mask_b;
	u8 reg_infrasys_apsrc_req_mask_b;
	u8 reg_infrasys_ddren_req_mask_b;
	u8 reg_infrasys_emi_req_mask_b;
	u8 reg_infrasys_infra_req_mask_b;
	u8 reg_infrasys_pmic_req_mask_b;
	u8 reg_infrasys_srcclkena_mask_b;
	u8 reg_infrasys_vcore_req_mask_b;
	u8 reg_infrasys_vrf18_req_mask_b;
	u8 reg_ipic_infra_req_mask_b;
	u8 reg_ipic_vrf18_req_mask_b;
	u8 reg_mcu_apsrc_req_mask_b;
	u8 reg_mcu_ddren_req_mask_b;
	u8 reg_mcu_emi_req_mask_b;
	u8 reg_mcu_infra_req_mask_b;
	u8 reg_mcu_pmic_req_mask_b;
	u8 reg_mcu_srcclkena_mask_b;
	u8 reg_mcu_vcore_req_mask_b;
	u8 reg_mcu_vrf18_req_mask_b;
	u8 reg_md_apsrc_req_mask_b;
	u8 reg_md_ddren_req_mask_b;
	u8 reg_md_emi_req_mask_b;
	u8 reg_md_infra_req_mask_b;
	u8 reg_md_pmic_req_mask_b;
	u8 reg_md_srcclkena_mask_b;

	/* SPM_SRC_MASK_11 */
	u8 reg_md_srcclkena1_mask_b;
	u8 reg_md_vcore_req_mask_b;
	u8 reg_md_vrf18_req_mask_b;
	u8 reg_mm_proc_apsrc_req_mask_b;
	u8 reg_mm_proc_ddren_req_mask_b;
	u8 reg_mm_proc_emi_req_mask_b;
	u8 reg_mm_proc_infra_req_mask_b;
	u8 reg_mm_proc_pmic_req_mask_b;
	u8 reg_mm_proc_srcclkena_mask_b;
	u8 reg_mm_proc_vcore_req_mask_b;
	u8 reg_mm_proc_vrf18_req_mask_b;
	u8 reg_mml0_apsrc_req_mask_b;
	u8 reg_mml0_ddren_req_mask_b;
	u8 reg_mml0_emi_req_mask_b;
	u8 reg_mml0_infra_req_mask_b;
	u8 reg_mml0_pmic_req_mask_b;
	u8 reg_mml0_srcclkena_mask_b;
	u8 reg_mml0_vrf18_req_mask_b;
	u8 reg_mml1_apsrc_req_mask_b;
	u8 reg_mml1_ddren_req_mask_b;
	u8 reg_mml1_emi_req_mask_b;
	u8 reg_mml1_infra_req_mask_b;
	u8 reg_mml1_pmic_req_mask_b;
	u8 reg_mml1_srcclkena_mask_b;
	u8 reg_mml1_vrf18_req_mask_b;
	u8 reg_ovl0_apsrc_req_mask_b;
	u8 reg_ovl0_ddren_req_mask_b;
	u8 reg_ovl0_emi_req_mask_b;
	u8 reg_ovl0_infra_req_mask_b;
	u8 reg_ovl0_pmic_req_mask_b;
	u8 reg_ovl0_srcclkena_mask_b;
	u8 reg_ovl0_vrf18_req_mask_b;

	/* SPM_SRC_MASK_12 */
	u8 reg_ovl1_apsrc_req_mask_b;
	u8 reg_ovl1_ddren_req_mask_b;
	u8 reg_ovl1_emi_req_mask_b;
	u8 reg_ovl1_infra_req_mask_b;
	u8 reg_ovl1_pmic_req_mask_b;
	u8 reg_ovl1_srcclkena_mask_b;
	u8 reg_ovl1_vrf18_req_mask_b;
	u8 reg_pcie0_apsrc_req_mask_b;
	u8 reg_pcie0_ddren_req_mask_b;
	u8 reg_pcie0_emi_req_mask_b;
	u8 reg_pcie0_infra_req_mask_b;
	u8 reg_pcie0_pmic_req_mask_b;
	u8 reg_pcie0_srcclkena_mask_b;
	u8 reg_pcie0_vcore_req_mask_b;
	u8 reg_pcie0_vrf18_req_mask_b;
	u8 reg_pcie1_apsrc_req_mask_b;
	u8 reg_pcie1_ddren_req_mask_b;
	u8 reg_pcie1_emi_req_mask_b;
	u8 reg_pcie1_infra_req_mask_b;
	u8 reg_pcie1_pmic_req_mask_b;
	u8 reg_pcie1_srcclkena_mask_b;
	u8 reg_pcie1_vcore_req_mask_b;
	u8 reg_pcie1_vrf18_req_mask_b;
	u8 reg_perisys_apsrc_req_mask_b;
	u8 reg_perisys_ddren_req_mask_b;
	u8 reg_perisys_emi_req_mask_b;
	u8 reg_perisys_infra_req_mask_b;
	u8 reg_perisys_pmic_req_mask_b;
	u8 reg_perisys_srcclkena_mask_b;
	u8 reg_perisys_vcore_req_mask_b;
	u8 reg_perisys_vrf18_req_mask_b;
	u8 reg_pmsr_apsrc_req_mask_b;

	/* SPM_SRC_MASK_13 */
	u8 reg_pmsr_ddren_req_mask_b;
	u8 reg_pmsr_emi_req_mask_b;
	u8 reg_pmsr_infra_req_mask_b;
	u8 reg_pmsr_pmic_req_mask_b;
	u8 reg_pmsr_srcclkena_mask_b;
	u8 reg_pmsr_vcore_req_mask_b;
	u8 reg_pmsr_vrf18_req_mask_b;
	u8 reg_scp_apsrc_req_mask_b;
	u8 reg_scp_ddren_req_mask_b;
	u8 reg_scp_emi_req_mask_b;
	u8 reg_scp_infra_req_mask_b;
	u8 reg_scp_pmic_req_mask_b;
	u8 reg_scp_srcclkena_mask_b;
	u8 reg_scp_vcore_req_mask_b;
	u8 reg_scp_vrf18_req_mask_b;
	u8 reg_spu_hwrot_apsrc_req_mask_b;
	u8 reg_spu_hwrot_ddren_req_mask_b;
	u8 reg_spu_hwrot_emi_req_mask_b;
	u8 reg_spu_hwrot_infra_req_mask_b;
	u8 reg_spu_hwrot_pmic_req_mask_b;
	u8 reg_spu_hwrot_srcclkena_mask_b;
	u8 reg_spu_hwrot_vcore_req_mask_b;
	u8 reg_spu_hwrot_vrf18_req_mask_b;
	u8 reg_spu_ise_apsrc_req_mask_b;
	u8 reg_spu_ise_ddren_req_mask_b;
	u8 reg_spu_ise_emi_req_mask_b;
	u8 reg_spu_ise_infra_req_mask_b;
	u8 reg_spu_ise_pmic_req_mask_b;
	u8 reg_spu_ise_srcclkena_mask_b;
	u8 reg_spu_ise_vcore_req_mask_b;
	u8 reg_spu_ise_vrf18_req_mask_b;

	/* SPM_SRC_MASK_14 */
	u8 reg_srcclkeni_infra_req_mask_b;
	u8 reg_srcclkeni_pmic_req_mask_b;
	u8 reg_srcclkeni_srcclkena_mask_b;
	u8 reg_srcclkeni_vcore_req_mask_b;
	u8 reg_sspm_apsrc_req_mask_b;
	u8 reg_sspm_ddren_req_mask_b;
	u8 reg_sspm_emi_req_mask_b;
	u8 reg_sspm_infra_req_mask_b;
	u8 reg_sspm_pmic_req_mask_b;
	u8 reg_sspm_srcclkena_mask_b;
	u8 reg_sspm_vrf18_req_mask_b;
	u8 reg_ssrsys_apsrc_req_mask_b;
	u8 reg_ssrsys_ddren_req_mask_b;
	u8 reg_ssrsys_emi_req_mask_b;
	u8 reg_ssrsys_infra_req_mask_b;
	u8 reg_ssrsys_pmic_req_mask_b;
	u8 reg_ssrsys_srcclkena_mask_b;
	u8 reg_ssrsys_vcore_req_mask_b;
	u8 reg_ssrsys_vrf18_req_mask_b;
	u8 reg_ssusb_apsrc_req_mask_b;
	u8 reg_ssusb_ddren_req_mask_b;
	u8 reg_ssusb_emi_req_mask_b;
	u8 reg_ssusb_infra_req_mask_b;
	u8 reg_ssusb_pmic_req_mask_b;
	u8 reg_ssusb_srcclkena_mask_b;
	u8 reg_ssusb_vcore_req_mask_b;
	u8 reg_ssusb_vrf18_req_mask_b;
	u8 reg_uart_hub_infra_req_mask_b;

	/* SPM_SRC_MASK_15 */
	u8 reg_uart_hub_pmic_req_mask_b;
	u8 reg_uart_hub_srcclkena_mask_b;
	u8 reg_uart_hub_vcore_req_mask_b;
	u8 reg_uart_hub_vrf18_req_mask_b;
	u8 reg_ufs_apsrc_req_mask_b;
	u8 reg_ufs_ddren_req_mask_b;
	u8 reg_ufs_emi_req_mask_b;
	u8 reg_ufs_infra_req_mask_b;
	u8 reg_ufs_pmic_req_mask_b;
	u8 reg_ufs_srcclkena_mask_b;
	u8 reg_ufs_vcore_req_mask_b;
	u8 reg_ufs_vrf18_req_mask_b;
	u8 reg_vdec_apsrc_req_mask_b;
	u8 reg_vdec_ddren_req_mask_b;
	u8 reg_vdec_emi_req_mask_b;
	u8 reg_vdec_infra_req_mask_b;
	u8 reg_vdec_pmic_req_mask_b;
	u8 reg_vdec_srcclkena_mask_b;
	u8 reg_vdec_vrf18_req_mask_b;
	u8 reg_venc_apsrc_req_mask_b;
	u8 reg_venc_ddren_req_mask_b;
	u8 reg_venc_emi_req_mask_b;
	u8 reg_venc_infra_req_mask_b;
	u8 reg_venc_pmic_req_mask_b;
	u8 reg_venc_srcclkena_mask_b;
	u8 reg_venc_vrf18_req_mask_b;
	u8 reg_vlpcfg_rsv0_apsrc_req_mask_b;
	u8 reg_vlpcfg_rsv0_ddren_req_mask_b;
	u8 reg_vlpcfg_rsv0_emi_req_mask_b;
	u8 reg_vlpcfg_rsv0_infra_req_mask_b;
	u8 reg_vlpcfg_rsv0_pmic_req_mask_b;
	u8 reg_vlpcfg_rsv0_srcclkena_mask_b;

	/* SPM_SRC_MASK_16 */
	u8 reg_vlpcfg_rsv0_vcore_req_mask_b;
	u8 reg_vlpcfg_rsv0_vrf18_req_mask_b;
	u8 reg_vlpcfg_rsv1_apsrc_req_mask_b;
	u8 reg_vlpcfg_rsv1_ddren_req_mask_b;
	u8 reg_vlpcfg_rsv1_emi_req_mask_b;
	u8 reg_vlpcfg_rsv1_infra_req_mask_b;
	u8 reg_vlpcfg_rsv1_pmic_req_mask_b;
	u8 reg_vlpcfg_rsv1_srcclkena_mask_b;
	u8 reg_vlpcfg_rsv1_vcore_req_mask_b;
	u8 reg_vlpcfg_rsv1_vrf18_req_mask_b;

	/* SPM_EVENT_CON_MISC */
	u8 reg_srcclken_fast_resp;
	u8 reg_csyspwrup_ack_mask;

	/* SPM_SRC_MASK_17 */
	u32 reg_spm_sw_rsv_vcore_req_mask_b;
	u32 reg_spm_sw_rsv_pmic_req_mask_b;

	/* SPM_SRC_MASK_18 */
	u32 reg_spm_sw_rsv_srcclkena_mask_b;

	/* SPM_WAKEUP_EVENT_MASK */
	u32 reg_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	u32 reg_ext_wakeup_event_mask;

	/* Auto-gen End */
};

struct mtk_spm_regs {
	u32 poweron_config_en;
	u32 spm_power_on_val[4];
	u32 pcm_pwr_io_en;
	u32 pcm_con0;
	u32 pcm_con1;
	u32 spm_sram_sleep_ctrl;
	u32 spm_clk_con;
	u32 spm_clk_settle;
	u8  reserved0[20];
	u32 spm_sw_rst_con;
	u32 spm_sw_rst_con_set;
	u32 spm_sw_rst_con_clr;
	u8  reserved1[4];
	u32 r_sec_read_mask;
	u32 r_one_time_lock_l;
	u32 r_one_time_lock_m;
	u32 r_one_time_lock_h;
	u8  reserved2[36];
	u32 sspm_clk_con;
	u32 scp_clk_con;
	u8  reserved3[4];
	u32 spm_swint;
	u32 spm_swint_set;
	u32 spm_swint_clr;
	u8  reserved4[20];
	u32 spm_cpu_wakeup_event;
	u32 spm_irq_mask;
	u8  reserved5[72];
	u32 md32pcm_scu_ctrl[4];
	u32 md32pcm_scu_sta0;
	u8  reserved6[20];
	u32 spm_irq_sta;
	u8  reserved7[4];
	u32 md32pcm_wakeup_sta;
	u32 md32pcm_event_sta;
	u8  reserved8[8];
	u32 spm_wakeup_misc;
	u8  reserved9[32];
	u32 spm_ck_sta;
	u8  reserved10[40];
	u32 md32pcm_sta;
	u32 md32pcm_pc;
	u32 spm_ulposc_en_cg_con;
	u32 spm_resource_ulposc_ack_con;
	u32 spm_ulposc_off_mode_con;
	u32 spm_ulposc_ack_sta;
	u32 spm_reosource_ulposc_mask;
	u32 spm_req_block;
	u32 dvfs_ips_ctrl;
	u32 top_cksys_con;
	u8  reserved11[72];
	u32 spm_ap_standby_con;
	u32 cpu_wfi_en;
	u32 cpu_wfi_en_set;
	u32 cpu_wfi_en_clr;
	u32 ext_int_wakeup_req;
	u32 ext_int_wakeup_req_set;
	u32 ext_int_wakeup_req_clr;
	u32 mcusys_idle_sta;
	u32 cpu_pwr_status;
	u32 sw2spm_wakeup;
	u32 sw2spm_wakeup_set;
	u32 sw2spm_wakeup_clr;
	u32 sw2spm_mailbox[4];
	u32 spm2sw_mailbox[4];
	u32 spm2mcupm_con;
	u8  reserved12[12];
	u32 spm_mcusys_pwr_con;
	u32 spm_cputop_pwr_con;
	u32 spm_cpu_pwr_con[8];
	u32 spm_mcupm_spmc_con;
	u32 sodi5_mcusys_con;
	u8  reserved13[16];
	u32 spm_dpm_p2p_sta;
	u32 spm_dpm_p2p_con;
	u32 spm_dpm_intf_sta;
	u32 spm_dpm_wb_con;
	u8  reserved14[80];
	u32 spm_pwrap_con;
	u32 spm_pwrap_con_sta;
	u32 spm_pmic_spmi_con;
	u8 reserved15[4];
	u32 spm_pwrap_cmd[32];
	u32 dvfsrc_event_sta;
	u32 spm_force_dvfs;
	u32 spm_dvfs_sta;
	u32 spm_dvs_dfs_level;
	u32 spm_dvfs_level;
	u32 spm_dvfs_opp;
	u32 spm_ultra_req;
	u32 spm_dvfs_con;
	u32 spm_sramrc_con;
	u32 spm_srclkenrc_con;
	u8  reserved16[4];
	u32 spm_dpsw_vapu_iso_con;
	u32 spm_dpsw_vmm_iso_con;
	u32 spm_dpsw_vmd_iso_con;
	u32 spm_dpsw_vmodem_iso_con;
	u32 spm_dpsw_vcore_iso_con;
	u32 spm_dpsw_con;
	u32 spm_dpsw_con_set;
	u32 spm_dpsw_con_clr;
	u32 spm_dpsw_aoc_iso_con;
	u32 spm_dpsw_aoc_iso_con_set;
	u32 spm_dpsw_aoc_iso_con_clr;
	u32 spm_dpsw_force_switch_con;
	u32 spm_dpsw_force_switch_con_set;
	u32 spm_dpsw_force_switch_con_clr;
	u32 spm2dpsw_ctrl_ack;
	u8  reserved17[8];
	u32 ulposc_con;
	u32 ap_mdsrc_req;
	u32 spm2md_switch_ctrl;
	u32 rc_spm_ctrl;
	u32 spm2gpupm_con;
	u32 spm2apu_con;
	u32 spm2efuse_con;
	u32 spm2dfd_con;
	u32 rsv_pll_con;
	u32 emi_slb_con;
	u32 spm_suspend_flag_con;
	u32 spm2pmsr_con;
	u32 spm_topck_rtff_con;
	u32 emi_shf_con;
	u32 cirq_bypass_con;
	u32 aoc_vcore_sram_con;
	u32 spm2emi_pdn_ctrl;
	u32 vlp_rtff_ctrl_mask;
	u32 vlp_rtff_ctrl_mask_set;
	u32 vlp_rtff_ctrl_mask_clr;
	u32 vlp_rtff_ctrl_mask_2;
	u32 vlp_rtff_ctrl_mask_2_set;
	u32 vlp_rtff_ctrl_mask_2_clr;
	u8  reserved18[4];
	u32 reg_module_sw_cg_ddren_req_mask[4];
	u32 reg_module_sw_cg_vrf18_req_mask[4];
	u32 reg_module_sw_cg_infra_req_mask[4];
	u32 reg_module_sw_cg_f26m_req_mask[4];
	u32 reg_module_sw_cg_vcore_req_mask[4];
	u32 reg_pwr_status_ddren_req_mask;
	u32 reg_pwr_status_vrf18_req_mask;
	u32 reg_pwr_status_infra_req_mask;
	u32 reg_pwr_status_f26m_req_mask;
	u32 reg_pwr_status_pmic_req_mask;
	u32 reg_pwr_status_vcore_req_mask;
	u32 reg_pwr_status_msb_ddren_req_mask;
	u32 reg_pwr_status_msb_vrf18_req_mask;
	u32 reg_pwr_status_msb_infra_req_mask;
	u32 reg_pwr_status_msb_f26m_req_mask;
	u32 reg_pwr_status_msb_pmic_req_mask;
	u32 reg_pwr_status_msb_vcore_req_mask;
	u32 reg_module_busy_ddren_req_mask;
	u32 reg_module_busy_vrf18_req_mask;
	u32 reg_module_busy_infra_req_mask;
	u32 reg_module_busy_f26m_req_mask;
	u32 reg_module_busy_pmic_req_mask;
	u32 reg_module_busy_vcore_req_mask;
	u8  reserved19[8];
	u32 sys_timer_con;
	u32 sys_timer_value_l;
	u32 sys_timer_value_h;
	u32 sys_timer_start_l;
	u32 sys_timer_start_h;
	struct {
		u32 latch_l;
		u32 latch_h;
	} sys_timer_latch[16];
	u32 pcm_timer_val;
	u32 pcm_timer_out;
	u32 spm_counter[3];
	u32 pcm_wdt_val;
	u32 pcm_wdt_out;
	u8  reserved20[80];
	u32 spm_sw_flag_0;
	u32 spm_sw_debug_0;
	u32 spm_sw_flag_1;
	u32 spm_sw_debug_1;
	u32 spm_sw_rsv[9];
	u32 spm_bk_wake_event;
	u32 spm_bk_vtcxo_dur;
	u32 spm_bk_wake_misc;
	u32 spm_bk_pcm_timer;
	u8  reserved21[12];
	u32 spm_rsv_con_0;
	u32 spm_rsv_con_1;
	u32 spm_rsv_sta_0;
	u32 spm_rsv_sta_1;
	u32 spm_spare_con;
	u32 spm_spare_con_set;
	u32 spm_spare_con_clr;
	u32 spm_cross_wake_m00_req;
	u32 spm_cross_wake_m01_req;
	u32 spm_cross_wake_m02_req;
	u32 spm_cross_wake_m03_req;
	u32 scp_vcore_level;
	u32 spm_ddren_ack_sel_con;
	u32 spm_sw_flag_2;
	u32 spm_sw_debug_2;
	u32 spm_dv_con_0;
	u32 spm_dv_con_1;
	u8  reserved22[8];
	u32 spm_sema_m[8];
	u32 spm2adsp_mailbox;
	u32 adsp2spm_mailbox;
	u32 spm2pmcu_mailbox[4];
	u32 pmcu2spm_mailbox[4];
	u32 spm2scp_mailbox;
	u32 scp2spm_mailbox;
	u32 scp_aov_bus_con;
	u32 vcore_rtff_ctrl_mask;
	u32 vcore_rtff_ctrl_mask_set;
	u32 vcore_rtff_ctrl_mask_clr;
	u32 spm_sram_srclkeno_mask;
	u8  reserved23[256];
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_event_mask;
	u32 spm_wakeup_event_ext_mask;
	u32 spm_wakeup_event_sens;
	u32 spm_wakeup_event_clear;
	u32 spm_src_req;
	u32 spm_src_mask[17];
	u32 spm_req_sta[17];
	u32 spm_ipc_wakeup_req;
	u32 ipc_wakeup_req_mask_sta;
	u32 spm_event_con_misc;
	u32 ddren_dbc_con;
	u32 spm_resource_ack_con[2];
	u32 spm_resource_ack_mask[13];
	u32 spm_event_counter_clear;
	u32 spm_vcore_event_count_sta;
	u32 spm_pmic_event_count_sta;
	u32 spm_srclkena_event_count_sta;
	u32 spm_infra_event_count_sta;
	u32 spm_vrf18_event_count_sta;
	u32 spm_emi_event_count_sta;
	u32 spm_apsrc_event_count_sta;
	u32 spm_ddren_event_count_sta;
	u32 spm_src_mask_17;
	u32 spm_src_mask_18;
	u32 spm_resource_ack_mask_13;
	u32 spm_resource_ack_mask_14;
	u32 spm_req_sta_17;
	u32 spm_req_sta_18;
	u8  reserved24[212];
	u32 md32pcm_cfgreg_sw_rstn;
	u8  reserved25[508];
	u32 md32pcm_dma0_src;
	u32 md32pcm_dma0_dst;
	u32 md32pcm_dma0_wppt;
	u32 md32pcm_dma0_wpto;
	u32 md32pcm_dma0_count;
	u32 md32pcm_dma0_con;
	u32 md32pcm_dma0_start;
	u8  reserved26[8];
	u32 md32pcm_dma0_rlct;
	u8  reserved27[472];
	u32 md1_pwr_con;
	u32 conn_pwr_con;
	u32 apifr_io_pwr_con;
	u32 apifr_mem_pwr_con;
	u32 peri_pwr_con;
	u32 peri_ether_pwr_con;
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
	u32 adsp_core1_pwr_con;
	u32 adsp_top_pwr_con;
	u32 adsp_infra_pwr_con;
	u32 adsp_ao_pwr_con;
	u32 mm_proc_pwr_con;
	u32 scp_pwr_con;
	u32 dpm0_pwr_con;
	u32 dpm1_pwr_con;
	u32 dpm2_pwr_con;
	u32 dpm3_pwr_con;
	u32 emi0_pwr_con;
	u32 emi1_pwr_con;
	u32 emi_infra_pwr_con;
	u32 ssrsys_pwr_con;
	u32 spu_ise_pwr_con;
	u32 spu_hwrot_pwr_con;
	u32 vlp_pwr_con;
	u32 hsgmii0_pwr_con;
	u32 hsgmii1_pwr_con;
	u32 mfg_vlp_pwr_con;
	u32 mcusys_busblk_pwr_con;
	u32 cpueb_pwr_con;
	u32 mfg0_pwr_con;
	u32 adsp_hre_sram_con;
	u32 ccu_sleep_sram_con;
	u32 efuse_sram_con;
	u32 emi_hre_sram_con;
	u32 infra_hre_sram_con;
	u32 infra_sleep_sram_con;
	u32 mml_hre_sram_con;
	u32 mm_hre_sram_con;
	u32 mm_infra_ao_pdn_sram_con;
	u32 nth_emi_slb_sram_con;
	u32 peri_sleep_sram_con;
	u32 spm_sram_con;
	u32 spu_hwrot_sleep_sram_con;
	u32 spu_ise_sleep_sram_con;
	u32 sspm_sram_con;
	u32 ssr_sleep_sram_con;
	u32 sth_emi_slb_sram_con;
	u32 ufs_sleep_sram_con;
	u32 unipro_pdn_sram_con;
	u32 cpu_buck_iso_con;
	u32 md_buck_iso_con;
	u32 soc_buck_iso_con;
	u32 reserved28[2];
	u32 soc_buck_iso_con_set;
	u32 soc_buck_iso_con_clr;
	u32 pwr_status;
	u32 pwr_status_2nd;
	u32 pwr_status_msb;
	u32 pwr_status_msb_2nd;
	u32 xpu_pwr_status;
	u32 xpu_pwr_status_2nd;
	u32 dfd_soc_pwr_latch;
	u32 nth_emi_slb_sram_ack;
	u32 sth_emi_slb_sram_ack;
	u32 dpyd0_pwr_con;
	u32 dpyd1_pwr_con;
	u32 dpyd2_pwr_con;
	u32 dpyd3_pwr_con;
	u32 dpya0_pwr_con;
	u32 dpya1_pwr_con;
	u32 dpya2_pwr_con;
	u32 reserved29[2];
	u32 dpya3_pwr_con;
	u32 scp_2_pwr_con;
	u32 rsv_0_sleep_sram_con;
	u32 rsv_1_sleep_sram_con;
	u32 apifr_mem_sleep_sram_con;
	u32 rsv_0_pwr_con;
	u32 rsv_1_pwr_con;
	u32 reserved30[22];
	u32 spm_twam_con;
	u32 spm_twam_window_len;
	u32 spm_twam_idle_sel;
	u32 spm_twam_last_sta[4];
	u32 spm_twam_curr_sta[4];
	u32 spm_twam_timer_out;
	u32 reserved31[8192];
	u32 md1_ssyspm_con;
	u32 conn_ssyspm_con;
	u32 apifr_io_ssyspm_con;
	u32 apifr_mem_ssyspm_con;
	u32 peri_ssyspm_con;
	u32 peri_ether_ssyspm_con;
	u32 ssusb_dp_phy_p0_ssyspm_con;
	u32 ssusb_p0_ssyspm_con;
	u32 ssusb_p1_ssyspm_con;
	u32 ssusb_p23_ssyspm_con;
	u32 ssusb_phy_p2_ssyspm_con;
	u32 ufs0_ssyspm_con;
	u32 ufs0_phy_ssyspm_con;
	u32 pextp_mac0_ssyspm_con;
	u32 pextp_mac1_ssyspm_con;
	u32 pextp_mac2_ssyspm_con;
	u32 pextp_phy0_ssyspm_con;
	u32 pextp_phy1_ssyspm_con;
	u32 pextp_phy2_ssyspm_con;
	u32 audio_ssyspm_con;
	u32 adsp_core1_ssyspm_con;
	u32 adsp_top_ssyspm_con;
	u32 adsp_infra_ssyspm_con;
	u32 adsp_ao_ssyspm_con;
	u32 mm_proc_ssyspm_con;
	u32 scp_ssyspm_con;
	u32 scp_2_ssyspm_con;
	u32 dpyd0_ssyspm_con;
	u32 dpyd1_ssyspm_con;
	u32 dpyd2_ssyspm_con;
	u32 dpyd3_ssyspm_con;
	u32 dpya0_ssyspm_con;
	u32 dpya1_ssyspm_con;
	u32 dpya2_ssyspm_con;
	u32 dpya3_ssyspm_con;
	u32 dpm0_ssyspm_con;
	u32 dpm1_ssyspm_con;
	u32 dpm2_ssyspm_con;
	u32 dpm3_ssyspm_con;
	u32 emi0_ssyspm_con;
	u32 emi1_ssyspm_con;
	u32 emi_infra_ssyspm_con;
	u32 ssrsys_ssyspm_con;
	u32 spu_ise_ssyspm_con;
	u32 spu_hwrot_ssyspm_con;
	u32 vlp_ssyspm_con;
	u32 hsgmii0_ssyspm_con;
	u32 hsgmii1_ssyspm_con;
	u32 mfg_vlp_ssyspm_con;
	u32 mcusys_busblk_ssyspm_con;
	u32 rsv_0_ssyspm_con;
	u32 rsv_1_ssyspm_con;
	u32 cpueb_ssyspm_con;
	u32 mfg0_ssyspm_con;
	u32 bus_protect_con;
	u32 bus_protect_con_set;
	u32 bus_protect_con_clr;
	u32 bus_protect_msb_con;
	u32 bus_protect_msb_con_set;
	u32 bus_protect_msb_con_clr;
	u32 bus_protect_cg_con;
	u32 bus_protect_cg_con_set;
	u32 bus_protect_cg_con_clr;
	u32 bus_protect_cg_msb_con;
	u32 ocla_en;
	u32 ocla_sw_rst;
	u32 ocla_config_0;
	u32 ocla_cand_mux_sel;
	u32 ocla_sleep_sram_con;
	u32 ocla_comp_val;
	u32 ocla_vcore_req_sel;
	u32 ocla_vcore_ack_sel;
	u32 ocla_pmic_req_sel;
	u32 ocla_pmic_ack_sel;
	u32 ocla_26m_req_sel;
	u32 ocla_26m_ack_sel;
	u32 ocla_infra_req_sel;
	u32 ocla_infra_ack_sel;
	u32 ocla_buspll_req_sel;
	u32 ocla_buspll_ack_sel;
	u32 ocla_emi_req_sel;
	u32 ocla_emi_ack_sel;
	u32 ocla_apsrc_req_sel;
	u32 ocla_apsrc_ack_sel;
	u32 ocla_ddren_req_sel;
	u32 ocla_ddren_ack_sel;
	u32 ocla_mtcmos_pwr_on_sel;
	u32 ocla_mtcmos_pwr_ack_sel;
	u32 ocla_mon_mode[2];
	u32 ocla_bit_seq[2];
	u32 ocla_bit_en;
	u32 ocla_timer_lsb;
	u32 ocla_timer_msb;
	u32 ocla_trig_addr;
	u32 ocla_sta;
	u32 ocla_level_max_time;
	u32 ocla_rsv[6];
	u32 reserved32[24];
	u32 bus_protect_cg_msb_con_set;
	u32 bus_protect_cg_msb_con_clr;
	u32 bus_protect_rdy;
	u32 bus_protect_rdy_msb;
	u32 spm_rsv_ulposc_req;
	u32 spm_sw_rsv_vcore_req_con;
	u32 spm_sw_rsv_vcore_req_con_set;
	u32 spm_sw_rsv_vcore_req_con_clr;
	u32 spm_lteclksq_bg_off;
	u32 reserved33[55];
	u32 pbus_vcore_pkt_ctrl;
	u32 pbus_vlp_pkt_ctrl;
	u32 reserved34[2];
	u32 pbus_vlp_pkt_data[4];
	u32 pbus_vcore_pkt_data[4];
	u32 pbus_vcore_ctrl;
	u32 pbus_vlp_ctrl;
	u32 reserved35[2];
	u32 pbus_vcore_rx_pkt_ctrl;
	u32 pbus_vlp_rx_pkt_ctrl;
	u32 reserved36[2];
	u32 pbus_vlp_rx_pkt_data[4];
	u32 pbus_vcore_rx_pkt_data[4];
	u32 reserved37[100];
	u32 pcm_wdt_latch[39];
	u32 pcm_wdt_latch_spare[10];
	u32 dramc_gating_err_latch[6];
	u32 dramc_gating_err_latch_spare_0;
	u32 spm_debug_con;
	struct {
		u32 con;
		u32 sel;
		u32 timer;
		u32 sta;
	} spm_ack_chk[4];
	u32 reserved38[3];
	u32 pcm_apwdt_latch[27];
	u32 reserved39[2];
	u32 pcm_apwdt_latch_27[12];
};

check_member(mtk_spm_regs, spm_wakeup_misc, 0x140);
check_member(mtk_spm_regs, spm_mcupm_spmc_con, 0x288);
check_member(mtk_spm_regs, spm_dpm_wb_con, 0x2AC);
check_member(mtk_spm_regs, spm_pwrap_cmd[31], 0x38C);
check_member(mtk_spm_regs, spm_dpsw_con, 0x3D0);
check_member(mtk_spm_regs, spm2emi_pdn_ctrl, 0x440);
check_member(mtk_spm_regs, reg_module_busy_vcore_req_mask, 0x4F4);
check_member(mtk_spm_regs, sys_timer_con, 0x500);
check_member(mtk_spm_regs, pcm_wdt_out, 0x5AC);
check_member(mtk_spm_regs, spm_sram_srclkeno_mask, 0x6FC);
check_member(mtk_spm_regs, spm_vcore_event_count_sta, 0x8F4);
check_member(mtk_spm_regs, md32pcm_cfgreg_sw_rstn, 0xA00);
check_member(mtk_spm_regs, md32pcm_dma0_src, 0xC00);
check_member(mtk_spm_regs, md32pcm_dma0_rlct, 0xC24);
check_member(mtk_spm_regs, md1_pwr_con, 0xE00);
check_member(mtk_spm_regs, sspm_sram_con, 0xEE4);
check_member(mtk_spm_regs, soc_buck_iso_con, 0xF00);
check_member(mtk_spm_regs, spm_twam_timer_out, 0xFFC);
check_member(mtk_spm_regs, spm_lteclksq_bg_off, 0x9220);
check_member(mtk_spm_regs, pbus_vcore_pkt_ctrl, 0x9300);
check_member(mtk_spm_regs, pbus_vlp_pkt_data[3], 0x931C);
check_member(mtk_spm_regs, pbus_vcore_rx_pkt_data[3], 0x936C);
check_member(mtk_spm_regs, pcm_wdt_latch, 0x9500);
check_member(mtk_spm_regs, spm_ack_chk[3].sta, 0x9620);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

#endif  /* SOC_MEDIATEK_MT8196_SPM_H */
