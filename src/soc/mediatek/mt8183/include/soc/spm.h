/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8183_SPM_H
#define SOC_MEDIATEK_MT8183_SPM_H

#include <soc/addressmap.h>
#include <types.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE                    0xb16
#define SPM_REGWR_CFG_KEY                   (SPM_PROJECT_CODE << 16)

/* POWERON_CONFIG_EN (0x10006000+0x000) */
#define BCLK_CG_EN_LSB                      (1U << 0)       /* 1b */
#define MD_BCLK_CG_EN_LSB                   (1U << 1)       /* 1b */
#define PCM_IM_HOST_W_EN_LSB                (1U << 30)      /* 1b */
#define PCM_IM_HOST_EN_LSB                  (1U << 31)      /* 1b */

/* SPM_CLK_CON (0x10006000+0x00C) */
#define SYSCLK0_EN_CTRL_LSB                 (1U << 0)       /* 2b */
#define SYSCLK1_EN_CTRL_LSB                 (1U << 2)       /* 2b */
#define SPM_LOCK_INFRA_DCM_LSB              (1U << 5)       /* 1b */
#define EXT_SRCCLKEN_MASK                   (1U << 6)       /* 1b */
#define CXO32K_REMOVE_EN_MD1_LSB            (1U << 9)       /* 1b */
#define CLKSQ1_SEL_CTRL_LSB                 (1U << 12)      /* 1b */
#define SRCLKEN0_EN_LSB                     (1U << 13)      /* 1b */

/* PCM_CON0 (0x10006000+0x018) */
#define PCM_KICK_L_LSB                      (1U << 0)       /* 1b */
#define IM_KICK_L_LSB                       (1U << 1)       /* 1b */
#define PCM_CK_EN_LSB                       (1U << 2)       /* 1b */
#define EN_IM_SLEEP_DVS_LSB                 (1U << 3)       /* 1b */
#define IM_AUTO_PDN_EN_LSB                  (1U << 4)       /* 1b */
#define PCM_SW_RESET_LSB                    (1U << 15)      /* 1b */

/* PCM_CON1 (0x10006000+0x01C) */
#define IM_SLAVE_LSB                        (1U << 0)       /* 1b */
#define IM_SLEEP_LSB                        (1U << 1)       /* 1b */
#define MIF_APBEN_LSB                       (1U << 3)       /* 1b */
#define IM_PDN_LSB                          (1U << 4)       /* 1b */
#define PCM_TIMER_EN_LSB                    (1U << 5)       /* 1b */
#define IM_NONRP_EN_LSB                     (1U << 6)       /* 1b */
#define DIS_MIF_PROT_LSB                    (1U << 7)       /* 1b */
#define PCM_WDT_EN_LSB                      (1U << 8)       /* 1b */
#define PCM_WDT_WAKE_MODE_LSB               (1U << 9)       /* 1b */
#define SPM_SRAM_SLEEP_B_LSB                (1U << 10)      /* 1b */
#define SPM_SRAM_ISOINT_B_LSB               (1U << 11)      /* 1b */
#define EVENT_LOCK_EN_LSB                   (1U << 12)      /* 1b */
#define SRCCLKEN_FAST_RESP_LSB              (1U << 13)      /* 1b */
#define SCP_APB_INTERNAL_EN_LSB             (1U << 14)      /* 1b */

/* SPM_IRQ_MASK (0x10006000+0x0B4) */
#define PCM_IRQ_ROOT_MASK_LSB               (1U << 3)       /* 1b */

/* SPM_WAKEUP_EVENT_MASK (0x10006000+0x0C4) */
#define WAKEUP_EVENT_MASK_B_BIT0            (1U << 0)       /* 1b */

/* SPARE_SRC_REQ_MASK (0x10006000+0x6C0) */
#define SPARE1_DDREN_MASK_B_LSB             (1U << 0)       /* 1b */

/* SPM_PC_TRACE_CON (0x10006000+0x8C0) */
#define SPM_PC_TRACE_OFFSET_LSB             (1U << 0)       /* 12b */
#define SPM_PC_TRACE_OFFSET                 (1U << 3)       /* 1b */
#define SPM_PC_TRACE_HW_EN_LSB              (1U << 16)      /* 1b */

/* SPM_SPARE_ACK_MASK (0x10006000+0x6F4) */
#define SPARE_ACK_MASK_B_BIT0               (1U << 0)       /* 1b */
#define SPARE_ACK_MASK_B_BIT1               (1U << 1)       /* 1b */

/**************************************
 * Config and Parameter
 **************************************/
#define CONN_DDR_EN_DBC_LEN                 (0x00000154 << 20)
#define IFR_SRAMROM_ROM_PDN                 (0x0000003f)
#define IM_STATE                            (0x4 << 7)
#define IM_STATE_MASK                       (0x7 << 7)
#define MD_DDR_EN_0_DBC_LEN                 (0x00000154)
#define MD_DDR_EN_1_DBC_LEN                 (0x00000154 << 10)
#define PCM_FSM_STA_DEF                     (0x00108490)
#define PCM_FSM_STA_MASK                    (0x7FFFFF)
#define POWER_ON_VAL1_DEF                   (0x00015800)
#define SPM_CORE_TIMEOUT                    (5000)
#define SPM_MAS_PAUSE_MASK_B_VAL            (0xFFFFFFFF)
#define SPM_MAS_PAUSE2_MASK_B_VAL           (0xFFFFFFFF)
#define SPM_PCM_REG1_DATA_CHECK             (0x1)
#define SPM_PCM_REG15_DATA_CHECK            (0x0)
#define SPM_WAKEUP_EVENT_MASK_DEF           (0xF0F92218)
#define SYSCLK1_EN_CTRL                     (0x3 << 2)
#define SYSCLK1_SRC_MASK_B                  (0x10 << 23)

/**************************************
 * Define and Declare
 **************************************/
/* SPM_IRQ_MASK */
#define ISRM_TWAM                           (1U << 2)
#define ISRM_PCM_RETURN                     (1U << 3)
#define ISRM_RET_IRQ_AUX                    (0x3FF00)
#define ISRM_ALL_EXC_TWAM                   (ISRM_RET_IRQ_AUX)
#define ISRM_ALL                            (ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM                           (1U << 2)
#define ISRS_PCM_RETURN                     (1U << 3)
#define ISRS_SW_INT0                        (1U << 4)
#define ISRC_TWAM                           (ISRS_TWAM)
#define ISRC_ALL_EXC_TWAM                   (ISRS_PCM_RETURN)
#define ISRC_ALL                            (ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* PCM_PWR_IO_EN */
#define PCM_PWRIO_EN_R0                     (1U << 0)
#define PCM_PWRIO_EN_R7                     (1U << 7)
#define PCM_RF_SYNC_R0                      (1U << 16)
#define PCM_RF_SYNC_R6                      (1U << 22)
#define PCM_RF_SYNC_R7                      (1U << 23)

/* SPM_SWINT */
#define PCM_SW_INT_ALL                      (0x3FF)

enum {
	DISP_PWR_STA_MASK	= 0x1 << 3,
	DISP_SRAM_PDN_MASK	= 0x1 << 8,
	DISP_SRAM_ACK_MASK	= 0x1 << 12,
	AUDIO_PWR_STA_MASK	= 0x1 << 24,
	AUDIO_SRAM_PDN_MASK	= 0xf << 8,
	AUDIO_SRAM_ACK_MASK	= 0xf << 12,
};

#define PCM_EVENT_VECTOR_NUM 16

struct mtk_spm_regs {
	u32 poweron_config_set;
	u32 spm_power_on_val0;
	u32 spm_power_on_val1;
	u32 spm_clk_con;
	u32 spm_clk_settle;
	u32 spm_ap_standby_con;
	u32 pcm_con0;
	u32 pcm_con1;
	u32 pcm_im_ptr;
	u32 pcm_im_len;
	u32 pcm_reg_data_ini;
	u32 pcm_pwr_io_en;
	u32 pcm_timer_val;
	u32 pcm_wdt_val;
	u32 pcm_im_host_rw_ptr;
	u32 pcm_im_host_rw_dat;
	u32 pcm_event_vector[PCM_EVENT_VECTOR_NUM];
	u32 pcm_event_vector_en;
	u32 reserved1[1];
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
	u32 spm_wakeup_event_mask;
	u32 spm_wakeup_event_ext_mask;
	u32 spm_twam_event_clear;
	u32 scp_clk_con;
	u32 pcm_debug_con;
	u32 ddr_en_dbc_len;
	u32 ahb_bus_con;
	u32 spm_src3_mask;
	u32 ddr_en_emi_dbc_con;
	u32 md32_clk_con;
	u32 reserved2[5];
	u32 pcm_reg0_data;
	u32 pcm_reg1_data;
	u32 pcm_reg2_data;
	u32 pcm_reg3_data;
	u32 pcm_reg4_data;
	u32 pcm_reg5_data;
	u32 pcm_reg6_data;
	u32 pcm_reg7_data;
	u32 pcm_reg8_data;
	u32 pcm_reg9_data;
	u32 pcm_reg10_data;
	u32 pcm_reg11_data;
	u32 pcm_reg12_data;
	u32 pcm_reg13_data;
	u32 pcm_reg14_data;
	u32 pcm_reg15_data;
	u32 pcm_reg12_mask_b_sta;
	u32 pcm_reg12_ext_data;
	u32 pcm_reg12_ext_mask_b_sta;
	u32 pcm_event_reg_sta;
	u32 pcm_timer_out;
	u32 pcm_wdt_out;
	u32 spm_irq_sta;
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_misc;
	u32 bus_protect_rdy;
	u32 bus_protect2_rdy;
	u32 subsys_idle_sta;
	u32 cpu_idle_sta;
	u32 pcm_fsm_sta;
	u32 src_req_sta;
	u32 pwr_status;
	u32 pwr_status_2nd;
	u32 cpu_pwr_status;
	u32 cpu_pwr_status_2nd;
	u32 misc_sta;
	u32 spm_src_rdy_sta;
	u32 reserved3[1];
	u32 dramc_dbg_latch;
	u32 spm_twam_last_sta0;
	u32 spm_twam_last_sta1;
	u32 spm_twam_last_sta2;
	u32 spm_twam_last_sta3;
	u32 spm_twam_curr_sta0;
	u32 spm_twam_curr_sta1;
	u32 spm_twam_curr_sta2;
	u32 spm_twam_curr_sta3;
	u32 spm_twam_timer_out;
	u32 reserved4[1];
	u32 spm_dvfs_sta;
	u32 bus_protect3_rdy;
	u32 reserved5[4];
	u32 src_ddren_sta;
	u32 reserved6[7];
	u32 mcu_pwr_con;
	u32 mp0_cputop_pwr_con;
	u32 mp0_cpu0_pwr_con;
	u32 mp0_cpu1_pwr_con;
	u32 mp0_cpu2_pwr_con;
	u32 mp0_cpu3_pwr_con;
	u32 mp1_cputop_pwr_con;
	u32 mp1_cpu0_pwr_con;
	u32 mp1_cpu1_pwr_con;
	u32 mp1_cpu2_pwr_con;
	u32 mp1_cpu3_pwr_con;
	u32 reserved7[5];
	u32 mp0_cputop_l2_pdn;
	u32 mp0_cputop_l2_sleep_b;
	u32 mp0_cpu0_l1_pdn;
	u32 mp0_cpu1_l1_pdn;
	u32 mp0_cpu2_l1_pdn;
	u32 mp0_cpu3_l1_pdn;
	u32 mp1_cputop_l2_pdn;
	u32 mp1_cputop_l2_sleep_b;
	u32 mp1_cpu0_l1_pdn;
	u32 mp1_cpu1_l1_pdn;
	u32 mp1_cpu2_l1_pdn;
	u32 mp1_cpu3_l1_pdn;
	u32 reserved8[8];
	u32 cpu_ext_buck_iso;
	u32 reserved9[7];
	u32 dummy1_pwr_con;
	u32 bypass_spmc;
	u32 spmc_dormant_enable;
	u32 armpll_clk_con;
	u32 spmc_in_ret;
	u32 reserved10[15];
	u32 vde_pwr_con;
	u32 ven_pwr_con;
	u32 isp_pwr_con;
	u32 dis_pwr_con;
	u32 mfg_core1_pwr_con;
	u32 audio_pwr_con;
	u32 ifr_pwr_con;
	u32 dpy_pwr_con;
	u32 md1_pwr_con;
	u32 vpu_top_pwr_con;
	u32 reserved11[1];
	u32 conn_pwr_con;
	u32 vpu_core2_pwr_con;
	u32 mfg_async_pwr_con;
	u32 mfg_pwr_con;
	u32 vpu_core0_pwr_con;
	u32 vpu_core1_pwr_con;
	u32 cam_pwr_con;
	u32 mfg_2d_pwr_con;
	u32 mfg_core0_pwr_con;
	u32 sysram_con;
	u32 sysrom_con;
	u32 sspm_sram_con;
	u32 scp_sram_con;
	u32 reserved12[3];
	u32 ufs_sram_con;
	u32 reserved13[4];
	u32 dummy_sram_con;
	u32 reserved14[3];
	u32 md_ext_buck_iso_con;
	u32 md_sram_iso_con;
	u32 md_extra_pwr_con;
	u32 reserved15[1];
	u32 ext_buck_con;
	u32 reserved16[11];
	u32 mbist_efuse_repair_ack_sta;
	u32 reserved17[11];
	u32 spm_dvfs_con;
	u32 spm_mdbsi_con;
	u32 spm_mas_pause_mask_b;
	u32 spm_mas_pause2_mask_b;
	u32 spm_bsi_gen;
	u32 spm_bsi_en_sr;
	u32 spm_bsi_clk_sr;
	u32 spm_bsi_d0_sr;
	u32 spm_bsi_d1_sr;
	u32 spm_bsi_d2_sr;
	u32 spm_ap_sema;
	u32 spm_spm_sema;
	u32 ap_mdsrc_req;
	u32 reserved18[1];
	u32 spm2md_dvfs_con;
	u32 md2spm_dvfs_con;
	u32 dramc_dpy_clk_sw_con_rsv;
	u32 dpy_lp_con;
	u32 cpu_dvfs_req;
	u32 spm_pll_con;
	u32 spm_emi_bw_mode;
	u32 ap2md_peer_wakeup;
	u32 ulposc_con;
	u32 spm2mm_con;
	u32 dramc_dpy_clk_sw_con_sel;
	u32 dramc_dpy_clk_sw_con;
	u32 spm_s1_mode_ch;
	u32 emi_self_refresh_ch_sta;
	u32 dramc_dpy_clk_sw_con_sel2;
	u32 dramc_dpy_clk_sw_con2;
	u32 dramc_dmyrd_con;
	u32 spm_drs_con;
	u32 spm_sema_m0;
	u32 spm_sema_m1;
	u32 spm_sema_m2;
	u32 spm_sema_m3;
	u32 spm_sema_m4;
	u32 spm_sema_m5;
	u32 spm_sema_m6;
	u32 spm_sema_m7;
	u32 spm_mas_pause_mm_mask_b;
	u32 spm_mas_pause_mcu_mask_b;
	u32 reserved19[1];
	u32 sram_dreq_ack;
	u32 sram_dreq_con;
	u32 sram_dreq_con_set;
	u32 sram_dreq_con_clr;
	u32 spm2emi_enter_ulpm;
	u32 spm_md32_irq;
	u32 spm2pmcu_int;
	u32 spm2pmcu_int_set;
	u32 spm2pmcu_int_clr;
	u32 spm2pmcu_mailbox_0;
	u32 spm2pmcu_mailbox_1;
	u32 spm2pmcu_mailbox_2;
	u32 spm2pmcu_mailbox_3;
	u32 pmcu2spm_int;
	u32 pmcu2spm_int_set;
	u32 pmcu2spm_int_clr;
	u32 pmcu2spm_mailbox_0;
	u32 pmcu2spm_mailbox_1;
	u32 pmcu2spm_mailbox_2;
	u32 pmcu2spm_mailbox_3;
	u32 pmcu2spm_cfg;
	u32 mp0_cpu0_irq_mask;
	u32 mp0_cpu1_irq_mask;
	u32 mp0_cpu2_irq_mask;
	u32 mp0_cpu3_irq_mask;
	u32 mp1_cpu0_irq_mask;
	u32 mp1_cpu1_irq_mask;
	u32 mp1_cpu2_irq_mask;
	u32 mp1_cpu3_irq_mask;
	u32 reserved20[4];
	u32 mp0_cpu0_wfi_en;
	u32 mp0_cpu1_wfi_en;
	u32 mp0_cpu2_wfi_en;
	u32 mp0_cpu3_wfi_en;
	u32 mp1_cpu0_wfi_en;
	u32 mp1_cpu1_wfi_en;
	u32 mp1_cpu2_wfi_en;
	u32 mp1_cpu3_wfi_en;
	u32 reserved21[1];
	u32 mp0_l2cflush;
	u32 mp1_l2cflush;
	u32 reserved22[1];
	u32 cpu_ptpod2_con;
	u32 reserved23[3];
	u32 root_cputop_addr;
	u32 root_core_addr;
	u32 reserved24[2];
	u32 cpu_spare_con;
	u32 cpu_spare_con_set;
	u32 cpu_spare_con_clr;
	u32 reserved25[17];
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
	u32 spm_sw_flag;
	u32 spm_sw_debug;
	u32 spm_sw_rsv_0;
	u32 spm_sw_rsv_1;
	u32 spm_sw_rsv_2;
	u32 spm_sw_rsv_3;
	u32 spm_sw_rsv_4;
	u32 spm_sw_rsv_5;
	u32 spm_rsv_con;
	u32 spm_rsv_sta;
	u32 spm_rsv_con1;
	u32 spm_rsv_sta1;
	u32 spm_pasr_dpd_0;
	u32 spm_pasr_dpd_1;
	u32 spm_pasr_dpd_2;
	u32 spm_pasr_dpd_3;
	u32 spm_spare_con;
	u32 spm_spare_con_set;
	u32 spm_spare_con_clr;
	u32 spm_sw_rsv_6;
	u32 spm_sw_rsv_7;
	u32 spm_sw_rsv_8;
	u32 spm_sw_rsv_9;
	u32 spm_sw_rsv_10;
	u32 reserved26[7];
	u32 spm_sw_rsv_18;
	u32 spm_sw_rsv_19;
	u32 reserved27[3];
	u32 dvfsrc_event_mask_con;
	u32 dvfsrc_event_force_on;
	u32 dvfsrc_event_sel;
	u32 spm_dvfs_event_sta;
	u32 spm_dvfs_event_sta1;
	u32 spm_dvfs_level;
	u32 dvfs_abort_sta;
	u32 dvfs_abort_others_mask;
	u32 spm_dfs_level;
	u32 spm_dvs_level;
	u32 spm_dvfs_misc;
	u32 reserved28[1];
	u32 spare_src_req_mask;
	u32 scp_vcore_level;
	u32 sc_mm_ck_sel_con;
	u32 reserved29[9];
	u32 spare_ack_sta;
	u32 spare_ack_mask;
	u32 reserved30[2];
	u32 spm_dvfs_con1;
	u32 spm_dvfs_con1_sta;
	u32 reserved31[2];
	u32 spm_dvfs_cmd0;
	u32 spm_dvfs_cmd1;
	u32 spm_dvfs_cmd2;
	u32 spm_dvfs_cmd3;
	u32 spm_dvfs_cmd4;
	u32 spm_dvfs_cmd5;
	u32 spm_dvfs_cmd6;
	u32 spm_dvfs_cmd7;
	u32 spm_dvfs_cmd8;
	u32 spm_dvfs_cmd9;
	u32 spm_dvfs_cmd10;
	u32 spm_dvfs_cmd11;
	u32 spm_dvfs_cmd12;
	u32 spm_dvfs_cmd13;
	u32 spm_dvfs_cmd14;
	u32 spm_dvfs_cmd15;
	u32 reserved32[12];
	u32 wdt_latch_spare0_fix;
	u32 wdt_latch_spare1_fix;
	u32 wdt_latch_spare2_fix;
	u32 wdt_latch_spare3_fix;
	u32 spare_ack_in_fix;
	u32 dcha_latch_rsv0_fix;
	u32 dchb_latch_rsv0_fix;
	u32 reserved33[25];
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
	u32 wdt_latch_spare0;
	u32 wdt_latch_spare1;
	u32 wdt_latch_spare2;
	u32 wdt_latch_spare3;
	u32 pcm_wdt_latch_10;
	u32 pcm_wdt_latch_11;
	u32 dcha_gating_latch_0;
	u32 dcha_gating_latch_1;
	u32 dcha_gating_latch_2;
	u32 dcha_gating_latch_3;
	u32 dcha_gating_latch_4;
	u32 dcha_gating_latch_5;
	u32 dcha_gating_latch_6;
	u32 dcha_gating_latch_7;
	u32 dchb_gating_latch_0;
	u32 dchb_gating_latch_1;
	u32 dchb_gating_latch_2;
	u32 dchb_gating_latch_3;
	u32 dchb_gating_latch_4;
	u32 dchb_gating_latch_5;
	u32 dchb_gating_latch_6;
	u32 dchb_gating_latch_7;
	u32 dcha_latch_rsv0;
	u32 dchb_latch_rsv0;
	u32 pcm_wdt_latch_12;
	u32 pcm_wdt_latch_13;
	u32 reserved34[12];
	u32 spm_pc_trace_con;
	u32 spm_pc_trace_g0;
	u32 spm_pc_trace_g1;
	u32 spm_pc_trace_g2;
	u32 spm_pc_trace_g3;
	u32 spm_pc_trace_g4;
	u32 spm_pc_trace_g5;
	u32 spm_pc_trace_g6;
	u32 spm_pc_trace_g7;
	u32 reserved35[7];
	u32 spm_ack_chk_con;
	u32 spm_ack_chk_pc;
	u32 spm_ack_chk_sel;
	u32 spm_ack_chk_timer;
	u32 spm_ack_chk_sta;
	u32 spm_ack_chk_latch;
	u32 reserved36[2];
	u32 spm_ack_chk_con2;
	u32 spm_ack_chk_pc2;
	u32 spm_ack_chk_sel2;
	u32 spm_ack_chk_timer2;
	u32 spm_ack_chk_sta2;
	u32 spm_ack_chk_latch2;
	u32 reserved37[2];
	u32 spm_ack_chk_con3;
	u32 spm_ack_chk_pc3;
	u32 spm_ack_chk_sel3;
	u32 spm_ack_chk_timer3;
	u32 spm_ack_chk_sta3;
	u32 spm_ack_chk_latch3;
	u32 reserved38[2];
	u32 spm_ack_chk_con4;
	u32 spm_ack_chk_pc4;
	u32 spm_ack_chk_sel4;
	u32 spm_ack_chk_timer4;
	u32 spm_ack_chk_sta4;
	u32 spm_ack_chk_latch4;
};
check_member(mtk_spm_regs, spm_ack_chk_latch4, 0x0974);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

enum dyna_load_pcm_index {
	DYNA_LOAD_PCM_SUSPEND_LP4_3733 = 0,
	DYNA_LOAD_PCM_SUSPEND_LP4_3200,
	DYNA_LOAD_PCM_MAX,
};

struct pcm_desc {
	u16 size;				/* binary array size */
	u8 sess;				/* session number */
	u8 replace;				/* replace mode */
	u16 addr_2nd;				/* 2nd binary array size */
	u16 reserved;				/* for 32bit alignment */
	u32 vector[PCM_EVENT_VECTOR_NUM];	/* event vector config */
};

struct dyna_load_pcm {
	u32 *buf;				/* binary array */
	struct pcm_desc desc;
};

int spm_init(void);

#endif  /* SOC_MEDIATEK_MT8183_SPM_H */
