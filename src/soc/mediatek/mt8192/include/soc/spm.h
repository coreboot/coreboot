/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_SPM_H
#define SOC_MEDIATEK_MT8192_SPM_H

#include <soc/addressmap.h>
#include <types.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE	0xb16

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
	u32 reserved1[1];
	u32 spm_sw_rst_con;	/* 0x0040 */
	u32 spm_sw_rst_con_set;
	u32 spm_sw_rst_con_clr;
	u32 vs1_psr_mask_b;
	u32 vs2_psr_mask_b;
	u32 reserved2[12];
	u32 md32_clk_con;	/* 0x0084 */
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
	u32 src_req_sta_0;
	u32 src_req_sta_1;
	u32 src_req_sta_2;
	u32 pcm_timer_out;
	u32 pcm_wdt_out;
	u32 spm_irq_sta;
	u32 src_req_sta_4;
	u32 md32pcm_wakeup_sta;
	u32 md32pcm_event_sta;
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_misc;
	u32 mm_dvfs_halt;
	u32 reserved3[2];
	u32 bus_protect_rdy;	/* 0x0150 */
	u32 bus_protect1_rdy;
	u32 bus_protect2_rdy;
	u32 bus_protect3_rdy;
	u32 subsys_idle_sta;
	u32 pcm_sta;
	u32 src_req_sta_3;
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
	u32 reserved4[3];
	u32 dvfsrc_event_sta;	/* 0x01a4 */
	u32 bus_protect4_rdy;
	u32 bus_protect5_rdy;
	u32 bus_protect6_rdy;
	u32 bus_protect7_rdy;
	u32 bus_protect8_rdy;
	u32 reserved5[5];
	u32 spm_twam_last_sta0;	/* 0x01d0 */
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
	u32 reserved6[1];
	u32 armpll_clk_con;	/* 0x022c */
	u32 mcusys_idle_sta;
	u32 gic_wakeup_sta;
	u32 cpu_spare_con;
	u32 cpu_spare_con_set;
	u32 cpu_spare_con_clr;
	u32 armpll_clk_sel;
	u32 ext_int_wakeup_req;
	u32 ext_int_wakeup_req_set;
	u32 ext_int_wakeup_req_clr;
	u32 reserved7[3];
	u32 mp0_cpu0_irq_mask;	/* 0x0260 */
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
	u32 reserved8[10];
	u32 spm2sw_mailbox_0;	/* 0x02d0 */
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
	u32 dpy_shu2_sram_con;
	u32 dramc_mcu2_sram_con;
	u32 dramc_mcu_sram_con;
	u32 mcupm_pwr_con;
	u32 dpy2_pwr_con;
	u32 peri_pwr_con;
};

check_member(mtk_spm_regs, md32_clk_con, 0x0084);
check_member(mtk_spm_regs, bus_protect_rdy, 0x0150);
check_member(mtk_spm_regs, dvfsrc_event_sta, 0x01a4);
check_member(mtk_spm_regs, spm_twam_last_sta0, 0x01d0);
check_member(mtk_spm_regs, mp0_cpu0_irq_mask, 0x0260);
check_member(mtk_spm_regs, spm2sw_mailbox_0, 0x02d0);
check_member(mtk_spm_regs, peri_pwr_con, 0x03c8);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

#endif  /* SOC_MEDIATEK_MT8192_SPM_H */
