/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_SPM_H
#define SOC_MEDIATEK_MT8192_SPM_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <soc/spm_common.h>
#include <types.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE                    0xb16
#define SPM_REGWR_CFG_KEY                   (SPM_PROJECT_CODE << 16)

/* POWERON_CONFIG_EN (0x10006000+0x000) */
#define BCLK_CG_EN_LSB                      (1U << 0)       /* 1b */

/* SPM_CLK_CON (0x10006000+0x00C) */
#define REG_SYSCLK1_SRC_MD2_SRCCLKENA       (1U << 28)      /* 1b */

/* PCM_CON0 (0x10006000+0x018) */
#define PCM_CK_EN_LSB                       (1U << 2)       /* 1b */
#define PCM_SW_RESET_LSB                    (1U << 15)      /* 1b */

/* PCM_CON1 (0x10006000+0x01C) */
#define RG_IM_SLAVE_LSB                     (1U << 0)       /* 1b */
#define RG_AHBMIF_APBEN_LSB                 (1U << 3)       /* 1b */
#define RG_PCM_TIMER_EN_LSB                 (1U << 5)       /* 1b */
#define SPM_EVENT_COUNTER_CLR_LSB           (1U << 6)       /* 1b */
#define RG_PCM_WDT_WAKE_LSB                 (1U << 9)       /* 1b */
#define REG_SPM_SRAM_ISOINT_B_LSB           (1U << 11)      /* 1b */
#define REG_EVENT_LOCK_EN_LSB               (1U << 12)      /* 1b */
#define REG_MD32_APB_INTERNAL_EN_LSB        (1U << 14)      /* 1b */

/* SPM_WAKEUP_EVENT_MASK (0x10006000+0x0D0) */
#define SPM_WAKEUP_EVENT_MASK_BIT0          (1U << 0)       /* 1b */
#define SPM_WAKEUP_EVENT_MASK_CSYSPWREQ_B   (1U << 24)      /* 1b */

/* DDR_EN_DBC_CON1 (0x10006000+0x0EC) */
#define REG_ALL_DDR_EN_DBC_EN_LSB           (1U << 0)       /* 1b */

/* SPM_DVFS_MISC (0x10006000+0x4AC) */
#define SPM_DVFS_FORCE_ENABLE_LSB           (1U << 2)       /* 1b */
#define SPM_DVFSRC_ENABLE_LSB               (1U << 4)       /* 1b */

/* SPM_SW_FLAG_0 (0x10006000+0x600) */
#define SPM_FLAG_DISABLE_VCORE_DVS          (1U << 3)       /* 1b */
#define SPM_FLAG_DISABLE_VCORE_DFS          (1U << 4)       /* 1b */
#define SPM_FLAG_RUN_COMMON_SCENARIO        (1U << 10)      /* 1b */

/* SYS_TIMER_CON (0x10006000+0x98C) */
#define SYS_TIMER_START_EN_LSB              (1U << 0)       /* 1b */

/* MD32PCM_CFGREG_SW_RSTN (0x10006000+0xA00) */
DEFINE_BIT(MD32PCM_CFGREG_SW_RSTN_RESET, 0)

/**************************************
 * Config and Parameter
 **************************************/
#define POWER_ON_VAL1_DEF                   0x80015860
#define SPM_WAKEUP_EVENT_MASK_DEF           0xefffffff
#define SPM_ACK_CHK_3_SEL_HW_S1             0x00350098
#define SPM_ACK_CHK_3_HW_S1_CNT             0x1
#define SPM_ACK_CHK_3_CON_HW_MODE_TRIG      0x800
#define SPM_ACK_CHK_3_CON_EN                0x110
#define SPM_ACK_CHK_3_CON_CLR_ALL           0x2
#define SPM_BUS_PROTECT_MASK_B_DEF          0xffffffff
#define SPM_BUS_PROTECT2_MASK_B_DEF         0xffffffff
#define MD32PCM_DMA0_CON_VAL                0x0003820e
#define MD32PCM_DMA0_START_VAL              0x00008000
#define MD32PCM_CFGREG_SW_RSTN_RUN          0x1
#define SPM_DVFS_LEVEL_DEF                  0x00000001
#define SPM_DVS_DFS_LEVEL_DEF               0x00010001
#define SPM_RESOURCE_ACK_CON0_DEF           0x00000000
#define SPM_RESOURCE_ACK_CON1_DEF           0x00000000
#define SPM_RESOURCE_ACK_CON2_DEF           0xcccc4e4e
#define SPM_RESOURCE_ACK_CON3_DEF           0x00000000
#define ARMPLL_CLK_SEL_DEF                  0x3ff
#define DDR_EN_DBC_CON0_DEF                 0x154
#define SPM_SYSCLK_SETTLE                   0x60fe
#define SPM_INIT_DONE_US                    20
#define PCM_WDT_TIMEOUT                     (30 * 32768)
#define PCM_TIMER_MAX                       (0xffffffff - PCM_WDT_TIMEOUT)

/**************************************
 * Define and Declare
 **************************************/
/* SPM_IRQ_MASK */
#define ISRM_TWAM                           (1U << 2)
#define ISRM_PCM_RETURN                     (1U << 3)
#define ISRM_RET_IRQ_AUX                    0x3fe00
#define ISRM_ALL_EXC_TWAM                   (ISRM_RET_IRQ_AUX)
#define ISRM_ALL                            (ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM                           (1U << 2)
#define ISRS_PCM_RETURN                     (1U << 3)
#define ISRC_TWAM                           ISRS_TWAM
#define ISRC_ALL_EXC_TWAM                   ISRS_PCM_RETURN
#define ISRC_ALL                            (ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* PCM_PWR_IO_EN */
#define PCM_PWRIO_EN_R0                     (1U << 0)
#define PCM_PWRIO_EN_R7                     (1U << 7)
#define PCM_RF_SYNC_R0                      (1U << 16)
#define PCM_RF_SYNC_R6                      (1U << 22)
#define PCM_RF_SYNC_R7                      (1U << 23)

/* SPM_SWINT */
#define PCM_SW_INT_ALL                      0x3ff

struct pwr_ctrl {
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
	u8 reg_srcclken0_ctl;
	u8 reg_srcclken1_ctl;
	u8 reg_spm_lock_infra_dcm;
	u8 reg_srcclken_mask;
	u8 reg_md1_c32rm_en;
	u8 reg_md2_c32rm_en;
	u8 reg_clksq0_sel_ctrl;
	u8 reg_clksq1_sel_ctrl;
	u8 reg_srcclken0_en;
	u8 reg_srcclken1_en;
	u32 reg_sysclk0_src_mask_b;
	u32 reg_sysclk1_src_mask_b;

	/* SPM_AP_STANDBY_CON */
	u8 reg_wfi_op;
	u8 reg_wfi_type;
	u8 reg_mp0_cputop_idle_mask;
	u8 reg_mp1_cputop_idle_mask;
	u8 reg_mcusys_idle_mask;
	u8 reg_md_apsrc_1_sel;
	u8 reg_md_apsrc_0_sel;
	u8 reg_conn_apsrc_sel;

	/* SPM_SRC6_MASK */
	u8 reg_dpmaif_srcclkena_mask_b;
	u8 reg_dpmaif_infra_req_mask_b;
	u8 reg_dpmaif_apsrc_req_mask_b;
	u8 reg_dpmaif_vrf18_req_mask_b;
	u8 reg_dpmaif_ddr_en_mask_b;

	/* SPM_SRC_REQ */
	u8 reg_spm_apsrc_req;
	u8 reg_spm_f26m_req;
	u8 reg_spm_infra_req;
	u8 reg_spm_vrf18_req;
	u8 reg_spm_ddr_en_req;
	u8 reg_spm_dvfs_req;
	u8 reg_spm_sw_mailbox_req;
	u8 reg_spm_sspm_mailbox_req;
	u8 reg_spm_adsp_mailbox_req;
	u8 reg_spm_scp_mailbox_req;

	/* SPM_SRC_MASK */
	u8 reg_md_srcclkena_0_mask_b;
	u8 reg_md_srcclkena2infra_req_0_mask_b;
	u8 reg_md_apsrc2infra_req_0_mask_b;
	u8 reg_md_apsrc_req_0_mask_b;
	u8 reg_md_vrf18_req_0_mask_b;
	u8 reg_md_ddr_en_0_mask_b;
	u8 reg_md_srcclkena_1_mask_b;
	u8 reg_md_srcclkena2infra_req_1_mask_b;
	u8 reg_md_apsrc2infra_req_1_mask_b;
	u8 reg_md_apsrc_req_1_mask_b;
	u8 reg_md_vrf18_req_1_mask_b;
	u8 reg_md_ddr_en_1_mask_b;
	u8 reg_conn_srcclkena_mask_b;
	u8 reg_conn_srcclkenb_mask_b;
	u8 reg_conn_infra_req_mask_b;
	u8 reg_conn_apsrc_req_mask_b;
	u8 reg_conn_vrf18_req_mask_b;
	u8 reg_conn_ddr_en_mask_b;
	u8 reg_conn_vfe28_mask_b;
	u8 reg_srcclkeni0_srcclkena_mask_b;
	u8 reg_srcclkeni0_infra_req_mask_b;
	u8 reg_srcclkeni1_srcclkena_mask_b;
	u8 reg_srcclkeni1_infra_req_mask_b;
	u8 reg_srcclkeni2_srcclkena_mask_b;
	u8 reg_srcclkeni2_infra_req_mask_b;
	u8 reg_infrasys_apsrc_req_mask_b;
	u8 reg_infrasys_ddr_en_mask_b;
	u8 reg_md32_srcclkena_mask_b;
	u8 reg_md32_infra_req_mask_b;
	u8 reg_md32_apsrc_req_mask_b;
	u8 reg_md32_vrf18_req_mask_b;
	u8 reg_md32_ddr_en_mask_b;

	/* SPM_SRC2_MASK */
	u8 reg_scp_srcclkena_mask_b;
	u8 reg_scp_infra_req_mask_b;
	u8 reg_scp_apsrc_req_mask_b;
	u8 reg_scp_vrf18_req_mask_b;
	u8 reg_scp_ddr_en_mask_b;
	u8 reg_audio_dsp_srcclkena_mask_b;
	u8 reg_audio_dsp_infra_req_mask_b;
	u8 reg_audio_dsp_apsrc_req_mask_b;
	u8 reg_audio_dsp_vrf18_req_mask_b;
	u8 reg_audio_dsp_ddr_en_mask_b;
	u8 reg_ufs_srcclkena_mask_b;
	u8 reg_ufs_infra_req_mask_b;
	u8 reg_ufs_apsrc_req_mask_b;
	u8 reg_ufs_vrf18_req_mask_b;
	u8 reg_ufs_ddr_en_mask_b;
	u8 reg_disp0_apsrc_req_mask_b;
	u8 reg_disp0_ddr_en_mask_b;
	u8 reg_disp1_apsrc_req_mask_b;
	u8 reg_disp1_ddr_en_mask_b;
	u8 reg_gce_infra_req_mask_b;
	u8 reg_gce_apsrc_req_mask_b;
	u8 reg_gce_vrf18_req_mask_b;
	u8 reg_gce_ddr_en_mask_b;
	u8 reg_apu_srcclkena_mask_b;
	u8 reg_apu_infra_req_mask_b;
	u8 reg_apu_apsrc_req_mask_b;
	u8 reg_apu_vrf18_req_mask_b;
	u8 reg_apu_ddr_en_mask_b;
	u8 reg_cg_check_srcclkena_mask_b;
	u8 reg_cg_check_apsrc_req_mask_b;
	u8 reg_cg_check_vrf18_req_mask_b;
	u8 reg_cg_check_ddr_en_mask_b;

	/* SPM_SRC3_MASK */
	u8 reg_dvfsrc_event_trigger_mask_b;
	u8 reg_sw2spm_int0_mask_b;
	u8 reg_sw2spm_int1_mask_b;
	u8 reg_sw2spm_int2_mask_b;
	u8 reg_sw2spm_int3_mask_b;
	u8 reg_sc_adsp2spm_wakeup_mask_b;
	u8 reg_sc_sspm2spm_wakeup_mask_b;
	u8 reg_sc_scp2spm_wakeup_mask_b;
	u8 reg_csyspwrreq_mask;
	u8 reg_spm_srcclkena_reserved_mask_b;
	u8 reg_spm_infra_req_reserved_mask_b;
	u8 reg_spm_apsrc_req_reserved_mask_b;
	u8 reg_spm_vrf18_req_reserved_mask_b;
	u8 reg_spm_ddr_en_reserved_mask_b;
	u8 reg_mcupm_srcclkena_mask_b;
	u8 reg_mcupm_infra_req_mask_b;
	u8 reg_mcupm_apsrc_req_mask_b;
	u8 reg_mcupm_vrf18_req_mask_b;
	u8 reg_mcupm_ddr_en_mask_b;
	u8 reg_msdc0_srcclkena_mask_b;
	u8 reg_msdc0_infra_req_mask_b;
	u8 reg_msdc0_apsrc_req_mask_b;
	u8 reg_msdc0_vrf18_req_mask_b;
	u8 reg_msdc0_ddr_en_mask_b;
	u8 reg_msdc1_srcclkena_mask_b;
	u8 reg_msdc1_infra_req_mask_b;
	u8 reg_msdc1_apsrc_req_mask_b;
	u8 reg_msdc1_vrf18_req_mask_b;
	u8 reg_msdc1_ddr_en_mask_b;

	/* SPM_SRC4_MASK */
	u32 ccif_event_mask_b;
	u8 reg_bak_psri_srcclkena_mask_b;
	u8 reg_bak_psri_infra_req_mask_b;
	u8 reg_bak_psri_apsrc_req_mask_b;
	u8 reg_bak_psri_vrf18_req_mask_b;
	u8 reg_bak_psri_ddr_en_mask_b;
	u8 reg_dramc0_md32_infra_req_mask_b;
	u8 reg_dramc0_md32_vrf18_req_mask_b;
	u8 reg_dramc1_md32_infra_req_mask_b;
	u8 reg_dramc1_md32_vrf18_req_mask_b;
	u8 reg_conn_srcclkenb2pwrap_mask_b;
	u8 reg_dramc0_md32_wakeup_mask;
	u8 reg_dramc1_md32_wakeup_mask;

	/* SPM_SRC5_MASK */
	u32 reg_mcusys_merge_apsrc_req_mask_b;
	u32 reg_mcusys_merge_ddr_en_mask_b;
	u8 reg_msdc2_srcclkena_mask_b;
	u8 reg_msdc2_infra_req_mask_b;
	u8 reg_msdc2_apsrc_req_mask_b;
	u8 reg_msdc2_vrf18_req_mask_b;
	u8 reg_msdc2_ddr_en_mask_b;
	u8 reg_pcie_srcclkena_mask_b;
	u8 reg_pcie_infra_req_mask_b;
	u8 reg_pcie_apsrc_req_mask_b;
	u8 reg_pcie_vrf18_req_mask_b;
	u8 reg_pcie_ddr_en_mask_b;

	/* SPM_WAKEUP_EVENT_MASK */
	u32 reg_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	u32 reg_ext_wakeup_event_mask;

	/* Auto-gen End */
};

enum {
	DISP_PWR_STA_MASK	= 0x1 << 20,
	DISP_SRAM_PDN_MASK	= 0x1 << 8,
	DISP_SRAM_ACK_MASK	= 0x1 << 12,
	AUDIO_PWR_STA_MASK	= 0x1 << 21,
	AUDIO_SRAM_PDN_MASK	= 0x1 << 8,
	AUDIO_SRAM_ACK_MASK	= 0x1 << 12,
};

struct mtk_spm_regs {
	u32 poweron_config_set;
	u32 spm_power_on_val0;
	u32 spm_power_on_val1;
	u32 spm_clk_con;
	u32 spm_clk_settle;
	u32 spm_ap_standby_con;
	u32 pcm_con0;
	u32 pcm_con1;
	u32 spm_power_on_val2;
	u32 spm_power_on_val3;
	u32 pcm_reg_data_ini;
	u32 pcm_pwr_io_en;
	u32 pcm_timer_val;
	u32 pcm_wdt_val;
	u32 spm_src6_mask;
	u32 reserved0;
	u32 spm_sw_rst_con;
	u32 spm_sw_rst_con_set;
	u32 spm_sw_rst_con_clr;
	u32 vs1_psr_mask_b;
	u32 vs2_psr_mask_b;
	u32 reserved1[12];
	u32 md32_clk_con;
	u32 spm_sram_rsv_con;
	u32 spm_swint;
	u32 spm_swint_set;
	u32 spm_swint_clr;
	u32 spm_scp_mailbox;
	u32 scp_spm_mailbox;
	u32 spm_twam_con;
	u32 spm_twam_window_len;
	u32 spm_twam_idle_sel;
	u32 spm_scp_irq;
	u32 spm_cpu_wakeup_event;
	u32 spm_irq_mask;
	u32 spm_src_req;
	u32 spm_src_mask;
	u32 spm_src2_mask;
	u32 spm_src3_mask;
	u32 spm_src4_mask;
	u32 spm_src5_mask;
	u32 spm_wakeup_event_mask;
	u32 spm_wakeup_event_ext_mask;
	u32 spm_twam_event_clear;
	u32 scp_clk_con;
	u32 pcm_debug_con;
	u32 ahb_bus_con;
	u32 ddr_en_dbc_con0;
	u32 ddr_en_dbc_con1;
	u32 spm_resource_ack_con0;
	u32 spm_resource_ack_con1;
	u32 spm_resource_ack_con2;
	u32 spm_resource_ack_con3;
	u32 pcm_reg0_data;
	u32 pcm_reg2_data;
	u32 pcm_reg6_data;
	u32 pcm_reg7_data;
	u32 pcm_reg13_data;
	u32 src_req_sta0;
	u32 src_req_sta1;
	u32 src_req_sta2;
	u32 pcm_timer_out;
	u32 pcm_wdt_out;
	u32 spm_irq_sta;
	u32 src_req_sta4;
	u32 md32pcm_wakeup_sta;
	u32 md32pcm_event_sta;
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_misc;
	u32 mm_dvfs_halt;
	u32 reserved2[2];
	u32 bus_protect_rdy;
	u32 bus_protect1_rdy;
	u32 bus_protect2_rdy;
	u32 bus_protect3_rdy;
	u32 subsys_idle_sta;
	u32 pcm_sta;
	u32 src_req_sta3;
	u32 pwr_status;
	u32 pwr_status_2nd;
	u32 cpu_pwr_status;
	u32 other_pwr_status;
	u32 spm_vtcxo_event_count_sta;
	u32 spm_infra_event_count_sta;
	u32 spm_vrf18_event_count_sta;
	u32 spm_apsrc_event_count_sta;
	u32 spm_ddren_event_count_sta;
	u32 md32pcm_sta;
	u32 md32pcm_pc;
	u32 reserved3[3];
	u32 dvfsrc_event_sta;
	u32 bus_protect4_rdy;
	u32 bus_protect5_rdy;
	u32 bus_protect6_rdy;
	u32 bus_protect7_rdy;
	u32 bus_protect8_rdy;
	u32 reserved4[5];
	u32 spm_twam_last_sta0;
	u32 spm_twam_last_sta1;
	u32 spm_twam_last_sta2;
	u32 spm_twam_last_sta3;
	u32 spm_twam_curr_sta0;
	u32 spm_twam_curr_sta1;
	u32 spm_twam_curr_sta2;
	u32 spm_twam_curr_sta3;
	u32 spm_twam_timer_out;
	u32 spm_cg_check_sta;
	u32 spm_dvfs_sta;
	u32 spm_dvfs_opp_sta;
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
	u32 reserved5;
	u32 armpll_clk_con;
	u32 mcusys_idle_sta;
	u32 gic_wakeup_sta;
	u32 cpu_spare_con;
	u32 cpu_spare_con_set;
	u32 cpu_spare_con_clr;
	u32 armpll_clk_sel;
	u32 ext_int_wakeup_req;
	u32 ext_int_wakeup_req_set;
	u32 ext_int_wakeup_req_clr;
	u32 reserved6[3];
	u32 mp0_cpu0_irq_mask;
	u32 mp0_cpu1_irq_mask;
	u32 mp0_cpu2_irq_mask;
	u32 mp0_cpu3_irq_mask;
	u32 mp1_cpu0_irq_mask;
	u32 mp1_cpu1_irq_mask;
	u32 mp1_cpu2_irq_mask;
	u32 mp1_cpu3_irq_mask;
	u32 mp0_cpu0_wfi_en;
	u32 mp0_cpu1_wfi_en;
	u32 mp0_cpu2_wfi_en;
	u32 mp0_cpu3_wfi_en;
	u32 mp0_cpu4_wfi_en;
	u32 mp0_cpu5_wfi_en;
	u32 mp0_cpu6_wfi_en;
	u32 mp0_cpu7_wfi_en;
	u32 root_cputop_addr;
	u32 root_core_addr;
	u32 reserved7[10];
	u32 spm2sw_mailbox_0;
	u32 spm2sw_mailbox_1;
	u32 spm2sw_mailbox_2;
	u32 spm2sw_mailbox_3;
	u32 sw2spm_int;
	u32 sw2spm_int_set;
	u32 sw2spm_int_clr;
	u32 sw2spm_mailbox_0;
	u32 sw2spm_mailbox_1;
	u32 sw2spm_mailbox_2;
	u32 sw2spm_mailbox_3;
	u32 sw2spm_cfg;
	u32 md1_pwr_con;
	u32 conn_pwr_con;
	u32 mfg0_pwr_con;
	u32 mfg1_pwr_con;
	u32 mfg2_pwr_con;
	u32 mfg3_pwr_con;
	u32 mfg4_pwr_con;
	u32 mfg5_pwr_con;
	u32 mfg6_pwr_con;
	u32 ifr_pwr_con;
	u32 ifr_sub_pwr_con;
	u32 dpy_pwr_con;
	u32 isp_pwr_con;
	u32 isp2_pwr_con;
	u32 ipe_pwr_con;
	u32 vde_pwr_con;
	u32 vde2_pwr_con;
	u32 ven_pwr_con;
	u32 ven_core1_pwr_con;
	u32 mdp_pwr_con;
	u32 dis_pwr_con;
	u32 audio_pwr_con;
	u32 adsp_pwr_con;
	u32 cam_pwr_con;
	u32 cam_rawa_pwr_con;
	u32 cam_rawb_pwr_con;
	u32 cam_rawc_pwr_con;
	u32 sysram_con;
	u32 sysrom_con;
	u32 sspm_sram_con;
	u32 scp_sram_con;
	u32 dpy_shu_sram_con;
	u32 ufs_sram_con;
	u32 devapc_ifr_sram_con;
	u32 devapc_subifr_sram_con;
	u32 devapc_acp_sram_con;
	u32 usb_sram_con;
	u32 dummy_sram_con;
	u32 md_ext_buck_iso_con;
	u32 ext_buck_iso;
	u32 dxcc_sram_con;
	u32 msdc_pwr_con;
	u32 debugtop_sram_con;
	u32 dp_tx_pwr_con;
	u32 dpmaif_sram_con;
	u32 dpy_shu2_con;
	u32 dramc_mcu2_sram_con;
	u32 dramc_mcu_sram_con;
	u32 mcupm_pwr_con;
	u32 dpy2_pwr_con;
	u32 peri_pwr_con;
	u32 reserved8[13];
	u32 spm_mem_ck_sel;
	u32 spm_bus_protect_mask_b;
	u32 spm_bus_protect1_mask_b;
	u32 spm_bus_protect2_mask_b;
	u32 spm_bus_protect3_mask_b;
	u32 spm_bus_protect4_mask_b;
	u32 spm_emi_bw_mode;
	u32 ap2md_peer_wakeup;
	u32 ulposc_con;
	u32 spm2mm_con;
	u32 spm_bus_protect5_mask_b;
	u32 spm2mcupm_con;
	u32 ap_mdsrc_req;
	u32 spm2emi_enter_ulpm;
	u32 spm2md_dvfs_con;
	u32 md2spm_dvfs_con;
	u32 spm_bus_protect6_mask_b;
	u32 spm_bus_protect7_mask_b;
	u32 spm_bus_protect8_mask_b;
	u32 spm_pll_con;
	u32 cpu_dvfs_req;
	u32 spm_dram_mcu_sw_con0;
	u32 spm_dram_mcu_sw_con1;
	u32 spm_dram_mcu_sw_con2;
	u32 spm_dram_mcu_sw_con3;
	u32 spm_dram_mcu_sw_con4;
	u32 spm_dram_mcu_sta_0;
	u32 spm_dram_mcu_sta_1;
	u32 spm_dram_mcu_sta_2;
	u32 spm_dram_mcu_sw_sel_0;
	u32 relay_dvfs_level;
	u32 reserved9;
	u32 dramc_dpy_clk_sw_con_0;
	u32 dramc_dpy_clk_sw_con_1;
	u32 dramc_dpy_clk_sw_con_2;
	u32 dramc_dpy_clk_sw_con_3;
	u32 dramc_dpy_clk_sw_sel_0;
	u32 dramc_dpy_clk_sw_sel_1;
	u32 dramc_dpy_clk_sw_sel_2;
	u32 dramc_dpy_clk_sw_sel_3;
	u32 dramc_dpy_clk_spm_con;
	u32 spm_dvfs_level;
	u32 spm_cirq_con;
	u32 spm_dvfs_misc;
	u32 spm_vs1_vs2_rc_con;
	u32 rg_module_sw_cg_0_mask_req_0;
	u32 rg_module_sw_cg_0_mask_req_1;
	u32 rg_module_sw_cg_0_mask_req_2;
	u32 rg_module_sw_cg_1_mask_req_0;
	u32 rg_module_sw_cg_1_mask_req_1;
	u32 rg_module_sw_cg_1_mask_req_2;
	u32 rg_module_sw_cg_2_mask_req_0;
	u32 rg_module_sw_cg_2_mask_req_1;
	u32 rg_module_sw_cg_2_mask_req_2;
	u32 rg_module_sw_cg_3_mask_req_0;
	u32 rg_module_sw_cg_3_mask_req_1;
	u32 rg_module_sw_cg_3_mask_req_2;
	u32 pwr_status_mask_req_0;
	u32 pwr_status_mask_req_1;
	u32 pwr_status_mask_req_2;
	u32 spm_cg_check_con;
	u32 spm_src_rdy_sta;
	u32 spm_dvs_dfs_level;
	u32 spm_force_dvfs;
	u32 reserved10[64];
	u32 spm_sw_flag_0;
	u32 spm_sw_debug_0;
	u32 spm_sw_flag_1;
	u32 reserved11[8];
	u32 spm_sw_rsv_7;
	u32 spm_sw_rsv_8;
	u32 reserved12[203];
	u32 spm_ack_chk_con_3;
	u32 spm_ack_chk_pc_3;
	u32 spm_ack_chk_sel_3;
	u32 spm_ack_chk_timer_3;
	u32 reserved13[7];
	u32 sys_timer_con;
	u32 reserved14[28];
	u32 md32pcm_cfgreg_sw_rstn;
	u32 reserved15[127];
	u32 md32pcm_dma0_src;
	u32 md32pcm_dma0_dst;
	u32 md32pcm_dma0_wppt;
	u32 md32pcm_dma0_wpto;
	u32 md32pcm_dma0_count;
	u32 md32pcm_dma0_con;
	u32 md32pcm_dma0_start;
	u32 reserved16[2];
	u32 md32pcm_dma0_rlct;
};
check_member(mtk_spm_regs, poweron_config_set, 0x0);
check_member(mtk_spm_regs, vs2_psr_mask_b, 0x50);
check_member(mtk_spm_regs, md32_clk_con, 0x84);
check_member(mtk_spm_regs, mm_dvfs_halt, 0x144);
check_member(mtk_spm_regs, bus_protect_rdy, 0x150);
check_member(mtk_spm_regs, md32pcm_pc, 0x194);
check_member(mtk_spm_regs, dvfsrc_event_sta, 0x1a4);
check_member(mtk_spm_regs, bus_protect8_rdy, 0x1b8);
check_member(mtk_spm_regs, spm_twam_last_sta0, 0x1d0);
check_member(mtk_spm_regs, ext_int_wakeup_req_clr, 0x250);
check_member(mtk_spm_regs, mp0_cpu0_irq_mask, 0x260);
check_member(mtk_spm_regs, root_core_addr, 0x2a4);
check_member(mtk_spm_regs, spm2sw_mailbox_0, 0x2d0);
check_member(mtk_spm_regs, peri_pwr_con, 0x3c8);
check_member(mtk_spm_regs, spm_mem_ck_sel, 0x400);
check_member(mtk_spm_regs, ulposc_con, 0x420);
check_member(mtk_spm_regs, spm_force_dvfs, 0x4fc);
check_member(mtk_spm_regs, spm_sw_flag_0, 0x600);
check_member(mtk_spm_regs, spm_sw_flag_1, 0x608);
check_member(mtk_spm_regs, spm_sw_rsv_7, 0x62c);
check_member(mtk_spm_regs, spm_sw_rsv_8, 0x630);
check_member(mtk_spm_regs, spm_ack_chk_con_3, 0x960);
check_member(mtk_spm_regs, spm_ack_chk_timer_3, 0x96c);
check_member(mtk_spm_regs, sys_timer_con, 0x98c);
check_member(mtk_spm_regs, md32pcm_cfgreg_sw_rstn, 0xa00);
check_member(mtk_spm_regs, md32pcm_dma0_src, 0xc00);
check_member(mtk_spm_regs, md32pcm_dma0_dst, 0xc04);
check_member(mtk_spm_regs, md32pcm_dma0_wppt, 0xc08);
check_member(mtk_spm_regs, md32pcm_dma0_wpto, 0xc0c);
check_member(mtk_spm_regs, md32pcm_dma0_count, 0xc10);
check_member(mtk_spm_regs, md32pcm_dma0_con, 0xc14);
check_member(mtk_spm_regs, md32pcm_dma0_start, 0xc18);
check_member(mtk_spm_regs, md32pcm_dma0_rlct, 0xc24);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

static const struct power_domain_data disp[] = {
	{
		.pwr_con = &mtk_spm->dis_pwr_con,
		.pwr_sta_mask = DISP_PWR_STA_MASK,
		.sram_pdn_mask = DISP_SRAM_PDN_MASK,
		.sram_ack_mask = DISP_SRAM_ACK_MASK,
	},
};

static const struct power_domain_data audio[] = {
	{
		.pwr_con = &mtk_spm->audio_pwr_con,
		.pwr_sta_mask = AUDIO_PWR_STA_MASK,
		.sram_pdn_mask = AUDIO_SRAM_PDN_MASK,
		.sram_ack_mask = AUDIO_SRAM_ACK_MASK,
	},
};

#endif  /* SOC_MEDIATEK_MT8192_SPM_H */
