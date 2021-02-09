/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8195_SPM_H
#define SOC_MEDIATEK_MT8195_SPM_H

#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <types.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE	0xb16
#define SPM_REGWR_CFG_KEY	(SPM_PROJECT_CODE << 16)

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
	u32 rsv_6038[2];
	u32 spm_sw_rst_con;
	u32 spm_sw_rst_con_set;
	u32 spm_sw_rst_con_clr;
	u32 rsv_604c;
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
	u32 rsv_607c;
	u32 spm_src_req;
	u32 spm_src_mask;
	u32 spm_src2_mask;
	u32 rsv_608c;
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
	u32 rsv_60d0[12];
	u32 pcm_reg0_data;
	u32 pcm_reg2_data;
	u32 pcm_reg6_data;
	u32 pcm_reg7_data;
	u32 pcm_reg13_data;
	u32 src_req_sta_0;
	u32 src_req_sta_1;
	u32 rsv_611c;
	u32 src_req_sta_2;
	u32 src_req_sta_3;
	u32 src_req_sta_4;
	u32 rsv_612c;
	u32 pcm_timer_out;
	u32 pcm_wdt_out;
	u32 spm_irq_sta;
	u32 md32pcm_wakeup_sta;
	u32 md32pcm_event_sta;
	u32 spm_wakeup_sta;
	u32 spm_wakeup_ext_sta;
	u32 spm_wakeup_misc;
	u32 mm_dvfs_halt;
	u32 rsv_6154[4];
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
	u32 rsv_61c8[2];
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
	u32 rsv_6254[3];
	u32 cpu_irq_mask;
	u32 cpu_irq_mask_set;
	u32 cpu_irq_mask_clr;
	u32 rsv_626c[5];
	u32 cpu_wfi_en;
	u32 cpu_wfi_en_set;
	u32 cpu_wfi_en_clr;
	u32 rsv_628c;
	u32 sysram_con;
	u32 sysrom_con;
	u32 rsv_6298[2];
	u32 root_cputop_addr;
	u32 root_core_addr;
	u32 rsv_62a8[10];
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
	u32 mfg6_pwr_con;
	u32 ifr_pwr_con;
	u32 ifr_sub_pwr_con;
	u32 peri_pwr_con;
	u32 pextp_mac_top_p0_pwr_con;
	u32 pextp_mac_top_p1_pwr_con;
	u32 pcie_phy_pwr_con;
	u32 ssusb_pcie_phy_pwr_con;
	u32 ssusb_top_p1_pwr_con;
	u32 ssusb_top_p2_pwr_con;
	u32 ssusb_top_p3_pwr_con;
	u32 ether_pwr_con;
	u32 dpy0_pwr_con;
	u32 dpy1_pwr_con;
	u32 dpm0_pwr_con;
	u32 dpm1_pwr_con;
	u32 audio_pwr_con;
	u32 audio_asrc_pwr_con;
	u32 adsp_pwr_con;
	u32 vppsys0_pwr_con;
	u32 vppsys1_pwr_con;
	u32 vdosys0_pwr_con;
	u32 vdosys1_pwr_con;
	u32 wpesys_pwr_con;
	u32 dp_tx_pwr_con;
	u32 edp_tx_pwr_con;
	u32 hdmi_tx_pwr_con;
	u32 hdmi_rx_pwr_con;
	u32 vde0_pwr_con;
	u32 vde1_pwr_con;
	u32 vde2_pwr_con;
	u32 ven_pwr_con;
	u32 ven_core1_pwr_con;
	u32 cam_pwr_con;
	u32 cam_rawa_pwr_con;
	u32 cam_rawb_pwr_con;
	u32 cam_rawc_pwr_con;
	u32 img_m_pwr_con;
	u32 img_d_pwr_con;
	u32 ipe_pwr_con;
	u32 nna0_pwr_con;
	u32 nna1_pwr_con;
	u32 ipnna_pwr_con;
	u32 csi_rx_top_pwr_con;
	u32 rsv_63c8;
	u32 sspm_sram_con;
	u32 scp_sram_con;
	u32 ufs_sram_con;
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
	u32 rsv_6430;
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
	u32 rsv_6500[64];
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
	u32 rsv_6648[2];
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
	u32 rsv_6688;
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
	u32 ufs_psri_sw;
	u32 ufs_psri_sw_set;
	u32 ufs_psri_sw_clr;
	u32 spm_ap_sema;
	u32 spm_spm_sema;
	u32 spm_dvfs_con;
	u32 spm_dvfs_con_sta;
	u32 spm_pmic_spmi_con;
	u32 rsv_670c;
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
	u32 rsv_6858[18];
	u32 dramc_gating_err_latch_ch0_0;
	u32 dramc_gating_err_latch_ch0_1;
	u32 dramc_gating_err_latch_ch0_2;
	u32 dramc_gating_err_latch_ch0_3;
	u32 dramc_gating_err_latch_ch0_4;
	u32 dramc_gating_err_latch_ch0_5;
	u32 rsv_68b8[15];
	u32 dramc_gating_err_latch_spare_0;
	u32 rsv_68f8[2];
	u32 spm_ack_chk_con_0;
	u32 spm_ack_chk_pc_0;
	u32 spm_ack_chk_sel_0;
	u32 spm_ack_chk_timer_0;
	u32 spm_ack_chk_sta_0;
	u32 spm_ack_chk_swint_0;
	u32 rsv_6918[2];
	u32 spm_ack_chk_con_1;
	u32 spm_ack_chk_pc_1;
	u32 spm_ack_chk_sel_1;
	u32 spm_ack_chk_timer_1;
	u32 spm_ack_chk_sta_1;
	u32 spm_ack_chk_swint_1;
	u32 rsv_6938[2];
	u32 spm_ack_chk_con_2;
	u32 spm_ack_chk_pc_2;
	u32 spm_ack_chk_sel_2;
	u32 spm_ack_chk_timer_2;
	u32 spm_ack_chk_sta_2;
	u32 spm_ack_chk_swint_2;
	u32 rsv_6958[2];
	u32 spm_ack_chk_con_3;
	u32 spm_ack_chk_pc_3;
	u32 spm_ack_chk_sel_3;
	u32 spm_ack_chk_timer_3;
	u32 spm_ack_chk_sta_3;
	u32 spm_ack_chk_swint_3;
	u32 spm_counter_0;
	u32 spm_counter_1;
	u32 spm_counter_2;
	u32 rsv_6984[2];
	u32 sys_timer_con;
	u32 spm_twam_con;
	u32 spm_twam_window_len;
	u32 spm_twam_idle_sel;
	u32 spm_twam_event_clear;
	u32 rsv_69a0[344];
	u32 pmsr_last_dat;
	u32 pmsr_last_cnt;
	u32 pmsr_last_ack;
	u32 rsv_6f0c;
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
	u32 rsv_6f38;
	u32 spm_pmsr_sel_con10;
	u32 spm_pmsr_sel_con11;
	u32 rsv_6f44[29];
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
check_member(mtk_spm_regs, audio_pwr_con, 0x0358);
check_member(mtk_spm_regs, ap_mdsrc_req, 0x043c);
check_member(mtk_spm_regs, ulposc_con, 0x644);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

static const struct power_domain_data disp[] = {
	{
		.pwr_con = &mtk_spm->vppsys0_pwr_con,
		.pwr_sta_mask = 0x1 << 11,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
	},
	{
		.pwr_con = &mtk_spm->vdosys0_pwr_con,
		.pwr_sta_mask = 0x1 << 13,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
	},
	{
		.pwr_con = &mtk_spm->vppsys1_pwr_con,
		.pwr_sta_mask = 0x1 << 12,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
	},
	{
		.pwr_con = &mtk_spm->vdosys1_pwr_con,
		.pwr_sta_mask = 0x1 << 14,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
	},
};

static const struct power_domain_data audio[] = {
	{
		.pwr_con = &mtk_spm->adsp_pwr_con,
		.pwr_sta_mask = 0x1 << 10,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
		.caps = SCPD_SRAM_ISO,
	},
	{
		.pwr_con = &mtk_spm->audio_pwr_con,
		.pwr_sta_mask = 0x1 << 8,
		.sram_pdn_mask = 0x1 << 8,
		.sram_ack_mask = 0x1 << 12,
	},
};

#endif  /* SOC_MEDIATEK_MT8195_SPM_H */
