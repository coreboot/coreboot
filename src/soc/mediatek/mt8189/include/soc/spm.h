/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_H__

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <soc/pll.h>
#include <soc/spm_common.h>
#include <types.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE			0xB16
#define SPM_REGWR_CFG_KEY			(SPM_PROJECT_CODE << 16)

/* POWERON_CONFIG_EN (0x1C001000 + 0x000) */
#define BCLK_CG_EN_LSB				BIT(0)
#define PROJECT_CODE_LSB			BIT(16)

/* SPM_CLK_CON (0x1C001000 + 0x024) */
#define REG_SPM_LOCK_INFRA_DCM_LSB		BIT(0)
#define REG_CXO32K_REMOVE_EN_LSB		BIT(1)
#define REG_SPM_LEAVE_SUSPEND_MERGE_MASK_LSB	BIT(4)
#define REG_SRCLKENO0_SRC_MASK_B_LSB		BIT(8)
#define REG_SRCLKENO1_SRC_MASK_B_LSB		BIT(16)
#define REG_SRCLKENO2_SRC_MASK_B_LSB		BIT(24)

/* PCM_CON0 (0x1C001000 + 0x018) */
#define PCM_CK_EN_LSB				BIT(2)
#define PCM_SW_RESET_LSB			BIT(15)
#define PCM_CON0_PROJECT_CODE_LSB		BIT(16)

/* PCM_CON1 (0x1C001000 + 0x01C) */
#define REG_SPM_APB_INTERNAL_EN_LSB		BIT(3)
#define REG_PCM_TIMER_EN_LSB			BIT(5)
#define REG_PCM_WDT_EN_LSB			BIT(8)
#define REG_PCM_WDT_WAKE_LSB			BIT(9)
#define REG_SSPM_APB_P2P_EN_LSB			BIT(10)
#define REG_MCUPM_APB_P2P_EN_LSB		BIT(11)
#define REG_RSV_APB_P2P_EN_LSB			BIT(12)
#define RG_PCM_IRQ_MSK_LSB			BIT(15)
#define PCM_CON1_PROJECT_CODE_LSB		BIT(16)

/* SPM_WAKEUP_EVENT_MASK (0x1C001000 + 0x808) */
#define REG_WAKEUP_EVENT_MASK_LSB		BIT(0)

/* DDREN_DBC_CON (0x1C001000 + 0x890) */
#define REG_DDREN_DBC_LEN_LSB			BIT(0)
#define REG_DDREN_DBC_EN_LSB			BIT(16)

/* SPM_DVFS_CON (0x1C001000 + 0x3AC) */
#define SPM_DVFS_FORCE_ENABLE_LSB		BIT(2)
#define FORCE_DVFS_WAKE_LSB			BIT(3)
#define SPM_DVFSRC_ENABLE_LSB			BIT(4)
#define DVFSRC_WAKEUP_EVENT_MASK_LSB		BIT(6)
#define SPM2RC_EVENT_ABORT_LSB			BIT(7)
#define DVFSRC_LEVEL_ACK_LSB			BIT(8)

/* SPM_SW_FLAG_0 (0x1C001000 + 0x600) */
#define SPM_SW_FLAG_LSB				BIT(0)

/* SYS_TIMER_CON (0x1C001000 + 0x500) */
#define SYS_TIMER_START_EN_LSB			BIT(0)
#define SYS_TIMER_LATCH_EN_LSB			BIT(1)
#define SYS_TIMER_ID_LSB			BIT(8)
#define SYS_TIMER_VALID_LSB			BIT(31)

/**************************************
 * Config and Parameter
 **************************************/
#define POWER_ON_VAL0_DEF		0x0000F100
#define POWER_ON_VAL1_DEF		0x003FFE20
#define SPM_WAKEUP_EVENT_MASK_DEF	0xF97FFCFF
#define SPM_BUS_PROTECT_MASK_B_DEF	0xFFFFFFFF
#define SPM_BUS_PROTECT2_MASK_B_DEF	0xFFFFFFFF
#define MD32PCM_DMA0_CON_VAL		0x0003820E
#define MD32PCM_DMA0_START_VAL		0x00008000
#define SPM_DVFS_LEVEL_DEF		0x00000001
#define SPM_DVS_DFS_LEVEL_DEF		0x00010001
#define SPM_RESOURCE_ACK_CON0_DEF	0xCC4E4ECC
#define SPM_RESOURCE_ACK_CON1_DEF	0x00CCCCCC
#define SPM_SYSCLK_SETTLE		0x60FE /* 1685us */
#define SPM_INIT_DONE_US		20
#define PCM_WDT_TIMEOUT			(30 * 32768)
#define PCM_TIMER_MAX			((5400 - 30) * 32768) /* 90min - 30sec */

/**************************************
 * Definition and Declaration
 **************************************/
/* SPM_IRQ_MASK */
#define ISRM_TWAM		BIT(2)
#define ISRM_RET_IRQ1		BIT(9)
#define ISRM_RET_IRQ2		BIT(10)
#define ISRM_RET_IRQ3		BIT(11)
#define ISRM_RET_IRQ4		BIT(12)
#define ISRM_RET_IRQ5		BIT(13)
#define ISRM_RET_IRQ6		BIT(14)
#define ISRM_RET_IRQ7		BIT(15)
#define ISRM_RET_IRQ8		BIT(16)
#define ISRM_RET_IRQ9		BIT(17)
#define ISRM_RET_IRQ_AUX	(ISRM_RET_IRQ9 | ISRM_RET_IRQ8 | \
				 ISRM_RET_IRQ7 | ISRM_RET_IRQ6 | \
				 ISRM_RET_IRQ5 | ISRM_RET_IRQ4 | \
				 ISRM_RET_IRQ3 | ISRM_RET_IRQ2 | \
				 ISRM_RET_IRQ1)

#define ISRM_ALL_EXC_TWAM	ISRM_RET_IRQ_AUX
#define ISRM_ALL		(ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM		BIT(2)
#define ISRS_PCM_RETURN		BIT(3)
#define ISRC_TWAM		ISRS_TWAM
#define ISRC_ALL_EXC_TWAM	ISRS_PCM_RETURN
#define ISRC_ALL		(ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* PCM_PWR_IO_EN */
#define PCM_PWRIO_EN_R0		BIT(0)
#define PCM_PWRIO_EN_R7		BIT(7)
#define PCM_RF_SYNC_R0		BIT(16)
#define PCM_RF_SYNC_R6		BIT(22)
#define PCM_RF_SYNC_R7		BIT(23)

/* SPM_SWINT */
#define PCM_SW_INT0		BIT(0)
#define PCM_SW_INT1		BIT(1)
#define PCM_SW_INT2		BIT(2)
#define PCM_SW_INT3		BIT(3)
#define PCM_SW_INT4		BIT(4)
#define PCM_SW_INT5		BIT(5)
#define PCM_SW_INT6		BIT(6)
#define PCM_SW_INT7		BIT(7)
#define PCM_SW_INT8		BIT(8)
#define PCM_SW_INT9		BIT(9)
#define PCM_SW_INT_ALL		(PCM_SW_INT9 | PCM_SW_INT8 | PCM_SW_INT7 | \
				 PCM_SW_INT6 | PCM_SW_INT5 | PCM_SW_INT4 | \
				 PCM_SW_INT3 | PCM_SW_INT2 | PCM_SW_INT1 | \
				 PCM_SW_INT0)

#define SPM_ACK_CHK_3_SEL_HW_S1	0x00350098
#define SPM_ACK_CHK_3_HW_S1_CNT	1

DEFINE_BIT(SPM_ACK_CHK_3_CON_CLR_ALL, 1)
DEFINE_BIT(SPM_ACK_CHK_3_CON_EN_0, 4)
DEFINE_BIT(SPM_ACK_CHK_3_CON_EN_1, 8)
DEFINE_BIT(SPM_ACK_CHK_3_CON_HW_MODE_TRIG, 11)

struct pwr_ctrl {
	/* for SPM */
	u32 pcm_flags;
	/* can override pcm_flags */
	u32 pcm_flags_cust;
	/* set bit of pcm_flags, after pcm_flags_cust */
	u32 pcm_flags_cust_set;
	/* clr bit of pcm_flags, after pcm_flags_cust */
	u32 pcm_flags_cust_clr;
	u32 pcm_flags1;
	/* can override pcm_flags1 */
	u32 pcm_flags1_cust;
	/* set bit of pcm_flags1, after pcm_flags1_cust */
	u32 pcm_flags1_cust_set;
	/* clr bit of pcm_flags1, after pcm_flags1_cust */
	u32 pcm_flags1_cust_clr;
	/* @ 1T 32K */
	u32 timer_val;
	/* @ 1T 32K, can override timer_val */
	u32 timer_val_cust;
	/* stress for dpidle */
	u32 timer_val_ramp_en;
	/* stress for suspend */
	u32 timer_val_ramp_en_sec;
	u32 wake_src;
	/* can override wake_src */
	u32 wake_src_cust;
	u32 wakelock_timer_val;
	/* disable wdt in suspend */
	u8 wdt_disable;

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
	u8 reg_apu_apsrc_req_mask_b;
	u8 reg_apu_ddren_req_mask_b;
	u8 reg_apu_emi_req_mask_b;
	u8 reg_apu_infra_req_mask_b;
	u8 reg_apu_pmic_req_mask_b;
	u8 reg_apu_srcclkena_mask_b;
	u8 reg_apu_vrf18_req_mask_b;
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
	u8 reg_mdp_emi_req_mask_b;

	/* SPM_SRC_MASK_1 */
	u32 reg_ccif_apsrc_req_mask_b;
	u32 reg_ccif_emi_req_mask_b;

	/* SPM_SRC_MASK_2 */
	u32 reg_ccif_infra_req_mask_b;
	u32 reg_ccif_pmic_req_mask_b;

	/* SPM_SRC_MASK_3 */
	u32 reg_ccif_srcclkena_mask_b;
	u32 reg_ccif_vrf18_req_mask_b;
	u8 reg_ccu_apsrc_req_mask_b;
	u8 reg_ccu_ddren_req_mask_b;
	u8 reg_ccu_emi_req_mask_b;
	u8 reg_ccu_infra_req_mask_b;
	u8 reg_ccu_pmic_req_mask_b;
	u8 reg_ccu_srcclkena_mask_b;
	u8 reg_ccu_vrf18_req_mask_b;
	u8 reg_cg_check_apsrc_req_mask_b;

	/* SPM_SRC_MASK_4 */
	u8 reg_cg_check_ddren_req_mask_b;
	u8 reg_cg_check_emi_req_mask_b;
	u8 reg_cg_check_infra_req_mask_b;
	u8 reg_cg_check_pmic_req_mask_b;
	u8 reg_cg_check_srcclkena_mask_b;
	u8 reg_cg_check_vcore_req_mask_b;
	u8 reg_cg_check_vrf18_req_mask_b;
	u8 reg_conn_apsrc_req_mask_b;
	u8 reg_conn_ddren_req_mask_b;
	u8 reg_conn_emi_req_mask_b;
	u8 reg_conn_infra_req_mask_b;
	u8 reg_conn_pmic_req_mask_b;
	u8 reg_conn_srcclkena_mask_b;
	u8 reg_conn_srcclkenb_mask_b;
	u8 reg_conn_vcore_req_mask_b;
	u8 reg_conn_vrf18_req_mask_b;
	u8 reg_cpueb_apsrc_req_mask_b;
	u8 reg_cpueb_ddren_req_mask_b;
	u8 reg_cpueb_emi_req_mask_b;
	u8 reg_cpueb_infra_req_mask_b;
	u8 reg_cpueb_pmic_req_mask_b;
	u8 reg_cpueb_srcclkena_mask_b;
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

	/* SPM_SRC_MASK_5 */
	u8 reg_disp1_emi_req_mask_b;
	u8 reg_disp1_infra_req_mask_b;
	u8 reg_disp1_pmic_req_mask_b;
	u8 reg_disp1_srcclkena_mask_b;
	u8 reg_disp1_vrf18_req_mask_b;
	u8 reg_dpm_apsrc_req_mask_b;
	u8 reg_dpm_ddren_req_mask_b;
	u8 reg_dpm_emi_req_mask_b;
	u8 reg_dpm_infra_req_mask_b;
	u8 reg_dpm_pmic_req_mask_b;
	u8 reg_dpm_srcclkena_mask_b;

	/* SPM_SRC_MASK_6 */
	u8 reg_dpm_vcore_req_mask_b;
	u8 reg_dpm_vrf18_req_mask_b;
	u8 reg_dpmaif_apsrc_req_mask_b;
	u8 reg_dpmaif_ddren_req_mask_b;
	u8 reg_dpmaif_emi_req_mask_b;
	u8 reg_dpmaif_infra_req_mask_b;
	u8 reg_dpmaif_pmic_req_mask_b;
	u8 reg_dpmaif_srcclkena_mask_b;
	u8 reg_dpmaif_vrf18_req_mask_b;
	u8 reg_dvfsrc_level_req_mask_b;
	u8 reg_emisys_apsrc_req_mask_b;
	u8 reg_emisys_ddren_req_mask_b;
	u8 reg_emisys_emi_req_mask_b;
	u8 reg_gce_d_apsrc_req_mask_b;
	u8 reg_gce_d_ddren_req_mask_b;
	u8 reg_gce_d_emi_req_mask_b;
	u8 reg_gce_d_infra_req_mask_b;
	u8 reg_gce_d_pmic_req_mask_b;
	u8 reg_gce_d_srcclkena_mask_b;
	u8 reg_gce_d_vrf18_req_mask_b;
	u8 reg_gce_m_apsrc_req_mask_b;
	u8 reg_gce_m_ddren_req_mask_b;
	u8 reg_gce_m_emi_req_mask_b;
	u8 reg_gce_m_infra_req_mask_b;
	u8 reg_gce_m_pmic_req_mask_b;
	u8 reg_gce_m_srcclkena_mask_b;

	/* SPM_SRC_MASK_7 */
	u8 reg_gce_m_vrf18_req_mask_b;
	u8 reg_gpueb_apsrc_req_mask_b;
	u8 reg_gpueb_ddren_req_mask_b;
	u8 reg_gpueb_emi_req_mask_b;
	u8 reg_gpueb_infra_req_mask_b;
	u8 reg_gpueb_pmic_req_mask_b;
	u8 reg_gpueb_srcclkena_mask_b;
	u8 reg_gpueb_vrf18_req_mask_b;
	u8 reg_hwccf_apsrc_req_mask_b;
	u8 reg_hwccf_ddren_req_mask_b;
	u8 reg_hwccf_emi_req_mask_b;
	u8 reg_hwccf_infra_req_mask_b;
	u8 reg_hwccf_pmic_req_mask_b;
	u8 reg_hwccf_srcclkena_mask_b;
	u8 reg_hwccf_vcore_req_mask_b;
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
	u8 reg_ipic_infra_req_mask_b;
	u8 reg_ipic_vrf18_req_mask_b;
	u8 reg_mcu_apsrc_req_mask_b;
	u8 reg_mcu_ddren_req_mask_b;
	u8 reg_mcu_emi_req_mask_b;

	/* SPM_SRC_MASK_8 */
	u8 reg_mcusys_apsrc_req_mask_b;
	u8 reg_mcusys_ddren_req_mask_b;
	u8 reg_mcusys_emi_req_mask_b;
	u8 reg_mcusys_infra_req_mask_b;

	/* SPM_SRC_MASK_9 */
	u8 reg_mcusys_pmic_req_mask_b;
	u8 reg_mcusys_srcclkena_mask_b;
	u8 reg_mcusys_vrf18_req_mask_b;
	u8 reg_md_apsrc_req_mask_b;
	u8 reg_md_ddren_req_mask_b;
	u8 reg_md_emi_req_mask_b;
	u8 reg_md_infra_req_mask_b;
	u8 reg_md_pmic_req_mask_b;
	u8 reg_md_srcclkena_mask_b;
	u8 reg_md_srcclkena1_mask_b;
	u8 reg_md_vcore_req_mask_b;

	/* SPM_SRC_MASK_10 */
	u8 reg_md_vrf18_req_mask_b;
	u8 reg_mdp_apsrc_req_mask_b;
	u8 reg_mdp_ddren_req_mask_b;
	u8 reg_mm_proc_apsrc_req_mask_b;
	u8 reg_mm_proc_ddren_req_mask_b;
	u8 reg_mm_proc_emi_req_mask_b;
	u8 reg_mm_proc_infra_req_mask_b;
	u8 reg_mm_proc_pmic_req_mask_b;
	u8 reg_mm_proc_srcclkena_mask_b;
	u8 reg_mm_proc_vrf18_req_mask_b;
	u8 reg_mmsys_apsrc_req_mask_b;
	u8 reg_mmsys_ddren_req_mask_b;
	u8 reg_mmsys_vrf18_req_mask_b;
	u8 reg_pcie0_apsrc_req_mask_b;
	u8 reg_pcie0_ddren_req_mask_b;
	u8 reg_pcie0_infra_req_mask_b;
	u8 reg_pcie0_srcclkena_mask_b;
	u8 reg_pcie0_vrf18_req_mask_b;
	u8 reg_pcie1_apsrc_req_mask_b;
	u8 reg_pcie1_ddren_req_mask_b;
	u8 reg_pcie1_infra_req_mask_b;
	u8 reg_pcie1_srcclkena_mask_b;
	u8 reg_pcie1_vrf18_req_mask_b;
	u8 reg_perisys_apsrc_req_mask_b;
	u8 reg_perisys_ddren_req_mask_b;
	u8 reg_perisys_emi_req_mask_b;
	u8 reg_perisys_infra_req_mask_b;
	u8 reg_perisys_pmic_req_mask_b;
	u8 reg_perisys_srcclkena_mask_b;
	u8 reg_perisys_vcore_req_mask_b;
	u8 reg_perisys_vrf18_req_mask_b;
	u8 reg_scp_apsrc_req_mask_b;

	/* SPM_SRC_MASK_11 */
	u8 reg_scp_ddren_req_mask_b;
	u8 reg_scp_emi_req_mask_b;
	u8 reg_scp_infra_req_mask_b;
	u8 reg_scp_pmic_req_mask_b;
	u8 reg_scp_srcclkena_mask_b;
	u8 reg_scp_vcore_req_mask_b;
	u8 reg_scp_vrf18_req_mask_b;
	u8 reg_srcclkeni_infra_req_mask_b;
	u8 reg_srcclkeni_pmic_req_mask_b;
	u8 reg_srcclkeni_srcclkena_mask_b;
	u8 reg_sspm_apsrc_req_mask_b;
	u8 reg_sspm_ddren_req_mask_b;
	u8 reg_sspm_emi_req_mask_b;
	u8 reg_sspm_infra_req_mask_b;
	u8 reg_sspm_pmic_req_mask_b;
	u8 reg_sspm_srcclkena_mask_b;
	u8 reg_sspm_vrf18_req_mask_b;
	u8 reg_ssr_apsrc_req_mask_b;
	u8 reg_ssr_ddren_req_mask_b;
	u8 reg_ssr_emi_req_mask_b;
	u8 reg_ssr_infra_req_mask_b;
	u8 reg_ssr_pmic_req_mask_b;
	u8 reg_ssr_srcclkena_mask_b;
	u8 reg_ssr_vrf18_req_mask_b;
	u8 reg_ufs_apsrc_req_mask_b;
	u8 reg_ufs_ddren_req_mask_b;
	u8 reg_ufs_emi_req_mask_b;
	u8 reg_ufs_infra_req_mask_b;
	u8 reg_ufs_pmic_req_mask_b;

	/* SPM_SRC_MASK_12 */
	u8 reg_ufs_srcclkena_mask_b;
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
	u8 reg_ipe_apsrc_req_mask_b;
	u8 reg_ipe_ddren_req_mask_b;
	u8 reg_ipe_emi_req_mask_b;
	u8 reg_ipe_infra_req_mask_b;
	u8 reg_ipe_pmic_req_mask_b;
	u8 reg_ipe_srcclkena_mask_b;
	u8 reg_ipe_vrf18_req_mask_b;
	u8 reg_ufs_vcore_req_mask_b;

	/* SPM_EVENT_CON_MISC */
	u8 reg_srcclken_fast_resp;
	u8 reg_csyspwrup_ack_mask;

	/* SPM_WAKEUP_EVENT_MASK */
	u32 reg_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	u32 reg_ext_wakeup_event_mask;
};

struct mtk_spm_regs {
	u32 poweron_config_set;
	u32 spm_power_on_val[4];
	u32 pcm_pwr_io_en;
	u32 pcm_con0;
	u32 pcm_con1;
	u32 spm_sram_sleep_ctrl;
	u32 spm_clk_con;
	u32 spm_clk_settle;
	u32 spm_clk_con1;
	u8 reserved0[16];
	u32 spm_sw_rst_con;
	u32 spm_sw_rst_con_set;
	u32 spm_sw_rst_con_clr;
	u8 reserved1[4];
	u32 spm_sec_read_mask;
	u32 spm_one_time_lock_l;
	u32 spm_one_time_lock_m;
	u32 spm_one_time_lock_h;
	u8 reserved2[36];
	u32 sspm_clk_con;
	u32 scp_clk_con;
	u8 reserved3[4];
	u32 spm_swint;
	u32 spm_swint_set;
	u32 spm_swint_clr;
	u8 reserved4[20];
	u32 spm_cpu_wakeup_event;
	u32 spm_irq_mask;
	u8 reserved5[72];
	u32 md32pcm_scu_ctrl[4];
	u32 md32pcm_scu_sta0;
	u8 reserved6[20];
	u32 spm_irq_sta;
	u8 reserved7[4];
	u32 md32pcm_wakeup_sta;
	u32 md32pcm_event_sta;
	u8 reserved8[8];
	u32 spm_wakeup_misc;
	u8 reserved9[32];
	u32 spm_ck_sta;
	u8 reserved10[40];
	u32 md32pcm_sta;
	u32 md32pcm_pc;
	u8 reserved11[104];
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
	u8 reserved12[12];
	u32 spm_mcusys_pwr_con;
	u32 spm_cputop_pwr_con;
	u32 spm_cpu0_pwr_con;
	u32 spm_cpu1_pwr_con;
	u32 spm_cpu2_pwr_con;
	u32 spm_cpu3_pwr_con;
	u32 spm_cpu4_pwr_con;
	u32 spm_cpu5_pwr_con;
	u32 spm_cpu6_pwr_con;
	u32 spm_cpu7_pwr_con;
	u32 spm_mcupm_spmc_con;
	u8 reserved13[20];
	u32 spm_dpm_p2p_sta;
	u32 spm_dpm_p2p_con;
	u32 spm_dpm_intf_sta;
	u32 spm_dpm_wb_con;
	u32 spm_ack_chk_timer_3;
	u32 spm_ack_chk_sta_3;
	u8 reserved14[72];
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
	u32 spm_dpsw_con;
	u8 reserved16[68];
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
	u32 cirq_byoass_con;
	u32 aoc_vcore_sram_con;
	u8 reserved17[32];
	u32 reg_module_sw_cg_ddren_req_mask_0;
	u32 reg_module_sw_cg_ddren_req_mask_1;
	u32 reg_module_sw_cg_ddren_req_mask_2;
	u32 reg_module_sw_cg_ddren_req_mask_3;
	u32 reg_module_sw_cg_vrf18_req_mask_0;
	u32 reg_module_sw_cg_vrf18_req_mask_1;
	u32 reg_module_sw_cg_vrf18_req_mask_2;
	u32 reg_module_sw_cg_vrf18_req_mask_3;
	u32 reg_module_sw_cg_infra_req_mask_0;
	u32 reg_module_sw_cg_infra_req_mask_1;
	u32 reg_module_sw_cg_infra_req_mask_2;
	u32 reg_module_sw_cg_infra_req_mask_3;
	u32 reg_module_sw_cg_f26m_req_mask_0;
	u32 reg_module_sw_cg_f26m_req_mask_1;
	u32 reg_module_sw_cg_f26m_req_mask_2;
	u32 reg_module_sw_cg_f26m_req_mask_3;
	u32 reg_module_sw_cg_vcore_req_mask_0;
	u32 reg_module_sw_cg_vcore_req_mask_1;
	u32 reg_module_sw_cg_vcore_req_mask_2;
	u32 reg_module_sw_cg_vcore_req_mask_3;
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
	u32 reg_module_busy_msb_ddren_req_mask;
	u32 reg_module_busy_msb_vrf18_req_mask;
	u32 reg_module_busy_msb_infra_req_mask;
	u32 reg_module_busy_msb_f26m_req_mask;
	u32 reg_module_busy_msb_pmic_req_mask;
	u32 reg_module_busy_msb_vcore_req_mask;
	u8 reserved18[8];
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
	u32 spm_counter_0;
	u32 spm_counter_1;
	u32 spm_counter_2;
	u32 pcm_wdt_val;
	u32 pcm_wdt_out;
	u8 reserved19[80];
	u32 spm_sw_flag_0;
	u32 spm_sw_debug_0;
	u32 spm_sw_flag_1;
	u32 spm_sw_debug_1;
	u32 spm_sw_rsv[9];
	u32 spm_bk_wake_event;
	u32 spm_bk_vtcxo_dur;
	u32 spm_bk_wake_misc;
	u32 spm_bk_pcm_timer;
	u8 reserved20[12];
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
	u8 reserved21[8];
	u32 spm_sema_m0;
	u32 spm_sema_m1;
	u32 spm_sema_m2;
	u32 spm_sema_m3;
	u32 spm_sema_m4;
	u32 spm_sema_m5;
	u32 spm_sema_m6;
	u32 spm_sema_m7;
	u32 spm2adsp_mailbox;
	u32 adsp2spm_mailbox;
	u32 vcore_rtff_ctrl_mask_set;
	u32 vcore_rtff_ctrl_mask_clr;
	u32 spm2pmcu_mailbox_0;
	u32 spm2pmcu_mailbox_1;
	u32 spm2pmcu_mailbox_2;
	u32 spm2pmcu_mailbox_3;
	u32 pmcu2spm_mailbox_0;
	u32 pmcu2spm_mailbox_1;
	u32 pmcu2spm_mailbox_2;
	u32 pmcu2spm_mailbox_3;
	u32 spm2scp_mailbox;
	u32 scp2spm_mailbox;
	u32 scp_aov_bus_con;
	u32 vcore_rtff_ctrl_mask;
	u32 spm_sram_srclkeno_mask;
	u32 emi_pdn_req;
	u32 emi_busy_req;
	u32 emi_reserved_sta;
	u32 sc_univpll_div_rst_b;
	u32 eco_armpll_div_clock_off;
	u32 spm_mcdsr_cg_check_x1;
	u32 spm_sodi2_cg_check_x1;
	u8 reserved22[228];
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_event_mask;
	u32 spm_wakeup_event_ext_mask;
	u32 spm_wakeup_event_sens;
	u32 spm_wakeup_event_clear;
	u32 spm_src_req;
	u32 spm_src_mask_0;
	u32 spm_src_mask_1;
	u32 spm_src_mask_2;
	u32 spm_src_mask_3;
	u32 spm_src_mask_4;
	u32 spm_src_mask_5;
	u32 spm_src_mask_6;
	u32 spm_src_mask_7;
	u32 spm_src_mask_8;
	u32 spm_src_mask_9;
	u32 spm_src_mask_10;
	u32 spm_src_mask_11;
	u32 spm_src_mask_12;
	u32 src_req_sta_0;
	u32 src_req_sta_1;
	u32 src_req_sta_2;
	u32 src_req_sta_3;
	u32 src_req_sta_4;
	u32 src_req_sta_5;
	u32 src_req_sta_6;
	u32 src_req_sta_7;
	u32 src_req_sta_8;
	u32 src_req_sta_9;
	u32 src_req_sta_10;
	u32 src_req_sta_11;
	u32 src_req_sta_12;
	u32 spm_ipc_wakeup_req;
	u32 ipc_wakeup_req_mask_sta;
	u32 spm_event_con_misc;
	u32 ddren_dbc_con;
	u32 spm_resource_ack_con0;
	u32 spm_resource_ack_con1;
	u32 spm_resource_ack_mask0;
	u32 spm_resource_ack_mask1;
	u32 spm_resource_ack_mask2;
	u32 spm_resource_ack_mask3;
	u32 spm_resource_ack_mask4;
	u32 spm_resource_ack_mask5;
	u32 spm_resource_ack_mask6;
	u32 spm_event_counter_clear;
	u32 spm_vcore_event_count_sta;
	u32 spm_pmic_event_count_sta;
	u32 spm_srcclkena_event_count_sta;
	u32 spm_infra_event_count_sta;
	u32 spm_vrf18_event_count_sta;
	u32 spm_emi_event_count_sta;
	u32 spm_apsrc_event_count_sta;
	u32 spm_ddren_event_count_sta;
	u32 pcm_wdt_latch_0;
	u32 pcm_wdt_latch_1;
	u32 pcm_wdt_latch_2;
	u32 pcm_wdt_latch_3;
	u32 pcm_wdt_latch_4;
	u32 pcm_wdt_latch_5;
	u32 pcm_wdt_latch_6;
	u32 pcm_wdt_latch_7;
	u32 pcm_wdt_latch_8;
	u32 pcm_wdt_latch_9;
	u32 pcm_wdt_latch_10;
	u32 pcm_wdt_latch_11;
	u32 pcm_wdt_latch_12;
	u32 pcm_wdt_latch_13;
	u32 pcm_wdt_latch_14;
	u32 pcm_wdt_latch_15;
	u32 pcm_wdt_latch_16;
	u32 pcm_wdt_latch_17;
	u32 pcm_wdt_latch_18;
	u32 pcm_wdt_latch_19;
	u32 pcm_wdt_latch_20;
	u32 pcm_wdt_latch_21;
	u32 pcm_wdt_latch_22;
	u32 pcm_wdt_latch_23;
	u32 pcm_wdt_latch_24;
	u32 pcm_wdt_latch_25;
	u32 pcm_wdt_latch_26;
	u32 pcm_wdt_latch_27;
	u32 pcm_wdt_latch_28;
	u32 pcm_wdt_latch_29;
	u32 pcm_wdt_latch_30;
	u32 pcm_wdt_latch_31;
	u32 pcm_wdt_latch_32;
	u32 pcm_wdt_latch_33;
	u32 pcm_wdt_latch_34;
	u32 pcm_wdt_latch_35;
	u32 pcm_wdt_latch_36;
	u32 pcm_wdt_latch_37;
	u32 pcm_wdt_latch_38;
	u32 pcm_wdt_latch_39;
	u32 pcm_wdt_latch_40;
	u32 pcm_wdt_latch_spare_0;
	u32 pcm_wdt_latch_spare_1;
	u32 pcm_wdt_latch_spare_2;
	u32 pcm_wdt_latch_spare_3;
	u32 pcm_wdt_latch_spare_4;
	u32 pcm_wdt_latch_spare_5;
	u32 pcm_wdt_latch_spare_6;
	u32 pcm_wdt_latch_spare_7;
	u32 pcm_wdt_latch_spare_8;
	u32 pcm_wdt_latch_spare_9;
	u32 dramc_gating_err_latch_0;
	u32 dramc_gating_err_latch_1;
	u32 dramc_gating_err_latch_2;
	u32 dramc_gating_err_latch_3;
	u32 dramc_gating_err_latch_4;
	u32 dramc_gating_err_latch_5;
	u32 dramc_gating_err_latch_spare_0;
	u32 spm_debug_con;
	u32 spm_ack_chk_con_0;
	u32 spm_ack_chk_sel_0;
	u32 spm_ack_chk_timer_0;
	u32 spm_ack_chk_sta_0;
	u32 spm_ack_chk_con_1;
	u32 spm_ack_chk_sel_1;
	u32 spm_ack_chk_timer_1;
	u32 spm_ack_chk_sta_1;
	u32 spm_ack_chk_con_2;
	u32 spm_ack_chk_sel_2;
	u32 spm_ack_chk_timer_2;
	u32 spm_ack_chk_sta_2;
	u32 spm_ack_chk_con_3;
	u32 spm_ack_chk_sel_3;
	u8 reserved23[1024];
	u32 md1_pwr_con;
	u32 conn_pwr_con;
	u32 ifr_pwr_con;
	u32 peri_pwr_con;
	u32 ufs0_pwr_con;
	u32 ufs0_phy_pwr_con;
	u32 audio_pwr_con;
	u32 adsp_top_pwr_con;
	u32 adsp_infra_pwr_con;
	u32 adsp_ao_pwr_con;
	u32 isp_img1_pwr_con;
	u32 isp_img2_pwr_con;
	u32 isp_ipe_pwr_con;
	u32 isp_vcore_pwr_con;
	u32 vde0_pwr_con;
	u32 vde1_pwr_con;
	u32 ven0_pwr_con;
	u32 ven1_pwr_con;
	u32 cam_main_pwr_con;
	u32 cam_mraw_pwr_con;
	u32 cam_suba_pwr_con;
	u32 cam_subb_pwr_con;
	u32 cam_subc_pwr_con;
	u32 cam_vcore_pwr_con;
	u32 cam_ccu_pwr_con;
	u32 cam_ccu_ao_pwr_con;
	u32 mdp0_pwr_con;
	u32 mdp1_pwr_con;
	u32 dis0_pwr_con;
	u32 dis1_pwr_con;
	u32 mm_infra_pwr_con;
	u32 mm_proc_pwr_con;
	u32 dp_tx_pwr_con;
	u32 scp_core_pwr_con;
	u32 scp_peri_pwr_con;
	u32 dpm0_pwr_con;
	u32 dpm1_pwr_con;
	u32 emi0_pwr_con;
	u32 emi1_pwr_con;
	u32 csi_rx_pwr_con;
	u32 ssrsys_pwr_con;
	u32 sspm_pwr_con;
	u32 ssusb_pwr_con;
	u32 ssusb_phy_pwr_con;
	u32 cpueb_pwr_con;
	u32 mfg0_pwr_con;
	u32 mfg1_pwr_con;
	u32 mfg2_pwr_con;
	u32 mfg3_pwr_con;
	u32 mfg4_pwr_con;
	u32 mfg5_pwr_con;
	u32 mfg6_pwr_con;
	u32 mfg7_pwr_con;
	u32 adsp_hre_sram_con;
	u32 ccu_sleep_sram_con;
	u32 efuse_sram_con;
	u32 emi_hre_sram_con;
	u32 emi_slb_sram_con;
	u32 infra_hre_sram_con;
	u32 infra_sleep_sram_con;
	u32 mm_hre_sram_con;
	u32 nth_emi_slb_sram_con;
	u32 nth_emi_slb_sram_ack;
	u32 peri_sleep_sram_con;
	u32 spm_sram_con;
	u32 sspm_sram_con;
	u32 ssr_sleep_sram_con;
	u32 sth_emi_slb_sram_con;
	u32 sth_emi_slb_sram_ack;
	u32 ufs_pdn_sram_con;
	u32 ufs_sleep_sram_con;
	u32 unipro_pdn_sram_con;
	u32 cpu_buck_iso_con;
	u32 md_buck_iso_con;
	u32 soc_buck_iso_con;
	u32 soc_buck_iso_con_set;
	u32 soc_buck_iso_con_clr;
	u32 soc_buck_iso_con_2;
	u32 soc_buck_iso_con_2_set;
	u32 soc_buck_iso_con_2_clr;
	u32 pwr_status;
	u32 pwr_status_2nd;
	u32 pwr_status_msb;
	u32 pwr_status_msb_2nd;
	u32 xpu_pwr_status;
	u32 xpu_pwr_status_2nd;
	u32 dfd_soc_pwr_latch;
	u32 subsys_pm_bypass;
	u32 vadsp_hre_sram_con;
	u32 vadsp_hre_sram_ack;
	u32 gcpu_sram_con;
	u32 gcpu_sram_ack;
	u32 edp_tx_pwr_con;
	u32 pcie_pwr_con;
	u32 pcie_phy_pwr_con;
	u8 reserved24[4];
	u32 spm_twam_con;
	u32 spm_twam_window_len;
	u32 spm_twam_idle_sel;
	u32 spm_twam_last_sta0;
	u32 spm_twam_last_sta1;
	u32 spm_twam_last_sta2;
	u32 spm_twam_last_sta3;
	u32 spm_twam_curr_sta0;
	u32 spm_twam_curr_sta1;
	u32 spm_twam_curr_sta2;
	u32 spm_twam_curr_sta3;
	u32 spm_twam_timer_out;
};

check_member(mtk_spm_regs, ap_mdsrc_req, 0x404);
check_member(mtk_spm_regs, ulposc_con, 0x400);
check_member(mtk_spm_regs, conn_pwr_con, 0x0E04);
check_member(mtk_spm_regs, ufs0_pwr_con, 0x0E10);
check_member(mtk_spm_regs, audio_pwr_con, 0xe18);
check_member(mtk_spm_regs, vde0_pwr_con, 0x0E38);
check_member(mtk_spm_regs, ven0_pwr_con, 0x0E40);
check_member(mtk_spm_regs, cam_main_pwr_con, 0x0E48);
check_member(mtk_spm_regs, cam_suba_pwr_con, 0x0E50);
check_member(mtk_spm_regs, mdp0_pwr_con, 0x0E68);
check_member(mtk_spm_regs, dis0_pwr_con, 0x0E70);
check_member(mtk_spm_regs, mm_infra_pwr_con, 0x0E78);
check_member(mtk_spm_regs, dp_tx_pwr_con, 0x0E80);
check_member(mtk_spm_regs, scp_peri_pwr_con, 0x0E88);
check_member(mtk_spm_regs, csi_rx_pwr_con, 0x0E9C);
check_member(mtk_spm_regs, ssusb_pwr_con, 0x0EA8);
check_member(mtk_spm_regs, mfg0_pwr_con, 0x0EB4);
check_member(mtk_spm_regs, mfg3_pwr_con, 0x0EC0);
check_member(mtk_spm_regs, pwr_status, 0x0F40);
check_member(mtk_spm_regs, edp_tx_pwr_con, 0x0F70);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

static const struct power_domain_data disp[] = {
	{
		/* dis0 */
		.pwr_con = &mtk_spm->dis0_pwr_con,
		.pwr_sta_mask = BIT(28),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
	{
		/* edp_tx */
		.pwr_con = &mtk_spm->edp_tx_pwr_con,
		.pwr_status = &mtk_spm->pwr_status_msb,
		.pwr_status_2nd = &mtk_spm->pwr_status_msb_2nd,
		.pwr_sta_mask = BIT(12),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
		.caps = SCPD_SRAM_ISO,
	},
};

static const struct power_domain_data audio[] = {
	{
		/* adsp_ao */
		.pwr_con = &mtk_spm->adsp_ao_pwr_con,
		.pwr_sta_mask = BIT(9),
	},
	{
		/* audio */
		.pwr_con = &mtk_spm->audio_pwr_con,
		.pwr_sta_mask = BIT(6),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
};

#endif  /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_H__ */
