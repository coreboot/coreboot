/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.5
 */

#ifndef SOC_MEDIATEK_MT8186_SPM_H
#define SOC_MEDIATEK_MT8186_SPM_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <soc/spm_common.h>
#include <types.h>

#define SPM_INIT_DONE_US		20

#define CLK_SCP_CFG_0			(IO_PHYS + 0x200)
#define CLK_SCP_CFG_1			(IO_PHYS + 0x210)
#define INFRA_AO_RES_CTRL_MASK		(INFRACFG_AO_BASE + 0xB8)

#define AP_PLL_CON3			(APMIXED_BASE + 0xC)
#define AP_PLL_CON4			(APMIXED_BASE + 0x10)

/* MD32PCM setting for SPM code fetch */
#define MD32PCM_CFGREG_SW_RSTN_RUN	1
#define MD32PCM_DMA0_CON_VAL		0x0003820E
#define MD32PCM_DMA0_START_VAL		0x00008000

/* SPM */
#define BCLK_CG_EN_LSB			BIT(0)
#define PCM_CK_EN_LSB			BIT(2)
#define PCM_SW_RESET_LSB		BIT(15)
#define RG_AHBMIF_APBEN_LSB		BIT(3)
#define REG_MD32_APB_INTERNAL_EN_LSB	BIT(14)
#define PCM_RF_SYNC_R7			BIT(23)
#define REG_DDREN_DBC_EN_LSB		BIT(16)

DEFINE_BIT(MD32PCM_CFGREG_SW_RSTN_RESET, 0)
DEFINE_BIT(REG_SYSCLK1_SRC_MD2_SRCCLKENA, 28)
DEFINE_BIT(SPM_ACK_CHK_3_CON_CLR_ALL, 1)
DEFINE_BIT(SPM_ACK_CHK_3_CON_EN_0, 4)
DEFINE_BIT(SPM_ACK_CHK_3_CON_EN_1, 8)
DEFINE_BIT(SPM_ACK_CHK_3_CON_HW_MODE_TRIG_0, 9)
DEFINE_BIT(SPM_ACK_CHK_3_CON_HW_MODE_TRIG_1, 10)
DEFINE_BIT(INFRA_AO_RES_CTRL_MASK_EMI_IDLE, 18)
DEFINE_BIT(INFRA_AO_RES_CTRL_MASK_MPU_IDLE, 15)
DEFINE_BIT(SPM_DVFS_FORCE_ENABLE_LSB, 2)
DEFINE_BIT(SPM_DVFSRC_ENABLE_LSB, 4)
DEFINE_BIT(SYS_TIMER_START_EN_LSB, 0)

#define SPM_PROJECT_CODE		0xB16
#define SPM_REGWR_CFG_KEY		(SPM_PROJECT_CODE << 16)
#define POWER_ON_VAL1_DEF		0x80015860
#define SPM_WAKEUP_EVENT_MASK_DEF	0xEFFFFFFF
#define DDREN_DBC_EN_VAL		0x154
#define ARMPLL_CLK_SEL_DEF		0x3FF
#define SPM_RESOURCE_ACK_CON0_DEF	0x00000000
#define SPM_RESOURCE_ACK_CON1_DEF	0x00000000
#define SPM_RESOURCE_ACK_CON2_DEF	0xCCCC4E4E
#define SPM_RESOURCE_ACK_CON3_DEF	0x00000000
#define APMIX_CON3_DEF			0xFFFF7770
#define APMIX_CON4_DEF			0xFFFAA007
#define SCP_CFG0_DEF			0x3FF
#define SCP_CFG1_DEF			0x3
#define SPM_DVFS_LEVEL_DEF		0x00000001
#define SPM_DVS_DFS_LEVEL_DEF		0x00010001
#define SPM_ACK_CHK_3_SEL_HW_S1		0x0035009F
#define SPM_ACK_CHK_3_HW_S1_CNT		1
#define SPM_SYSCLK_SETTLE		0x60FE /* 1685us */
#define SPM_WAKEUP_EVENT_MASK_BIT0	1
#define RG_PCM_TIMER_EN_LSB		BIT(5)
#define RG_PCM_WDT_WAKE_LSB		BIT(9)
#define PCM_RF_SYNC_R0			BIT(16)
#define REG_SPM_EVENT_COUNTER_CLR_LSB	BIT(6)
#define R12_CSYSPWREQ_B			BIT(24)
#define SPM_BUS_PROTECT_MASK_B_DEF	0xFFFFFFFF
#define SPM_BUS_PROTECT2_MASK_B_DEF	0xFFFFFFFF

#define SPM_FLAG_DISABLE_VCORE_DVS	BIT(3)
#define SPM_FLAG_DISABLE_VCORE_DFS	BIT(4)
#define SPM_FLAG_RUN_COMMON_SCENARIO	BIT(10)

/* PCM_WDT_VAL */
#define PCM_WDT_TIMEOUT			(30 * 32768) /* 30s */
/* PCM_TIMER_VAL */
#define PCM_TIMER_MAX			(0xffffffff - PCM_WDT_TIMEOUT)

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
#define ISRM_RET_IRQ_AUX	(ISRM_RET_IRQ9 | ISRM_RET_IRQ8 | ISRM_RET_IRQ7 | \
				 ISRM_RET_IRQ6 | ISRM_RET_IRQ5 | ISRM_RET_IRQ4 | \
				 ISRM_RET_IRQ3 | ISRM_RET_IRQ2 | ISRM_RET_IRQ1)
#define ISRM_ALL_EXC_TWAM	ISRM_RET_IRQ_AUX
#define ISRM_ALL		(ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM		BIT(2)
#define ISRS_PCM_RETURN		BIT(3)
#define ISRC_TWAM		ISRS_TWAM
#define ISRC_ALL_EXC_TWAM	ISRS_PCM_RETURN
#define ISRC_ALL		(ISRC_ALL_EXC_TWAM | ISRC_TWAM)

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

struct mtk_spm_regs {
	uint32_t poweron_config_set;
	uint32_t spm_power_on_val0;
	uint32_t spm_power_on_val1;
	uint32_t spm_clk_con;
	uint32_t spm_clk_settle;
	uint32_t spm_ap_standby_con;
	uint32_t pcm_con0;
	uint32_t pcm_con1;
	uint32_t spm_power_on_val2;
	uint32_t spm_power_on_val3;
	uint32_t pcm_reg_data_ini;
	uint32_t pcm_pwr_io_en;
	uint32_t pcm_timer_val;
	uint32_t pcm_wdt_val;
	uint8_t  reserved0[8];
	uint32_t spm_sw_rst_con;
	uint32_t spm_sw_rst_con_set;
	uint32_t spm_sw_rst_con_clr;
	uint32_t spm_src6_mask;
	uint8_t  reserved1[52];
	uint32_t md32_clk_con;
	uint32_t spm_sram_rsv_con;
	uint32_t spm_swint;
	uint32_t spm_swint_set;
	uint32_t spm_swint_clr;
	uint32_t spm_scp_mailbox;
	uint32_t scp_spm_mailbox;
	uint32_t spm_wakeup_event_sens;
	uint32_t spm_wakeup_event_clear;
	uint8_t  reserved2[4];
	uint32_t spm_scp_irq;
	uint32_t spm_cpu_wakeup_event;
	uint32_t spm_irq_mask;
	uint32_t spm_src_req;
	uint32_t spm_src_mask;
	uint32_t spm_src2_mask;
	uint32_t spm_src3_mask;
	uint32_t spm_src4_mask;
	uint32_t spm_src5_mask;
	uint32_t spm_wakeup_event_mask;
	uint32_t spm_wakeup_event_ext_mask;
	uint32_t spm_src7_mask;
	uint32_t scp_clk_con;
	uint32_t pcm_debug_con;
	uint8_t  reserved3[4];
	uint32_t ddren_dbc_con;
	uint32_t spm_resource_ack_con4;
	uint32_t spm_resource_ack_con0;
	uint32_t spm_resource_ack_con1;
	uint32_t spm_resource_ack_con2;
	uint32_t spm_resource_ack_con3;
	uint32_t pcm_reg0_data;
	uint32_t pcm_reg2_data;
	uint32_t pcm_reg6_data;
	uint32_t pcm_reg7_data;
	uint32_t pcm_reg13_data;
	uint32_t src_req_sta_0;
	uint32_t src_req_sta_1;
	uint32_t src_req_sta_2;
	uint32_t pcm_timer_out;
	uint32_t pcm_wdt_out;
	uint32_t spm_irq_sta;
	uint32_t src_req_sta_4;
	uint32_t md32pcm_wakeup_sta;
	uint32_t md32pcm_event_sta;
	uint32_t spm_wakeup_sta;
	uint32_t spm_wakeup_ext_sta;
	uint32_t spm_wakeup_misc;
	uint32_t mm_dvfs_halt;
	uint8_t  reserved4[8];
	uint32_t bus_protect_rdy;
	uint32_t bus_protect1_rdy;
	uint32_t bus_protect2_rdy;
	uint32_t bus_protect3_rdy;
	uint32_t subsys_idle_sta;
	uint32_t pcm_sta;
	uint32_t src_req_sta_3;
	uint32_t pwr_status;
	uint32_t pwr_status_2nd;
	uint32_t cpu_pwr_status;
	uint32_t other_pwr_status;
	uint32_t spm_vtcxo_event_count_sta;
	uint32_t spm_infra_event_count_sta;
	uint32_t spm_vrf18_event_count_sta;
	uint32_t spm_apsrc_event_count_sta;
	uint32_t spm_ddren_event_count_sta;
	uint32_t md32pcm_sta;
	uint32_t md32pcm_pc;
	uint8_t  reserved5[12];
	uint32_t dvfsrc_event_sta;
	uint32_t bus_protect4_rdy;
	uint32_t bus_protect5_rdy;
	uint32_t bus_protect6_rdy;
	uint32_t bus_protect7_rdy;
	uint32_t bus_protect8_rdy;
	uint8_t  reserved6[20];
	uint32_t spm_twam_last_sta0;
	uint32_t spm_twam_last_sta1;
	uint32_t spm_twam_last_sta2;
	uint32_t spm_twam_last_sta3;
	uint32_t spm_twam_curr_sta0;
	uint32_t spm_twam_curr_sta1;
	uint32_t spm_twam_curr_sta2;
	uint32_t spm_twam_curr_sta3;
	uint32_t spm_twam_timer_out;
	uint32_t spm_cg_check_sta;
	uint32_t spm_dvfs_sta;
	uint32_t spm_dvfs_opp_sta;
	uint32_t spm_mcusys_pwr_con;
	uint32_t spm_cputop_pwr_con;
	uint32_t spm_cpu0_pwr_con;
	uint32_t spm_cpu1_pwr_con;
	uint32_t spm_cpu2_pwr_con;
	uint32_t spm_cpu3_pwr_con;
	uint32_t spm_cpu4_pwr_con;
	uint32_t spm_cpu5_pwr_con;
	uint32_t spm_cpu6_pwr_con;
	uint32_t spm_cpu7_pwr_con;
	uint8_t  reserved7[4];
	uint32_t armpll_clk_con;
	uint32_t mcusys_idle_sta;
	uint32_t gic_wakeup_sta;
	uint32_t cpu_spare_con;
	uint32_t cpu_spare_con_set;
	uint32_t cpu_spare_con_clr;
	uint32_t armpll_clk_sel;
	uint32_t ext_int_wakeup_req;
	uint32_t ext_int_wakeup_req_set;
	uint32_t ext_int_wakeup_req_clr;
	uint8_t  reserved8[12];
	uint32_t cpu_irq_mask;
	uint32_t cpu_irq_mask_set;
	uint32_t cpu_irq_mask_clr;
	uint8_t  reserved9[20];
	uint32_t cpu_wfi_en;
	uint32_t cpu_wfi_en_set;
	uint32_t cpu_wfi_en_clr;
	uint8_t  reserved10[20];
	uint32_t root_cputop_addr;
	uint32_t root_core_addr;
	uint8_t  reserved11[40];
	uint32_t spm2sw_mailbox_0;
	uint32_t spm2sw_mailbox_1;
	uint32_t spm2sw_mailbox_2;
	uint32_t spm2sw_mailbox_3;
	uint32_t sw2spm_wakeup;
	uint32_t sw2spm_wakeup_set;
	uint32_t sw2spm_wakeup_clr;
	uint32_t sw2spm_mailbox_0;
	uint32_t sw2spm_mailbox_1;
	uint32_t sw2spm_mailbox_2;
	uint32_t sw2spm_mailbox_3;
	uint32_t sw2spm_cfg;
	uint32_t md1_pwr_con;
	uint32_t conn_pwr_con;
	uint32_t mfg0_pwr_con;
	uint32_t mfg1_pwr_con;
	uint32_t mfg2_pwr_con;
	uint32_t mfg3_pwr_con;
	uint32_t mfg4_pwr_con;
	uint32_t mfg5_pwr_con;
	uint32_t mfg6_pwr_con;
	uint32_t ifr_pwr_con;
	uint32_t ifr_sub_pwr_con;
	uint32_t dpy_pwr_con;
	uint32_t dramc_md32_pwr_con;
	uint32_t isp_pwr_con;
	uint32_t isp2_pwr_con;
	uint32_t ipe_pwr_con;
	uint32_t vde_pwr_con;
	uint32_t vde2_pwr_con;
	uint32_t ven_pwr_con;
	uint32_t ven_core1_pwr_con;
	uint32_t mdp_pwr_con;
	uint32_t dis_pwr_con;
	uint32_t audio_pwr_con;
	uint32_t cam_pwr_con;
	uint32_t cam_rawa_pwr_con;
	uint32_t cam_rawb_pwr_con;
	uint32_t cam_rawc_pwr_con;
	uint32_t sysram_con;
	uint32_t sysrom_con;
	uint32_t sspm_sram_con;
	uint32_t scp_sram_con;
	uint32_t dpy_shu_sram_con;
	uint32_t ufs_sram_con;
	uint32_t devapc_ifr_sram_con;
	uint32_t devapc_subifr_sram_con;
	uint32_t devapc_acp_sram_con;
	uint32_t usb_sram_con;
	uint32_t dummy_sram_con;
	uint32_t md_ext_buck_iso_con;
	uint32_t ext_buck_iso;
	uint32_t dxcc_sram_con;
	uint32_t msdc_pwr_con;
	uint32_t debugtop_sram_con;
	uint32_t dp_tx_pwr_con;
	uint32_t dpmaif_sram_con;
	uint32_t dpy_shu2_sram_con;
	uint32_t dramc_mcu2_sram_con;
	uint32_t dramc_mcu_sram_con;
	uint32_t mcupm_pwr_con;
	uint32_t dpy2_pwr_con;
	uint32_t spm_sram_con;
	uint8_t  reserved12[4];
	uint32_t peri_pwr_con;
	uint32_t nna0_pwr_con;
	uint32_t nna1_pwr_con;
	uint32_t nna2_pwr_con;
	uint32_t nna_pwr_con;
	uint32_t adsp_pwr_con;
	uint32_t dpy_sram_con;
	uint32_t nna3_pwr_con;
	uint8_t  reserved13[8];
	uint32_t wpe_pwr_con;
	uint8_t  reserved14[4];
	uint32_t spm_mem_ck_sel;
	uint32_t spm_bus_protect_mask_b;
	uint32_t spm_bus_protect1_mask_b;
	uint32_t spm_bus_protect2_mask_b;
	uint32_t spm_bus_protect3_mask_b;
	uint32_t spm_bus_protect4_mask_b;
	uint32_t spm_emi_bw_mode;
	uint32_t ap2md_peer_wakeup;
	uint32_t ulposc_con;
	uint32_t spm2mm_con;
	uint32_t spm_bus_protect5_mask_b;
	uint32_t spm2mcupm_con;
	uint32_t ap_mdsrc_req;
	uint32_t spm2emi_enter_ulpm;
	uint32_t spm2md_dvfs_con;
	uint32_t md2spm_dvfs_con;
	uint32_t spm_bus_protect6_mask_b;
	uint32_t spm_bus_protect7_mask_b;
	uint32_t spm_bus_protect8_mask_b;
	uint32_t spm_pll_con;
	uint32_t rc_spm_ctrl;
	uint32_t spm_dram_mcu_sw_con_0;
	uint32_t spm_dram_mcu_sw_con_1;
	uint32_t spm_dram_mcu_sw_con_2;
	uint32_t spm_dram_mcu_sw_con_3;
	uint32_t spm_dram_mcu_sw_con_4;
	uint32_t spm_dram_mcu_sta_0;
	uint32_t spm_dram_mcu_sta_1;
	uint32_t spm_dram_mcu_sta_2;
	uint32_t spm_dram_mcu_sw_sel_0;
	uint32_t relay_dvfs_level;
	uint8_t  reserved15[4];
	uint32_t dramc_dpy_clk_sw_con_0;
	uint32_t dramc_dpy_clk_sw_con_1;
	uint32_t dramc_dpy_clk_sw_con_2;
	uint32_t dramc_dpy_clk_sw_con_3;
	uint32_t dramc_dpy_clk_sw_sel_0;
	uint32_t dramc_dpy_clk_sw_sel_1;
	uint32_t dramc_dpy_clk_sw_sel_2;
	uint32_t dramc_dpy_clk_sw_sel_3;
	uint32_t dramc_dpy_clk_spm_con;
	uint32_t spm_dvfs_level;
	uint32_t spm_cirq_con;
	uint32_t spm_dvfs_misc;
	uint8_t  reserved16[4];
	uint32_t rg_module_sw_cg_0_mask_req_0;
	uint32_t rg_module_sw_cg_0_mask_req_1;
	uint32_t rg_module_sw_cg_0_mask_req_2;
	uint32_t rg_module_sw_cg_1_mask_req_0;
	uint32_t rg_module_sw_cg_1_mask_req_1;
	uint32_t rg_module_sw_cg_1_mask_req_2;
	uint32_t rg_module_sw_cg_2_mask_req_0;
	uint32_t rg_module_sw_cg_2_mask_req_1;
	uint32_t rg_module_sw_cg_2_mask_req_2;
	uint32_t rg_module_sw_cg_3_mask_req_0;
	uint32_t rg_module_sw_cg_3_mask_req_1;
	uint32_t rg_module_sw_cg_3_mask_req_2;
	uint32_t pwr_status_mask_req_0;
	uint32_t pwr_status_mask_req_1;
	uint32_t pwr_status_mask_req_2;
	uint32_t spm_cg_check_con;
	uint32_t spm_src_rdy_sta;
	uint32_t spm_dvs_dfs_level;
	uint32_t spm_force_dvfs;
	uint8_t  reserved17[256];
	uint32_t spm_sw_flag_0;
	uint32_t spm_sw_debug_0;
	uint32_t spm_sw_flag_1;
	uint32_t spm_sw_debug_1;
	uint32_t spm_sw_rsv_0;
	uint32_t spm_sw_rsv_1;
	uint32_t spm_sw_rsv_2;
	uint32_t spm_sw_rsv_3;
	uint32_t spm_sw_rsv_4;
	uint32_t spm_sw_rsv_5;
	uint32_t spm_sw_rsv_6;
	uint32_t spm_sw_rsv_7;
	uint32_t spm_sw_rsv_8;
	uint32_t spm_bk_wake_event;
	uint32_t spm_bk_vtcxo_dur;
	uint32_t spm_bk_wake_misc;
	uint32_t spm_bk_pcm_timer;
	uint8_t  reserved18[12];
	uint32_t spm_rsv_con_0;
	uint32_t spm_rsv_con_1;
	uint32_t spm_rsv_sta_0;
	uint32_t spm_rsv_sta_1;
	uint32_t spm_spare_con;
	uint32_t spm_spare_con_set;
	uint32_t spm_spare_con_clr;
	uint32_t spm_cross_wake_m00_req;
	uint32_t spm_cross_wake_m01_req;
	uint32_t spm_cross_wake_m02_req;
	uint32_t spm_cross_wake_m03_req;
	uint32_t scp_vcore_level;
	uint32_t sc_mm_ck_sel_con;
	uint32_t spare_ack_mask;
	uint32_t spm_spare_function;
	uint32_t spm_dv_con_0;
	uint32_t spm_dv_con_1;
	uint32_t spm_dv_sta;
	uint32_t conn_xowcn_debug_en;
	uint32_t spm_sema_m0;
	uint32_t spm_sema_m1;
	uint32_t spm_sema_m2;
	uint32_t spm_sema_m3;
	uint32_t spm_sema_m4;
	uint32_t spm_sema_m5;
	uint32_t spm_sema_m6;
	uint32_t spm_sema_m7;
	uint32_t spm2adsp_mailbox;
	uint32_t adsp2spm_mailbox;
	uint32_t spm_adsp_irq;
	uint32_t spm_md32_irq;
	uint32_t spm2pmcu_mailbox_0;
	uint32_t spm2pmcu_mailbox_1;
	uint32_t spm2pmcu_mailbox_2;
	uint32_t spm2pmcu_mailbox_3;
	uint32_t pmcu2spm_mailbox_0;
	uint32_t pmcu2spm_mailbox_1;
	uint32_t pmcu2spm_mailbox_2;
	uint32_t pmcu2spm_mailbox_3;
	uint32_t ufs_psri_sw;
	uint32_t ufs_psri_sw_set;
	uint32_t ufs_psri_sw_clr;
	uint32_t spm_ap_sema;
	uint32_t spm_spm_sema;
	uint32_t spm_dvfs_con;
	uint32_t spm_dvfs_con_sta;
	uint32_t spm_pmic_spmi_con;
	uint8_t  reserved19[4];
	uint32_t spm_dvfs_cmd0;
	uint32_t spm_dvfs_cmd1;
	uint32_t spm_dvfs_cmd2;
	uint32_t spm_dvfs_cmd3;
	uint32_t spm_dvfs_cmd4;
	uint32_t spm_dvfs_cmd5;
	uint32_t spm_dvfs_cmd6;
	uint32_t spm_dvfs_cmd7;
	uint32_t spm_dvfs_cmd8;
	uint32_t spm_dvfs_cmd9;
	uint32_t spm_dvfs_cmd10;
	uint32_t spm_dvfs_cmd11;
	uint32_t spm_dvfs_cmd12;
	uint32_t spm_dvfs_cmd13;
	uint32_t spm_dvfs_cmd14;
	uint32_t spm_dvfs_cmd15;
	uint32_t spm_dvfs_cmd16;
	uint32_t spm_dvfs_cmd17;
	uint32_t spm_dvfs_cmd18;
	uint32_t spm_dvfs_cmd19;
	uint32_t spm_dvfs_cmd20;
	uint32_t spm_dvfs_cmd21;
	uint32_t spm_dvfs_cmd22;
	uint32_t spm_dvfs_cmd23;
	uint32_t sys_timer_value_l;
	uint32_t sys_timer_value_h;
	uint32_t sys_timer_start_l;
	uint32_t sys_timer_start_h;
	uint32_t sys_timer_latch_l_00;
	uint32_t sys_timer_latch_h_00;
	uint32_t sys_timer_latch_l_01;
	uint32_t sys_timer_latch_h_01;
	uint32_t sys_timer_latch_l_02;
	uint32_t sys_timer_latch_h_02;
	uint32_t sys_timer_latch_l_03;
	uint32_t sys_timer_latch_h_03;
	uint32_t sys_timer_latch_l_04;
	uint32_t sys_timer_latch_h_04;
	uint32_t sys_timer_latch_l_05;
	uint32_t sys_timer_latch_h_05;
	uint32_t sys_timer_latch_l_06;
	uint32_t sys_timer_latch_h_06;
	uint32_t sys_timer_latch_l_07;
	uint32_t sys_timer_latch_h_07;
	uint32_t sys_timer_latch_l_08;
	uint32_t sys_timer_latch_h_08;
	uint32_t sys_timer_latch_l_09;
	uint32_t sys_timer_latch_h_09;
	uint32_t sys_timer_latch_l_10;
	uint32_t sys_timer_latch_h_10;
	uint32_t sys_timer_latch_l_11;
	uint32_t sys_timer_latch_h_11;
	uint32_t sys_timer_latch_l_12;
	uint32_t sys_timer_latch_h_12;
	uint32_t sys_timer_latch_l_13;
	uint32_t sys_timer_latch_h_13;
	uint32_t sys_timer_latch_l_14;
	uint32_t sys_timer_latch_h_14;
	uint32_t sys_timer_latch_l_15;
	uint32_t sys_timer_latch_h_15;
	uint32_t pcm_wdt_latch_0;
	uint32_t pcm_wdt_latch_1;
	uint32_t pcm_wdt_latch_2;
	uint32_t pcm_wdt_latch_3;
	uint32_t pcm_wdt_latch_4;
	uint32_t pcm_wdt_latch_5;
	uint32_t pcm_wdt_latch_6;
	uint32_t pcm_wdt_latch_7;
	uint32_t pcm_wdt_latch_8;
	uint32_t pcm_wdt_latch_9;
	uint32_t pcm_wdt_latch_10;
	uint32_t pcm_wdt_latch_11;
	uint32_t pcm_wdt_latch_12;
	uint32_t pcm_wdt_latch_13;
	uint32_t pcm_wdt_latch_14;
	uint32_t pcm_wdt_latch_15;
	uint32_t pcm_wdt_latch_16;
	uint32_t pcm_wdt_latch_17;
	uint32_t pcm_wdt_latch_18;
	uint32_t pcm_wdt_latch_spare_0;
	uint32_t pcm_wdt_latch_spare_1;
	uint32_t pcm_wdt_latch_spare_2;
	uint8_t  reserved20[24];
	uint32_t pcm_wdt_latch_conn_0;
	uint32_t pcm_wdt_latch_conn_1;
	uint32_t pcm_wdt_latch_conn_2;
	uint8_t  reserved21[36];
	uint32_t dramc_gating_err_latch_ch0_0;
	uint32_t dramc_gating_err_latch_ch0_1;
	uint32_t dramc_gating_err_latch_ch0_2;
	uint32_t dramc_gating_err_latch_ch0_3;
	uint32_t dramc_gating_err_latch_ch0_4;
	uint32_t dramc_gating_err_latch_ch0_5;
	uint32_t dramc_gating_err_latch_ch0_6;
	uint8_t  reserved22[56];
	uint32_t dramc_gating_err_latch_spare_0;
	uint8_t  reserved23[8];
	uint32_t spm_ack_chk_con_0;
	uint32_t spm_ack_chk_pc_0;
	uint32_t spm_ack_chk_sel_0;
	uint32_t spm_ack_chk_timer_0;
	uint32_t spm_ack_chk_sta_0;
	uint32_t spm_ack_chk_swint_0;
	uint32_t spm_ack_chk_con_1;
	uint32_t spm_ack_chk_pc_1;
	uint32_t spm_ack_chk_sel_1;
	uint32_t spm_ack_chk_timer_1;
	uint32_t spm_ack_chk_sta_1;
	uint32_t spm_ack_chk_swint_1;
	uint32_t spm_ack_chk_con_2;
	uint32_t spm_ack_chk_pc_2;
	uint32_t spm_ack_chk_sel_2;
	uint32_t spm_ack_chk_timer_2;
	uint32_t spm_ack_chk_sta_2;
	uint32_t spm_ack_chk_swint_2;
	uint32_t spm_ack_chk_con_3;
	uint32_t spm_ack_chk_pc_3;
	uint32_t spm_ack_chk_sel_3;
	uint32_t spm_ack_chk_timer_3;
	uint32_t spm_ack_chk_sta_3;
	uint32_t spm_ack_chk_swint_3;
	uint32_t spm_counter_0;
	uint32_t spm_counter_1;
	uint32_t spm_counter_2;
	uint32_t sys_timer_con;
	uint32_t spm_twam_con;
	uint32_t spm_twam_window_len;
	uint32_t spm_twam_idle_sel;
	uint32_t spm_twam_event_clear;
	uint32_t opp0_table;
	uint32_t opp1_table;
	uint32_t opp2_table;
	uint32_t opp3_table;
	uint32_t opp4_table;
	uint32_t opp5_table;
	uint32_t opp6_table;
	uint32_t opp7_table;
	uint32_t opp8_table;
	uint32_t opp9_table;
	uint32_t opp10_table;
	uint32_t opp11_table;
	uint32_t opp12_table;
	uint32_t opp13_table;
	uint32_t opp14_table;
	uint32_t opp15_table;
	uint32_t opp16_table;
	uint32_t opp17_table;
	uint32_t shu0_array;
	uint32_t shu1_array;
	uint32_t shu2_array;
	uint32_t shu3_array;
	uint32_t shu4_array;
	uint32_t shu5_array;
	uint32_t shu6_array;
	uint32_t shu7_array;
	uint32_t shu8_array;
	uint32_t shu9_array;
	uint32_t ssusb_top_pwr_con;
	uint32_t ssusb_top_p1_pwr_con;
	uint32_t adsp_infra_pwr_con;
	uint32_t adsp_ao_pwr_con;
	uint32_t md32pcm_cfgreg_sw_rstn;
	uint8_t reserved_6a04[0x200 - 4];
	uint32_t md32pcm_dma0_src;
	uint32_t md32pcm_dma0_dst;
	uint32_t md32pcm_dma0_wppt;
	uint32_t md32pcm_dma0_wpto;
	uint32_t md32pcm_dma0_count;
	uint32_t md32pcm_dma0_con;
	uint32_t md32pcm_dma0_start;
	uint8_t reserved_6c1c[8];
	uint32_t md32pcm_dma0_rlct;
};

struct pwr_ctrl {
	/* For SPM */
	uint32_t pcm_flags;
	uint32_t pcm_flags_cust;
	uint32_t pcm_flags_cust_set;
	uint32_t pcm_flags_cust_clr;
	uint32_t pcm_flags1;
	uint32_t pcm_flags1_cust;
	uint32_t pcm_flags1_cust_set;
	uint32_t pcm_flags1_cust_clr;
	uint32_t timer_val;
	uint32_t timer_val_cust;
	uint32_t timer_val_ramp_en;
	uint32_t timer_val_ramp_en_sec;
	uint32_t wake_src;
	uint32_t wake_src_cust;
	uint32_t wakelock_timer_val;
	uint8_t wdt_disable;
	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	uint8_t reg_wfi_op;
	uint8_t reg_wfi_type;
	uint8_t reg_mp0_cputop_idle_mask;
	uint8_t reg_mp1_cputop_idle_mask;
	uint8_t reg_mcusys_idle_mask;
	uint8_t reg_md_apsrc_1_sel;
	uint8_t reg_md_apsrc_0_sel;
	uint8_t reg_conn_apsrc_sel;

	/* SPM_SRC6_MASK */
	uint32_t reg_ccif_event_infra_req_mask_b;
	uint32_t reg_ccif_event_apsrc_req_mask_b;

	/* SPM_SRC_REQ */
	uint8_t reg_spm_apsrc_req;
	uint8_t reg_spm_f26m_req;
	uint8_t reg_spm_infra_req;
	uint8_t reg_spm_vrf18_req;
	uint8_t reg_spm_ddren_req;
	uint8_t reg_spm_dvfs_req;
	uint8_t reg_spm_sw_mailbox_req;
	uint8_t reg_spm_sspm_mailbox_req;
	uint8_t reg_spm_adsp_mailbox_req;
	uint8_t reg_spm_scp_mailbox_req;

	/* SPM_SRC_MASK */
	uint8_t reg_md_0_srcclkena_mask_b;
	uint8_t reg_md_0_infra_req_mask_b;
	uint8_t reg_md_0_apsrc_req_mask_b;
	uint8_t reg_md_0_vrf18_req_mask_b;
	uint8_t reg_md_0_ddren_req_mask_b;
	uint8_t reg_md_1_srcclkena_mask_b;
	uint8_t reg_md_1_infra_req_mask_b;
	uint8_t reg_md_1_apsrc_req_mask_b;
	uint8_t reg_md_1_vrf18_req_mask_b;
	uint8_t reg_md_1_ddren_req_mask_b;
	uint8_t reg_conn_srcclkena_mask_b;
	uint8_t reg_conn_srcclkenb_mask_b;
	uint8_t reg_conn_infra_req_mask_b;
	uint8_t reg_conn_apsrc_req_mask_b;
	uint8_t reg_conn_vrf18_req_mask_b;
	uint8_t reg_conn_ddren_req_mask_b;
	uint8_t reg_conn_vfe28_mask_b;
	uint8_t reg_srcclkeni_srcclkena_mask_b;
	uint8_t reg_srcclkeni_infra_req_mask_b;
	uint8_t reg_infrasys_apsrc_req_mask_b;
	uint8_t reg_infrasys_ddren_req_mask_b;
	uint8_t reg_sspm_srcclkena_mask_b;
	uint8_t reg_sspm_infra_req_mask_b;
	uint8_t reg_sspm_apsrc_req_mask_b;
	uint8_t reg_sspm_vrf18_req_mask_b;
	uint8_t reg_sspm_ddren_req_mask_b;

	/* SPM_SRC2_MASK */
	uint8_t reg_scp_srcclkena_mask_b;
	uint8_t reg_scp_infra_req_mask_b;
	uint8_t reg_scp_apsrc_req_mask_b;
	uint8_t reg_scp_vrf18_req_mask_b;
	uint8_t reg_scp_ddren_req_mask_b;
	uint8_t reg_audio_dsp_srcclkena_mask_b;
	uint8_t reg_audio_dsp_infra_req_mask_b;
	uint8_t reg_audio_dsp_apsrc_req_mask_b;
	uint8_t reg_audio_dsp_vrf18_req_mask_b;
	uint8_t reg_audio_dsp_ddren_req_mask_b;
	uint8_t reg_ufs_srcclkena_mask_b;
	uint8_t reg_ufs_infra_req_mask_b;
	uint8_t reg_ufs_apsrc_req_mask_b;
	uint8_t reg_ufs_vrf18_req_mask_b;
	uint8_t reg_ufs_ddren_req_mask_b;
	uint8_t reg_disp0_apsrc_req_mask_b;
	uint8_t reg_disp0_ddren_req_mask_b;
	uint8_t reg_disp1_apsrc_req_mask_b;
	uint8_t reg_disp1_ddren_req_mask_b;
	uint8_t reg_gce_infra_req_mask_b;
	uint8_t reg_gce_apsrc_req_mask_b;
	uint8_t reg_gce_vrf18_req_mask_b;
	uint8_t reg_gce_ddren_req_mask_b;
	uint8_t reg_apu_srcclkena_mask_b;
	uint8_t reg_apu_infra_req_mask_b;
	uint8_t reg_apu_apsrc_req_mask_b;
	uint8_t reg_apu_vrf18_req_mask_b;
	uint8_t reg_apu_ddren_req_mask_b;
	uint8_t reg_cg_check_srcclkena_mask_b;
	uint8_t reg_cg_check_apsrc_req_mask_b;
	uint8_t reg_cg_check_vrf18_req_mask_b;
	uint8_t reg_cg_check_ddren_req_mask_b;

	/* SPM_SRC3_MASK */
	uint8_t reg_dvfsrc_event_trigger_mask_b;
	uint8_t reg_sw2spm_wakeup_mask_b;
	uint8_t reg_adsp2spm_wakeup_mask_b;
	uint8_t reg_sspm2spm_wakeup_mask_b;
	uint8_t reg_scp2spm_wakeup_mask_b;
	uint8_t reg_csyspwrup_ack_mask;
	uint8_t reg_spm_reserved_srcclkena_mask_b;
	uint8_t reg_spm_reserved_infra_req_mask_b;
	uint8_t reg_spm_reserved_apsrc_req_mask_b;
	uint8_t reg_spm_reserved_vrf18_req_mask_b;
	uint8_t reg_spm_reserved_ddren_req_mask_b;
	uint8_t reg_mcupm_srcclkena_mask_b;
	uint8_t reg_mcupm_infra_req_mask_b;
	uint8_t reg_mcupm_apsrc_req_mask_b;
	uint8_t reg_mcupm_vrf18_req_mask_b;
	uint8_t reg_mcupm_ddren_req_mask_b;
	uint8_t reg_msdc0_srcclkena_mask_b;
	uint8_t reg_msdc0_infra_req_mask_b;
	uint8_t reg_msdc0_apsrc_req_mask_b;
	uint8_t reg_msdc0_vrf18_req_mask_b;
	uint8_t reg_msdc0_ddren_req_mask_b;
	uint8_t reg_msdc1_srcclkena_mask_b;
	uint8_t reg_msdc1_infra_req_mask_b;
	uint8_t reg_msdc1_apsrc_req_mask_b;
	uint8_t reg_msdc1_vrf18_req_mask_b;
	uint8_t reg_msdc1_ddren_req_mask_b;

	/* SPM_SRC4_MASK */
	uint32_t reg_ccif_event_srcclkena_mask_b;
	uint8_t reg_bak_psri_srcclkena_mask_b;
	uint8_t reg_bak_psri_infra_req_mask_b;
	uint8_t reg_bak_psri_apsrc_req_mask_b;
	uint8_t reg_bak_psri_vrf18_req_mask_b;
	uint8_t reg_bak_psri_ddren_req_mask_b;
	uint8_t reg_dramc_md32_infra_req_mask_b;
	uint8_t reg_dramc_md32_vrf18_req_mask_b;
	uint8_t reg_conn_srcclkenb2pwrap_mask_b;
	uint8_t reg_dramc_md32_apsrc_req_mask_b;

	/* SPM_SRC5_MASK */
	uint32_t reg_mcusys_merge_apsrc_req_mask_b;
	uint32_t reg_mcusys_merge_ddren_req_mask_b;
	uint8_t reg_afe_srcclkena_mask_b;
	uint8_t reg_afe_infra_req_mask_b;
	uint8_t reg_afe_apsrc_req_mask_b;
	uint8_t reg_afe_vrf18_req_mask_b;
	uint8_t reg_afe_ddren_req_mask_b;
	uint8_t reg_msdc2_srcclkena_mask_b;
	uint8_t reg_msdc2_infra_req_mask_b;
	uint8_t reg_msdc2_apsrc_req_mask_b;
	uint8_t reg_msdc2_vrf18_req_mask_b;
	uint8_t reg_msdc2_ddren_req_mask_b;

	/* SPM_WAKEUP_EVENT_MASK */
	uint32_t reg_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	uint32_t reg_ext_wakeup_event_mask;

	/* SPM_SRC7_MASK */
	uint8_t reg_pcie_srcclkena_mask_b;
	uint8_t reg_pcie_infra_req_mask_b;
	uint8_t reg_pcie_apsrc_req_mask_b;
	uint8_t reg_pcie_vrf18_req_mask_b;
	uint8_t reg_pcie_ddren_req_mask_b;
	uint8_t reg_dpmaif_srcclkena_mask_b;
	uint8_t reg_dpmaif_infra_req_mask_b;
	uint8_t reg_dpmaif_apsrc_req_mask_b;
	uint8_t reg_dpmaif_vrf18_req_mask_b;
	uint8_t reg_dpmaif_ddren_req_mask_b;

	/* Auto-gen End */
};

check_member(mtk_spm_regs, poweron_config_set, 0x0);
check_member(mtk_spm_regs, dis_pwr_con, 0x354);
check_member(mtk_spm_regs, nna_pwr_con, 0x3E0);
check_member(mtk_spm_regs, ap_mdsrc_req, 0x430);
check_member(mtk_spm_regs, ssusb_top_pwr_con, 0x9F0);
check_member(mtk_spm_regs, ssusb_top_p1_pwr_con, 0x9F4);
check_member(mtk_spm_regs, adsp_infra_pwr_con, 0x9F8);
check_member(mtk_spm_regs, adsp_ao_pwr_con, 0x9FC);
check_member(mtk_spm_regs, md32pcm_cfgreg_sw_rstn, 0xA00);
check_member(mtk_spm_regs, md32pcm_dma0_src, 0xC00);
check_member(mtk_spm_regs, md32pcm_dma0_dst, 0xC04);
check_member(mtk_spm_regs, md32pcm_dma0_wppt, 0xC08);
check_member(mtk_spm_regs, md32pcm_dma0_wpto, 0xC0C);
check_member(mtk_spm_regs, md32pcm_dma0_count, 0xC10);
check_member(mtk_spm_regs, md32pcm_dma0_con, 0xC14);
check_member(mtk_spm_regs, md32pcm_dma0_start, 0xC18);
check_member(mtk_spm_regs, md32pcm_dma0_rlct, 0xC24);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

static const struct power_domain_data disp[] = {
	{
		.pwr_con = &mtk_spm->dis_pwr_con,
		.pwr_sta_mask = 0x1 << 21,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
	},
};

/* without audio mtcmos control in MT8186 */
static const struct power_domain_data audio[] = {
};

static const struct power_domain_data adsp[] = {
	{
		.pwr_con = &mtk_spm->adsp_ao_pwr_con,
		.pwr_sta_mask = 0x1 << 17,
	},
	{
		.pwr_con = &mtk_spm->adsp_infra_pwr_con,
		.pwr_sta_mask = 0x1 << 10,
	},
	{
		.pwr_con = &mtk_spm->adsp_pwr_con,
		.pwr_sta_mask = 0x1 << 31,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
		.caps = SCPD_SRAM_ISO,
	},
};

#endif  /* SOC_MEDIATEK_MT8186_SPM_H */
