/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8188_SPM_H
#define SOC_MEDIATEK_MT8188_SPM_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <types.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE			0xb16
#define SPM_REGWR_CFG_KEY			(SPM_PROJECT_CODE << 16)

/* POWERON_CONFIG_EN (0x10006000+0x000) */
#define BCLK_CG_EN_LSB				BIT(0)

/* SPM_CLK_CON (0x10006000+0x00C) */
DEFINE_BIT(REG_SYSCLK1_SRC_MD2_SRCCLKENA, 28)

/* PCM_CON0 (0x10006000+0x018) */
#define PCM_CK_EN_LSB				BIT(2)
#define PCM_SW_RESET_LSB			BIT(15)

/* PCM_CON1 (0x10006000+0x01C) */
#define RG_IM_SLAVE_LSB				BIT(0)
#define RG_AHBMIF_APBEN_LSB			BIT(3)
#define RG_PCM_TIMER_EN_LSB			BIT(5)
#define SPM_EVENT_COUNTER_CLR_LSB		BIT(6)
#define RG_PCM_WDT_WAKE_LSB			BIT(9)
#define REG_SPM_SRAM_ISOINT_B_LSB		BIT(11)
#define REG_EVENT_LOCK_EN_LSB			BIT(12)
#define REG_MD32_APB_INTERNAL_EN_LSB		BIT(14)

/* SPM_WAKEUP_EVENT_MASK (0x10006000+0x0D0) */
#define SPM_WAKEUP_EVENT_MASK_BIT0		BIT(0)
#define SPM_WAKEUP_EVENT_MASK_CSYSPWREQ_B	BIT(11)

/* DDR_EN_DBC_CON1 (0x10006000+0x0EC) */
#define REG_ALL_DDR_EN_DBC_EN_LSB		BIT(0)

/* SPM_DVFS_MISC (0x10006000+0x4AC) */
DEFINE_BIT(SPM_DVFS_FORCE_ENABLE_LSB, 2)
DEFINE_BIT(SPM_DVFSRC_ENABLE_LSB, 4)

/* SPM_SW_FLAG_0 (0x10006000+0x600) */
#define SPM_FLAG_DISABLE_VCORE_DVS		BIT(3)
#define SPM_FLAG_DISABLE_VCORE_DFS		BIT(4)
#define SPM_FLAG_RUN_COMMON_SCENARIO		BIT(10)

/* SYS_TIMER_CON (0x10006000+0x98C) */
DEFINE_BIT(SYS_TIMER_START_EN_LSB, 0)

/* MD32PCM_CFGREG_SW_RSTN (0x10006000+0xA00) */
DEFINE_BIT(MD32PCM_CFGREG_SW_RSTN_RESET, 0)

/**************************************
 * Config and Parameter
 **************************************/
#define POWER_ON_VAL1_DEF		0x80015860
#define SPM_WAKEUP_EVENT_MASK_DEF	0xffffffff
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
#define PCM_WDT_TIMEOUT			(30 * 32768)
#define PCM_TIMER_MAX			(0xffffffff - PCM_WDT_TIMEOUT)

/**************************************
 * Definition and Declaration
 **************************************/
/* SPM_IRQ_MASK */
DEFINE_BIT(ISRM_TWAM_BF, 2)
DEFINE_BITFIELD(ISRM_RET_IRQ_AUX_BF, 17, 8)
#define ISRM_TWAM			BIT(2)
#define ISRM_RET_IRQ_AUX		(0x3ff << 8)
#define ISRM_ALL_EXC_TWAM		(ISRM_RET_IRQ_AUX)
#define ISRM_ALL			(ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM			BIT(2)
#define ISRS_PCM_RETURN			BIT(3)
#define ISRC_TWAM			ISRS_TWAM
#define ISRC_ALL_EXC_TWAM		ISRS_PCM_RETURN
#define ISRC_ALL			(ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* PCM_PWR_IO_EN */
#define PCM_PWRIO_EN_R0			BIT(0)
#define PCM_PWRIO_EN_R7			BIT(7)
#define PCM_RF_SYNC_R0			BIT(16)
#define PCM_RF_SYNC_R6			BIT(22)
#define PCM_RF_SYNC_R7			BIT(23)

/* SPM_SWINT */
#define PCM_SW_INT_ALL			0x3ff

struct pwr_ctrl {
	/* for SPM */
	uint32_t pcm_flags;
	/* can override pcm_flags */
	uint32_t pcm_flags_cust;
	/* set bit of pcm_flags, after pcm_flags_cust */
	uint32_t pcm_flags_cust_set;
	/* clr bit of pcm_flags, after pcm_flags_cust */
	uint32_t pcm_flags_cust_clr;
	uint32_t pcm_flags1;
	/* can override pcm_flags1 */
	uint32_t pcm_flags1_cust;
	/* set bit of pcm_flags1, after pcm_flags1_cust */
	uint32_t pcm_flags1_cust_set;
	/* clr bit of pcm_flags1, after pcm_flags1_cust */
	uint32_t pcm_flags1_cust_clr;
	/* @ 1T 32K */
	uint32_t timer_val;
	/* @ 1T 32K, can override timer_val */
	uint32_t timer_val_cust;
	/* stress for dpidle */
	uint32_t timer_val_ramp_en;
	/* stress for suspend */
	uint32_t timer_val_ramp_en_sec;
	uint32_t wake_src;
	/* can override wake_src */
	uint32_t wake_src_cust;
	/* disable wdt in suspend */
	uint8_t wdt_disable;

	/* SPM_AP_STANDBY_CON */
	/* [0] */
	uint8_t reg_wfi_op;
	/* [1] */
	uint8_t reg_wfi_type;
	/* [2] */
	uint8_t reg_mp0_cputop_idle_mask;
	/* [3] */
	uint8_t reg_mp1_cputop_idle_mask;
	/* [4] */
	uint8_t reg_mcusys_idle_mask;
	/* [25] */
	uint8_t reg_md_apsrc_1_sel;
	/* [26] */
	uint8_t reg_md_apsrc_0_sel;
	/* [29] */
	uint8_t reg_conn_apsrc_sel;

	/* SPM_SRC_REQ */
	/* [0] */
	uint8_t reg_spm_apsrc_req;
	/* [1] */
	uint8_t reg_spm_f26m_req;
	/* [3] */
	uint8_t reg_spm_infra_req;
	/* [4] */
	uint8_t reg_spm_vrf18_req;
	/* [7] */
	uint8_t reg_spm_ddr_en_req;
	/* [8] */
	uint8_t reg_spm_dvfs_req;
	/* [9] */
	uint8_t reg_spm_sw_mailbox_req;
	/* [10] */
	uint8_t reg_spm_sspm_mailbox_req;
	/* [11] */
	uint8_t reg_spm_adsp_mailbox_req;
	/* [12] */
	uint8_t reg_spm_scp_mailbox_req;

	/* SPM_SRC_MASK */
	/* [0] */
	uint8_t reg_sspm_srcclkena_0_mask_b;
	/* [1] */
	uint8_t reg_sspm_infra_req_0_mask_b;
	/* [2] */
	uint8_t reg_sspm_apsrc_req_0_mask_b;
	/* [3] */
	uint8_t reg_sspm_vrf18_req_0_mask_b;
	/* [4] */
	uint8_t reg_sspm_ddr_en_0_mask_b;
	/* [5] */
	uint8_t reg_scp_srcclkena_mask_b;
	/* [6] */
	uint8_t reg_scp_infra_req_mask_b;
	/* [7] */
	uint8_t reg_scp_apsrc_req_mask_b;
	/* [8] */
	uint8_t reg_scp_vrf18_req_mask_b;
	/* [9] */
	uint8_t reg_scp_ddr_en_mask_b;
	/* [10] */
	uint8_t reg_audio_dsp_srcclkena_mask_b;
	/* [11] */
	uint8_t reg_audio_dsp_infra_req_mask_b;
	/* [12] */
	uint8_t reg_audio_dsp_apsrc_req_mask_b;
	/* [13] */
	uint8_t reg_audio_dsp_vrf18_req_mask_b;
	/* [14] */
	uint8_t reg_audio_dsp_ddr_en_mask_b;
	/* [15] */
	uint8_t reg_apu_srcclkena_mask_b;
	/* [16] */
	uint8_t reg_apu_infra_req_mask_b;
	/* [17] */
	uint8_t reg_apu_apsrc_req_mask_b;
	/* [18] */
	uint8_t reg_apu_vrf18_req_mask_b;
	/* [19] */
	uint8_t reg_apu_ddr_en_mask_b;
	/* [20] */
	uint8_t reg_cpueb_srcclkena_mask_b;
	/* [21] */
	uint8_t reg_cpueb_infra_req_mask_b;
	/* [22] */
	uint8_t reg_cpueb_apsrc_req_mask_b;
	/* [23] */
	uint8_t reg_cpueb_vrf18_req_mask_b;
	/* [24] */
	uint8_t reg_cpueb_ddr_en_mask_b;
	/* [25] */
	uint8_t reg_bak_psri_srcclkena_mask_b;
	/* [26] */
	uint8_t reg_bak_psri_infra_req_mask_b;
	/* [27] */
	uint8_t reg_bak_psri_apsrc_req_mask_b;
	/* [28] */
	uint8_t reg_bak_psri_vrf18_req_mask_b;
	/* [29] */
	uint8_t reg_bak_psri_ddr_en_mask_b;

	/* SPM_SRC2_MASK */
	/* [0] */
	uint8_t reg_msdc0_srcclkena_mask_b;
	/* [1] */
	uint8_t reg_msdc0_infra_req_mask_b;
	/* [2] */
	uint8_t reg_msdc0_apsrc_req_mask_b;
	/* [3] */
	uint8_t reg_msdc0_vrf18_req_mask_b;
	/* [4] */
	uint8_t reg_msdc0_ddr_en_mask_b;
	/* [5] */
	uint8_t reg_msdc1_srcclkena_mask_b;
	/* [6] */
	uint8_t reg_msdc1_infra_req_mask_b;
	/* [7] */
	uint8_t reg_msdc1_apsrc_req_mask_b;
	/* [8] */
	uint8_t reg_msdc1_vrf18_req_mask_b;
	/* [9] */
	uint8_t reg_msdc1_ddr_en_mask_b;
	/* [10] */
	uint8_t reg_msdc2_srcclkena_mask_b;
	/* [11] */
	uint8_t reg_msdc2_infra_req_mask_b;
	/* [12] */
	uint8_t reg_msdc2_apsrc_req_mask_b;
	/* [13] */
	uint8_t reg_msdc2_vrf18_req_mask_b;
	/* [14] */
	uint8_t reg_msdc2_ddr_en_mask_b;
	/* [15] */
	uint8_t reg_ufs_srcclkena_mask_b;
	/* [16] */
	uint8_t reg_ufs_infra_req_mask_b;
	/* [17] */
	uint8_t reg_ufs_apsrc_req_mask_b;
	/* [18] */
	uint8_t reg_ufs_vrf18_req_mask_b;
	/* [19] */
	uint8_t reg_ufs_ddr_en_mask_b;
	/* [20] */
	uint8_t reg_usb_srcclkena_mask_b;
	/* [21] */
	uint8_t reg_usb_infra_req_mask_b;
	/* [22] */
	uint8_t reg_usb_apsrc_req_mask_b;
	/* [23] */
	uint8_t reg_usb_vrf18_req_mask_b;
	/* [24] */
	uint8_t reg_usb_ddr_en_mask_b;
	/* [25] */
	uint8_t reg_pextp_p0_srcclkena_mask_b;
	/* [26] */
	uint8_t reg_pextp_p0_infra_req_mask_b;
	/* [27] */
	uint8_t reg_pextp_p0_apsrc_req_mask_b;
	/* [28] */
	uint8_t reg_pextp_p0_vrf18_req_mask_b;
	/* [29] */
	uint8_t reg_pextp_p0_ddr_en_mask_b;

	/* SPM_SRC3_MASK */
	/* [0] */
	uint8_t reg_pextp_p1_srcclkena_mask_b;
	/* [1] */
	uint8_t reg_pextp_p1_infra_req_mask_b;
	/* [2] */
	uint8_t reg_pextp_p1_apsrc_req_mask_b;
	/* [3] */
	uint8_t reg_pextp_p1_vrf18_req_mask_b;
	/* [4] */
	uint8_t reg_pextp_p1_ddr_en_mask_b;
	/* [5] */
	uint8_t reg_gce0_infra_req_mask_b;
	/* [6] */
	uint8_t reg_gce0_apsrc_req_mask_b;
	/* [7] */
	uint8_t reg_gce0_vrf18_req_mask_b;
	/* [8] */
	uint8_t reg_gce0_ddr_en_mask_b;
	/* [9] */
	uint8_t reg_gce1_infra_req_mask_b;
	/* [10] */
	uint8_t reg_gce1_apsrc_req_mask_b;
	/* [11] */
	uint8_t reg_gce1_vrf18_req_mask_b;
	/* [12] */
	uint8_t reg_gce1_ddr_en_mask_b;
	/* [13] */
	uint8_t reg_spm_srcclkena_reserved_mask_b;
	/* [14] */
	uint8_t reg_spm_infra_req_reserved_mask_b;
	/* [15] */
	uint8_t reg_spm_apsrc_req_reserved_mask_b;
	/* [16] */
	uint8_t reg_spm_vrf18_req_reserved_mask_b;
	/* [17] */
	uint8_t reg_spm_ddr_en_reserved_mask_b;
	/* [18] */
	uint8_t reg_disp0_apsrc_req_mask_b;
	/* [19] */
	uint8_t reg_disp0_ddr_en_mask_b;
	/* [20] */
	uint8_t reg_disp1_apsrc_req_mask_b;
	/* [21] */
	uint8_t reg_disp1_ddr_en_mask_b;
	/* [22] */
	uint8_t reg_disp2_apsrc_req_mask_b;
	/* [23] */
	uint8_t reg_disp2_ddr_en_mask_b;
	/* [24] */
	uint8_t reg_disp3_apsrc_req_mask_b;
	/* [25] */
	uint8_t reg_disp3_ddr_en_mask_b;
	/* [26] */
	uint8_t reg_infrasys_apsrc_req_mask_b;
	/* [27] */
	uint8_t reg_infrasys_ddr_en_mask_b;
	/* [28] */
	uint8_t reg_cg_check_srcclkena_mask_b;
	/* [29] */
	uint8_t reg_cg_check_apsrc_req_mask_b;
	/* [30] */
	uint8_t reg_cg_check_vrf18_req_mask_b;
	/* [31] */
	uint8_t reg_cg_check_ddr_en_mask_b;

	/* SPM_SRC4_MASK */
	/* [8:0] */
	uint32_t reg_mcusys_merge_apsrc_req_mask_b;
	/* [17:9] */
	uint32_t reg_mcusys_merge_ddr_en_mask_b;
	/* [19:18] */
	uint8_t reg_dramc_md32_infra_req_mask_b;
	/* [21:20] */
	uint8_t reg_dramc_md32_vrf18_req_mask_b;
	/* [23:22] */
	uint8_t reg_dramc_md32_ddr_en_mask_b;
	/* [24] */
	uint8_t reg_dvfsrc_event_trigger_mask_b;

	/* SPM_WAKEUP_EVENT_MASK2 */
	/* [3:0] */
	uint8_t reg_sc_sw2spm_wakeup_mask_b;
	/* [4] */
	uint8_t reg_sc_adsp2spm_wakeup_mask_b;
	/* [8:5] */
	uint8_t reg_sc_sspm2spm_wakeup_mask_b;
	/* [9] */
	uint8_t reg_sc_scp2spm_wakeup_mask_b;
	/* [10] */
	uint8_t reg_csyspwrup_ack_mask;
	/* [11] */
	uint8_t reg_csyspwrup_req_mask;

	/* SPM_WAKEUP_EVENT_MASK */
	/* [31:0] */
	uint32_t reg_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	/* [31:0] */
	uint32_t reg_ext_wakeup_event_mask;
};

enum {
	DISP_PWR_STA_MASK	= BIT(20),
	DISP_SRAM_PDN_MASK	= BIT(8),
	DISP_SRAM_ACK_MASK	= BIT(12),
	AUDIO_PWR_STA_MASK	= BIT(21),
	AUDIO_SRAM_PDN_MASK	= BIT(8),
	AUDIO_SRAM_ACK_MASK	= BIT(12),
};

struct mtk_spm_regs {
	u32 poweron_config_en;
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
	u8  reserved0[8];
	u32 spm_sw_rst_con;
	u32 spm_sw_rst_con_set;
	u32 spm_sw_rst_con_clr;
	u8  reserved1[4];
	u32 spm_arbiter_en;
	u32 scpsys_clk_con;
	u32 spm_sram_rsv_con;
	u32 spm_swint;
	u32 spm_swint_set;
	u32 spm_swint_clr;
	u32 spm_scp_mailbox;
	u32 scp_spm_mailbox;
	u32 spm_scp_irq;
	u32 spm_cpu_wakeup_event;
	u32 spm_irq_mask;
	u8  reserved2[4];
	u32 spm_src_req;
	u32 spm_src_mask;
	u32 spm_src2_mask;
	u8  reserved3[4];
	u32 spm_src3_mask;
	u32 spm_src4_mask;
	u32 spm_wakeup_event_mask2;
	u32 spm_wakeup_event_mask;
	u32 spm_wakeup_event_sens;
	u32 spm_wakeup_event_clear;
	u32 spm_wakeup_event_ext_mask;
	u32 scp_clk_con;
	u32 pcm_debug_con;
	u32 ddren_dbc_con;
	u32 spm_resource_ack_con0;
	u32 spm_resource_ack_con1;
	u32 spm_resource_ack_con2;
	u32 spm_resource_ack_con3;
	u32 spm_resource_ack_con4;
	u32 spm_sram_con;
	u8  reserved4[48];
	u32 pcm_reg0_data;
	u32 pcm_reg2_data;
	u32 pcm_reg6_data;
	u32 pcm_reg7_data;
	u32 pcm_reg13_data;
	u32 src_req_sta_0;
	u32 src_req_sta_1;
	u8  reserved5[4];
	u32 src_req_sta_2;
	u32 src_req_sta_3;
	u32 src_req_sta_4;
	u8  reserved6[4];
	u32 pcm_timer_out;
	u32 pcm_wdt_out;
	u32 spm_irq_sta;
	u32 md32pcm_wakeup_sta;
	u32 md32pcm_event_sta;
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_misc;
	u32 mm_dvfs_halt;
	u8  reserved7[16];
	u32 subsys_idle_sta;
	u32 pcm_sta;
	u32 pwr_status;
	u32 pwr_status_2nd;
	u32 cpu_pwr_status;
	u32 cpu_pwr_status_2nd;
	u32 spm_vtcxo_event_count_sta;
	u32 spm_infra_event_count_sta;
	u32 spm_vrf18_event_count_sta;
	u32 spm_apsrc_event_count_sta;
	u32 spm_ddren_event_count_sta;
	u32 md32pcm_sta;
	u32 md32pcm_pc;
	u32 other_pwr_status;
	u32 dvfsrc_event_sta;
	u32 bus_protect_rdy;
	u32 bus_protect1_rdy;
	u32 bus_protect2_rdy;
	u32 bus_protect3_rdy;
	u32 bus_protect4_rdy;
	u32 bus_protect5_rdy;
	u32 bus_protect6_rdy;
	u32 bus_protect7_rdy;
	u32 bus_protect8_rdy;
	u32 bus_protect9_rdy;
	u8  reserved8[8];
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
	u32 cpueb_pwr_con;
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
	u8  reserved9[12];
	u32 cpu_irq_mask;
	u32 cpu_irq_mask_set;
	u32 cpu_irq_mask_clr;
	u8  reserved10[20];
	u32 cpu_wfi_en;
	u32 cpu_wfi_en_set;
	u32 cpu_wfi_en_clr;
	u8  reserved11[4];
	u32 sysram_con;
	u32 sysrom_con;
	u8  reserved12[8];
	u32 root_cputop_addr;
	u32 root_core_addr;
	u8  reserved13[40];
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
	u32 mfg0_pwr_con;
	u32 mfg1_pwr_con;
	u32 mfg2_pwr_con;
	u32 mfg3_pwr_con;
	u32 mfg4_pwr_con;
	u32 mfg5_pwr_con;
	u32 ifr_pwr_con;
	u32 ifr_sub_pwr_con;
	u32 peri_pwr_con;
	u32 pextp_mac_top_p0_pwr_con;
	u32 pextp_phy_top_pwr_con;
	u32 aphy_n_pwr_con;
	u32 aphy_s_pwr_con;
	u8  reserved14[4];
	u32 ether_pwr_con;
	u32 dpy0_pwr_con;
	u32 dpy1_pwr_con;
	u32 dpm0_pwr_con;
	u32 dpm1_pwr_con;
	u32 audio_pwr_con;
	u32 audio_asrc_pwr_con;
	u32 adsp_pwr_con;
	u32 adsp_infra_pwr_con;
	u32 adsp_ao_pwr_con;
	u32 vppsys0_pwr_con;
	u32 vppsys1_pwr_con;
	u32 vdosys0_pwr_con;
	u32 vdosys1_pwr_con;
	u32 wpesys_pwr_con;
	u32 dp_tx_pwr_con;
	u32 edp_tx_pwr_con;
	u32 hdmi_tx_pwr_con;
	u32 vde0_pwr_con;
	u32 vde1_pwr_con;
	u32 vde2_pwr_con;
	u32 ven_pwr_con;
	u32 ven_core1_pwr_con;
	u32 cam_main_pwr_con;
	u32 cam_suba_pwr_con;
	u32 cam_subb_pwr_con;
	u32 cam_vcore_pwr_con;
	u32 img_vcore_pwr_con;
	u32 img_main_pwr_con;
	u32 img_dip_pwr_con;
	u32 img_ipe_pwr_con;
	u32 nna0_pwr_con;
	u32 nna1_pwr_con;
	u8  reserved15[4];
	u32 ipnna_pwr_con;
	u32 csi_rx_top_pwr_con;
	u8  reserved16[4];
	u32 sspm_sram_con;
	u32 scp_sram_con;
	u8  reserved17[4];
	u32 devapc_ifr_sram_con;
	u32 devapc_subifr_sram_con;
	u32 devapc_acp_sram_con;
	u32 usb_sram_con;
	u32 dummy_sram_con;
	u32 ext_buck_iso;
	u32 msdc_sram_con;
	u32 debugtop_sram_con;
	u32 dpmaif_sram_con;
	u32 gcpu_sram_con;
	u32 spm_mem_ck_sel;
	u32 spm_bus_protect_mask_b;
	u32 spm_bus_protect1_mask_b;
	u32 spm_bus_protect2_mask_b;
	u32 spm_bus_protect3_mask_b;
	u32 spm_bus_protect4_mask_b;
	u32 spm_bus_protect5_mask_b;
	u32 spm_bus_protect6_mask_b;
	u32 spm_bus_protect7_mask_b;
	u32 spm_bus_protect8_mask_b;
	u32 spm_bus_protect9_mask_b;
	u32 spm_emi_bw_mode;
	u8  reserved18[4];
	u32 spm2mm_con;
	u32 spm2cpueb_con;
	u32 ap_mdsrc_req;
	u32 spm2emi_enter_ulpm;
	u32 spm_pll_con;
	u32 rc_spm_ctrl;
	u32 spm_dram_mcu_sw_con_0;
	u32 spm_dram_mcu_sw_con_1;
	u32 spm_dram_mcu_sw_con_2;
	u32 spm_dram_mcu_sw_con_3;
	u32 spm_dram_mcu_sw_con_4;
	u32 spm_dram_mcu_sta_0;
	u32 spm_dram_mcu_sta_1;
	u32 spm_dram_mcu_sta_2;
	u32 spm_dram_mcu_sw_sel_0;
	u32 relay_dvfs_level;
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
	u32 dramc_mcu_sram_con;
	u32 dramc_mcu2_sram_con;
	u32 dpy_shu_sram_con;
	u32 dpy_shu2_sram_con;
	u8  reserved19[20];
	u32 spm_dpm_p2p_sta;
	u32 spm_dpm_p2p_con;
	u8  reserved20[228];
	u32 spm_sw_flag_0;
	u32 spm_sw_debug_0;
	u32 spm_sw_flag_1;
	u32 spm_sw_debug_1;
	u32 spm_sw_rsv_0;
	u32 spm_sw_rsv_1;
	u32 spm_sw_rsv_2;
	u32 spm_sw_rsv_3;
	u32 spm_sw_rsv_4;
	u32 spm_sw_rsv_5;
	u32 spm_sw_rsv_6;
	u32 spm_sw_rsv_7;
	u32 spm_sw_rsv_8;
	u32 spm_bk_wake_event;
	u32 spm_bk_vtcxo_dur;
	u32 spm_bk_wake_misc;
	u32 spm_bk_pcm_timer;
	u32 ulposc_con;
	u8  reserved21[8];
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
	u32 sc_mm_ck_sel_con;
	u32 spare_ack_mask;
	u8  reserved22[4];
	u32 spm_dv_con_0;
	u32 spm_dv_con_1;
	u32 spm_dv_sta;
	u32 conn_xowcn_debug_en;
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
	u32 spm_adsp_irq;
	u32 spm_md32_irq;
	u32 spm2pmcu_mailbox_0;
	u32 spm2pmcu_mailbox_1;
	u32 spm2pmcu_mailbox_2;
	u32 spm2pmcu_mailbox_3;
	u32 pmcu2spm_mailbox_0;
	u32 pmcu2spm_mailbox_1;
	u32 pmcu2spm_mailbox_2;
	u32 pmcu2spm_mailbox_3;
	u8  reserved23[12];
	u32 spm_ap_sema;
	u32 spm_spm_sema;
	u32 spm_dvfs_con;
	u32 spm_dvfs_con_sta;
	u32 spm_pmic_spmi_con;
	u8  reserved24[4];
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
	u32 spm_dvfs_cmd16;
	u32 spm_dvfs_cmd17;
	u32 spm_dvfs_cmd18;
	u32 spm_dvfs_cmd19;
	u32 spm_dvfs_cmd20;
	u32 spm_dvfs_cmd21;
	u32 spm_dvfs_cmd22;
	u32 spm_dvfs_cmd23;
	u32 sys_timer_value_l;
	u32 sys_timer_value_h;
	u32 sys_timer_start_l;
	u32 sys_timer_start_h;
	u32 sys_timer_latch_l_00;
	u32 sys_timer_latch_h_00;
	u32 sys_timer_latch_l_01;
	u32 sys_timer_latch_h_01;
	u32 sys_timer_latch_l_02;
	u32 sys_timer_latch_h_02;
	u32 sys_timer_latch_l_03;
	u32 sys_timer_latch_h_03;
	u32 sys_timer_latch_l_04;
	u32 sys_timer_latch_h_04;
	u32 sys_timer_latch_l_05;
	u32 sys_timer_latch_h_05;
	u32 sys_timer_latch_l_06;
	u32 sys_timer_latch_h_06;
	u32 sys_timer_latch_l_07;
	u32 sys_timer_latch_h_07;
	u32 sys_timer_latch_l_08;
	u32 sys_timer_latch_h_08;
	u32 sys_timer_latch_l_09;
	u32 sys_timer_latch_h_09;
	u32 sys_timer_latch_l_10;
	u32 sys_timer_latch_h_10;
	u32 sys_timer_latch_l_11;
	u32 sys_timer_latch_h_11;
	u32 sys_timer_latch_l_12;
	u32 sys_timer_latch_h_12;
	u32 sys_timer_latch_l_13;
	u32 sys_timer_latch_h_13;
	u32 sys_timer_latch_l_14;
	u32 sys_timer_latch_h_14;
	u32 sys_timer_latch_l_15;
	u32 sys_timer_latch_h_15;
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
	u32 pcm_wdt_latch_spare_0;
	u32 pcm_wdt_latch_spare_1;
	u32 pcm_wdt_latch_spare_2;
	u8  reserved25[72];
	u32 dramc_gating_err_latch_ch0_0;
	u32 dramc_gating_err_latch_ch0_1;
	u32 dramc_gating_err_latch_ch0_2;
	u32 dramc_gating_err_latch_ch0_3;
	u32 dramc_gating_err_latch_ch0_4;
	u32 dramc_gating_err_latch_ch0_5;
	u8  reserved26[60];
	u32 dramc_gating_err_latch_spare_0;
	u8  reserved27[8];
	u32 spm_ack_chk_con_0;
	u32 spm_ack_chk_pc_0;
	u32 spm_ack_chk_sel_0;
	u32 spm_ack_chk_timer_0;
	u32 spm_ack_chk_sta_0;
	u32 spm_ack_chk_swint_0;
	u8  reserved28[8];
	u32 spm_ack_chk_con_1;
	u32 spm_ack_chk_pc_1;
	u32 spm_ack_chk_sel_1;
	u32 spm_ack_chk_timer_1;
	u32 spm_ack_chk_sta_1;
	u32 spm_ack_chk_swint_1;
	u8  reserved29[8];
	u32 spm_ack_chk_con_2;
	u32 spm_ack_chk_pc_2;
	u32 spm_ack_chk_sel_2;
	u32 spm_ack_chk_timer_2;
	u32 spm_ack_chk_sta_2;
	u32 spm_ack_chk_swint_2;
	u8  reserved30[8];
	u32 spm_ack_chk_con_3;
	u32 spm_ack_chk_pc_3;
	u32 spm_ack_chk_sel_3;
	u32 spm_ack_chk_timer_3;
	u32 spm_ack_chk_sta_3;
	u32 spm_ack_chk_swint_3;
	u32 spm_counter_0;
	u32 spm_counter_1;
	u32 spm_counter_2;
	u8  reserved31[8];
	u32 sys_timer_con;
	u32 spm_twam_con;
	u32 spm_twam_window_len;
	u32 spm_twam_idle_sel;
	u32 spm_twam_event_clear;
	u8  reserved32[1376];
	u32 pmsr_last_dat;
	u32 pmsr_last_cnt;
	u32 pmsr_last_ack;
	u8  reserved33[4];
	u32 spm_pmsr_sel_con0;
	u32 spm_pmsr_sel_con1;
	u32 spm_pmsr_sel_con2;
	u32 spm_pmsr_sel_con3;
	u32 spm_pmsr_sel_con4;
	u32 spm_pmsr_sel_con5;
	u32 spm_pmsr_sel_con6;
	u32 spm_pmsr_sel_con7;
	u32 spm_pmsr_sel_con8;
	u32 spm_pmsr_sel_con9;
	u8  reserved34[4];
	u32 spm_pmsr_sel_con10;
	u32 spm_pmsr_sel_con11;
	u8  reserved35[116];
	u32 spm_pmsr_tiemr_sta0;
	u32 spm_pmsr_tiemr_sta1;
	u32 spm_pmsr_tiemr_sta2;
	u32 spm_pmsr_general_con0;
	u32 spm_pmsr_general_con1;
	u32 spm_pmsr_general_con2;
	u32 spm_pmsr_general_con3;
	u32 spm_pmsr_general_con4;
	u32 spm_pmsr_general_con5;
	u32 spm_pmsr_sw_reset;
	u32 spm_pmsr_mon_con0;
	u32 spm_pmsr_mon_con1;
	u32 spm_pmsr_mon_con2;
	u32 spm_pmsr_len_con0;
	u32 spm_pmsr_len_con1;
	u32 spm_pmsr_len_con2;
};

check_member(mtk_spm_regs, pwr_status, 0x016c);
check_member(mtk_spm_regs, audio_pwr_con, 0x034c);
check_member(mtk_spm_regs, adsp_pwr_con, 0x0354);
check_member(mtk_spm_regs, ap_mdsrc_req, 0x043c);
check_member(mtk_spm_regs, ulposc_con, 0x644);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

struct pcm_desc {
	u32 pmem_words;
	u32 total_words;
	u32 pmem_start;
	u32 dmem_start;
};

struct dyna_load_pcm {
	u8 *buf;		/* binary array */
	struct pcm_desc desc;
};

static const struct power_domain_data disp[] = {
	{
		.pwr_con = &mtk_spm->vppsys0_pwr_con,
		.pwr_sta_mask = BIT(11),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
	{
		.pwr_con = &mtk_spm->vdosys0_pwr_con,
		.pwr_sta_mask = BIT(13),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
	{
		.pwr_con = &mtk_spm->vppsys1_pwr_con,
		.pwr_sta_mask = BIT(12),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
	{
		.pwr_con = &mtk_spm->vdosys1_pwr_con,
		.pwr_sta_mask = BIT(14),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
	{
		.pwr_con = &mtk_spm->edp_tx_pwr_con,
		.pwr_sta_mask = BIT(17),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
};

static const struct power_domain_data audio[] = {
	{
		.pwr_con = &mtk_spm->adsp_pwr_con,
		.pwr_sta_mask = BIT(10),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
	{
		.pwr_con = &mtk_spm->audio_pwr_con,
		.pwr_sta_mask = BIT(8),
		.sram_pdn_mask = BIT(8),
		.sram_ack_mask = BIT(12),
	},
};

int spm_init(void);

#endif  /* SOC_MEDIATEK_MT8188_SPM_H */
