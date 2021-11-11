/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_MCUCFG_H
#define SOC_MEDIATEK_MT8192_MCUCFG_H

#include <soc/addressmap.h>
#include <types.h>

struct mt8192_mcucfg_regs {
	u32 reserved1[2];
	u32 mbista_mcsi_sf1_con;	/* 0x0008 */
	u32 mbista_mcsi_sf1_result;
	u32 mbista_mcsi_sf2_con;
	u32 mbista_mcsi_sf2_result;
	u32 mbista_etb_con;
	u32 mbista_etb_result;
	u32 mbista_rstb;
	u32 mbista_all_result;
	u32 reserved2[2];
	u32 mbist_trigger_mux_ctl;	/* 0x0030 */
	u32 reserved3[3];
	u32 dfd_ctrl;	/* 0x0040 */
	u32 dfd_cnt_l;
	u32 dfd_cnt_h;
	u32 reserved4[5];
	u32 mp_top_dbg_mon_sel;	/* 0x0060 */
	u32 mp_top_dbg_mon;
	u32 mp0_dbg_mon_sel;
	u32 mp0_dbg_mon;
	u32 reserved5[28];
	u32 mp0_ptp_sensor_sel;	/* 0x00e0 */
	u32 reserved6[5];
	u32 armpll_jit_clk_out_sel;	/* 0x00f8 */
	u32 reserved7[1];
	u32 cci_tra_cfg0;	/* 0x0100 */
	u32 reserved8[4];
	u32 cci_tra_cfg5;	/* 0x0114 */
	u32 cci_tra_cfg6;
	u32 cci_tra_cfg7;
	u32 cci_tra_cfg8;
	u32 cci_tra_cfg9;
	u32 cci_tra_cfg10;
	u32 cci_tra_cfg11;
	u32 cci_tra_cfg12;
	u32 reserved9[51];
	u32 cci_m0_tra;	/* 0x0200 */
	u32 cci_m1_tra;
	u32 cci_m2_tra;
	u32 reserved10[5];
	u32 cci_s1_tra;	/* 0x0220 */
	u32 cci_s2_tra;
	u32 cci_s3_tra;
	u32 cci_s4_tra;
	u32 reserved11[4];
	u32 cci_m0_tra_latch;	/* 0x0240 */
	u32 cci_m1_tra_latch;
	u32 cci_m2_tra_latch;
	u32 reserved12[5];
	u32 cci_s1_tra_latch;	/* 0x0260 */
	u32 cci_s2_tra_latch;
	u32 cci_s3_tra_latch;
	u32 cci_s4_tra_latch;
	u32 reserved13[20];
	u32 cci_m0_if;	/* 0x02c0 */
	u32 cci_m1_if;
	u32 cci_m2_if;
	u32 reserved14[5];
	u32 cci_s1_if;	/* 0x02e0 */
	u32 cci_s2_if;
	u32 cci_s3_if;
	u32 cci_s4_if;
	u32 reserved15[12];
	u32 cci_m0_if_latch;	/* 0x0320 */
	u32 cci_m1_if_latch;
	u32 cci_m2_if_latch;
	u32 reserved16[5];
	u32 cci_s1_if_latch;	/* 0x0340 */
	u32 cci_s2_if_latch;
	u32 cci_s3_if_latch;
	u32 cci_s4_if_latch;
	u32 reserved17[44];
	u32 l3c_share_status0;	/* 0x0400 */
	u32 l3c_share_status1;
	u32 l3c_share_status2;
	u32 reserved18[1];
	u32 mp0_cpu0_dc_age;	/* 0x0410 */
	u32 mp0_cpu1_dc_age;
	u32 mp0_cpu2_dc_age;
	u32 mp0_cpu3_dc_age;
	u32 mp0_cpu4_dc_age;
	u32 mp0_cpu5_dc_age;
	u32 mp0_cpu6_dc_age;
	u32 mp0_cpu7_dc_age;
	u32 reserved19[52];
	u32 mp0_cpu0_nonwfx_ctrl;	/* 0x0500 */
	u32 mp0_cpu0_nonwfx_cnt;
	u32 mp0_cpu1_nonwfx_ctrl;
	u32 mp0_cpu1_nonwfx_cnt;
	u32 mp0_cpu2_nonwfx_ctrl;
	u32 mp0_cpu2_nonwfx_cnt;
	u32 mp0_cpu3_nonwfx_ctrl;
	u32 mp0_cpu3_nonwfx_cnt;
	u32 mp0_cpu4_nonwfx_ctrl;
	u32 mp0_cpu4_nonwfx_cnt;
	u32 mp0_cpu5_nonwfx_ctrl;
	u32 mp0_cpu5_nonwfx_cnt;
	u32 mp0_cpu6_nonwfx_ctrl;
	u32 mp0_cpu6_nonwfx_cnt;
	u32 mp0_cpu7_nonwfx_ctrl;
	u32 mp0_cpu7_nonwfx_cnt;
	u32 reserved20[48];
	u32 mp0_ses_apb_trig;	/* 0x0600 */
	u32 mp0_ses_clk_en;
	u32 reserved21[2];
	u32 wfx_ret_met_dbc_sel;	/* 0x0610 */
	u32 reserved22[3];
	u32 adb_bist_cfg1;	/* 0x0620 */
	u32 adb_bist_cfg2_md;
	u32 adb_bist_cfg3_go;
	u32 adb_bist_done;
	u32 adb_bist_pass;
	u32 adb_bist_done1;
	u32 reserved23[1];
	u32 adb_bist_pass1;	/* 0x063c */
	u32 reserved24[48];
	u32 axi2acp_cfg_ctrl;	/* 0x0700 */
	u32 axi2acp_cfg_mask_id;
	u32 reserved25[2];
	u32 axi2acp_ar_def_set;	/* 0x0710 */
	u32 reserved26[1];
	u32 axi2acp_aw_def_set;	/* 0x0718 */
	u32 reserved27[1];
	u32 axi2acp_err_addr0;	/* 0x0720 */
	u32 axi2acp_err_addr1;
	u32 axi2acp_err_st;
	u32 axi2acp_err_id;
	u32 axi2acp_err_irq;
	u32 axi2acp_err_dbgout;
	u32 reserved28[50];
	u32 mst_ccim0_inject_fault;	/* 0x0800 */
	u32 mst_ccim0_rec_par;
	u32 mst_ccim0_log_rid;
	u32 reserved29[1];
	u32 mst_ccim0_log_rd0;	/* 0x0810 */
	u32 mst_ccim0_log_rd1;
	u32 mst_ccim0_log_rd2;
	u32 mst_ccim0_log_rd3;
	u32 mst_ccim1_inject_fault;
	u32 mst_ccim1_rec_par;
	u32 mst_ccim1_log_rid;
	u32 reserved30[1];
	u32 mst_ccim1_log_rd0;	/* 0x0830 */
	u32 mst_ccim1_log_rd1;
	u32 mst_ccim1_log_rd2;
	u32 mst_ccim1_log_rd3;
	u32 mst_intaxi_inject_fault;
	u32 mst_intaxi_rec_par;
	u32 mst_intaxi_log_rid;
	u32 reserved31[1];
	u32 mst_intaxi_log_rd0;	/* 0x0850 */
	u32 mst_intaxi_log_rd1;
	u32 reserved32[2];
	u32 slv_1to2_inject_fault;	/* 0x0860 */
	u32 slv_1to2_rec_par;
	u32 slv_1to2_log_awid;
	u32 slv_1to2_log_arid;
	u32 slv_1to2_log_awa0;
	u32 slv_1to2_log_awa1;
	u32 slv_1to2_log_ara0;
	u32 slv_1to2_log_ara1;
	u32 slv_1to2_log_wid;
	u32 reserved33[3];
	u32 slv_1to2_log_wd0;	/* 0x0890 */
	u32 slv_1to2_log_wd1;
	u32 slv_1to2_log_wd2;
	u32 slv_1to2_log_wd3;
	u32 slv_l3c_inject_fault;
	u32 slv_l3c_rec_par;
	u32 slv_l3c_log_awid;
	u32 slv_l3c_log_arid;
	u32 slv_l3c_log_awa0;
	u32 slv_l3c_log_awa1;
	u32 slv_l3c_log_ara0;
	u32 slv_l3c_log_ara1;
	u32 slv_l3c_log_wid;
	u32 reserved34[1];
	u32 slv_l3c_log_wd0;	/* 0x08c8 */
	u32 slv_l3c_log_wd1;
	u32 slv_gic_inject_fault;
	u32 slv_gic_rec_par;
	u32 slv_gic_log_awid;
	u32 slv_gic_log_arid;
	u32 slv_gic_log_awa0;
	u32 slv_gic_log_awa1;
	u32 slv_gic_log_ara0;
	u32 slv_gic_log_ara1;
	u32 slv_gic_log_wid;
	u32 reserved35[1];
	u32 slv_gic_log_wd0;	/* 0x08f8 */
	u32 slv_gic_log_wd1;
	u32 reserved36[64];
	u32 ildo_vin_big0;	/* 0x0a00 */
	u32 ildo_out_big0;
	u32 ildo_ovstck_big0;
	u32 ildo_vo2hck_big0;
	u32 ildo_mhstck_big0;
	u32 ildo_mlstck_big0;
	u32 ilod_uvwin_big0;
	u32 ilod_ovwin_big0;
	u32 ildo_pd_cfg_big0;
	u32 reserved37[3];
	u32 ildo_vin_big1;	/* 0x0a30 */
	u32 ildo_out_big1;
	u32 ildo_ovstck_big1;
	u32 ildo_vo2hck_big1;
	u32 ildo_mhstck_big1;
	u32 ildo_mlstck_big1;
	u32 ilod_uvwin_big1;
	u32 ilod_ovwin_big1;
	u32 ildo_pd_cfg_big1;
	u32 reserved38[3];
	u32 ildo_vin_big2;	/* 0x0a60 */
	u32 ildo_out_big2;
	u32 ildo_ovstck_big2;
	u32 ildo_vo2hck_big2;
	u32 ildo_mhstck_big2;
	u32 ildo_mlstck_big2;
	u32 ilod_uvwin_big2;
	u32 ilod_ovwin_big2;
	u32 ildo_pd_cfg_big2;
	u32 reserved39[3];
	u32 ildo_vin_big3;	/* 0x0a90 */
	u32 ildo_out_big3;
	u32 ildo_ovstck_big3;
	u32 ildo_vo2hck_big3;
	u32 ildo_mhstck_big3;
	u32 ildo_mlstck_big3;
	u32 ilod_uvwin_big3;
	u32 ilod_ovwin_big3;
	u32 ildo_pd_cfg_big3;
	u32 reserved40[1495];
	u32 l3c_share_cfg0;	/* 0x2210 */
	u32 l3c_share_cfg1;
	u32 l3c_share_cfg2;
	u32 reserved41[1];
	u32 udi_cfg0;	/* 0x2220 */
	u32 udi_cfg1;
	u32 reserved42[2];
	u32 mcusys_core_status;	/* 0x2230 */
	u32 reserved43[1];
	u32 cfg_sys_valid;	/* 0x2238 */
	u32 cfg_sysbase_addr_0;
	u32 cfg_sysbase_addr_1;
	u32 cfg_sysbase_addr_2;
	u32 cfg_sysbase_addr_3;
	u32 cfg_sysbase_addr_4;
	u32 reserved44[4];
	u32 mcusys_base;	/* 0x2260 */
	u32 l3c_sram_base;
	u32 gic_periph_base;
	u32 cci_periph_base;
	u32 cci_periph_infra_base;
	u32 dfd_sram_base;
	u32 l3c_mm_sram_base;
	u32 ext_l3c_sram_base;
	u32 ext_gic_periph_base;
	u32 mcusys_qos_shaper_degree;
	u32 mcusys_qos_shaper_others;
	u32 mcusys_qos_age_set_enable;
	u32 sspm_cpueb_turbo_pll_ctl;
	u32 cpu_eb_apb_base;
	u32 cpu_eb_tcm_btb_delsel;
	u32 cpu_eb_mem_misc;
	u32 cpu_plldiv_cfg0;
	u32 cpu_plldiv_cfg1;
	u32 cpu_plldiv_cfg2;
	u32 cpu_plldiv_cfg3;
	u32 cpu_plldiv_cfg4;
	u32 reserved45[11];
	u32 bus_plldiv_cfg;	/* 0x22e0 */
	u32 reserved46[3];
	u32 plldiv_ctl0;	/* 0x22f0 */
	u32 reserved47[3];
	u32 mcsi_ram_delsel0;	/* 0x2300 */
	u32 mcsi_ram_delsel1;
	u32 reserved48[30];
	u32 etb_cfg0;	/* 0x2380 */
	u32 reserved49[3];
	u32 bus_parity_gen_en;	/* 0x2390 */
	u32 bus_parity_clr;
	u32 bus_parity_chk_en;
	u32 bus_parity_fail;
	u32 dsu2biu_addr_remap_cfg;
	u32 dsu2biu_addr_remap_sta;
	u32 reserved50[2];
	u32 dsu2biu_addr_remap_0_cfg0;	/* 0x23b0 */
	u32 dsu2biu_addr_remap_0_cfg1;
	u32 dsu2biu_addr_remap_0_cfg2;
	u32 reserved51[1];
	u32 dsu2biu_addr_remap_1_cfg0;	/* 0x23c0 */
	u32 dsu2biu_addr_remap_1_cfg1;
	u32 dsu2biu_addr_remap_1_cfg2;
	u32 reserved52[1];
	u32 dsu2biu_addr_remap_magic;	/* 0x23d0 */
	u32 reserved53[11];
	u32 cci_rgu;	/* 0x2400 */
	u32 reserved54[3];
	u32 mcsi_cfg0;	/* 0x2410 */
	u32 mcsi_cfg1;
	u32 mcsi_cfg2;
	u32 mcsi_cfg3;
	u32 mcsi_cfg4;
	u32 reserved55[7];
	u32 mcsic_dcm0;	/* 0x2440 */
	u32 mcsic_dcm1;
	u32 reserved56[18];
	u32 apmcu2emi_early_cke_ctl01;	/* 0x2490 */
	u32 reserved57[1];
	u32 apmcu2emi_early_ebg_ctl0;	/* 0x2498 */
	u32 apmcu2emi_early_ebg_ctl1;
	u32 apmcu2emi_early_ebg_ctl2;
	u32 apmcu2emi_early_ebg_ctl3;
	u32 apmcu2emi_early_ebg_ctl4;
	u32 apmcu2emi_early_ebg_ctl5;
	u32 reserved58[20];
	u32 mp_adb_dcm_cfg0;	/* 0x2500 */
	u32 reserved59[1];
	u32 mp_adb_dcm_cfg2;	/* 0x2508 */
	u32 reserved60[1];
	u32 mp_adb_dcm_cfg4;	/* 0x2510 */
	u32 reserved61[1];
	u32 mp_misc_dcm_cfg0;	/* 0x2518 */
	u32 reserved62[9];
	u32 etb_ck_ctl;	/* 0x2540 */
	u32 reserved63[15];
	u32 dcc_cpu_con0;	/* 0x2580 */
	u32 dcc_cpu_con1;
	u32 reserved64[6];
	u32 dcc_bus_con0;	/* 0x25a0 */
	u32 reserved65[7];
	u32 mcusys_dcm_cfg0;	/* 0x25c0 */
	u32 reserved66[79];
	u32 fcm_spmc_sw_cfg1;	/* 0x2700 */
	u32 fcm_spmc_sw_cfg2;
	u32 fcm_spmc_wait_cfg;
	u32 fcm_spmc_sw_pchannel;
	u32 fcm_spmc_pwr_status;
	u32 fcm_spmc_off_thres;
	u32 fcm_spmc_wdt_latch_info;
	u32 reserved67[9];
	u32 mcusys_spmc_sw_cfg;	/* 0x2740 */
	u32 mcusys_spmc_wait_cfg;
	u32 mcusys_spmc_pwr_status;
	u32 reserved68[45];
	u32 cpc_pllbuck_req_ctrl;	/* 0x2800 */
	u32 mcusys_pwr_ctrl;
	u32 cpusys_pwr_ctrl;
	u32 sw_gic_wakeup_req;
	u32 cpc_pllbuck_arb_weight;
	u32 cpc_flow_ctrl_cfg;
	u32 cpc_last_core_req;
	u32 cpc_cpusys_last_core_resp;
	u32 reserved69[1];
	u32 cpc_mcusys_last_core_resp;	/* 0x2824 */
	u32 cpc_pwr_on_mask;
	u32 reserved70[5];
	u32 cpc_spmc_pwr_status;	/* 0x2840 */
	u32 cpc_core_cur_fsm;
	u32 cpc_cpusys_mcusys_cur_fsm;
	u32 cpc_wakeup_req;
	u32 reserved71[1];
	u32 cpc_gp0_gp1_status;	/* 0x2854 */
	u32 reserved72[1];
	u32 cpc_turbo_ctrl;	/* 0x285c */
	u32 cpc_turbo_gp0_ctrl;
	u32 cpc_turbo_gp1_ctrl;
	u32 cpc_turbo_gp2_ctrl;
	u32 cpc_turbo_pwr_on_mask;
	u32 cpc_turbo_gp0_req;
	u32 cpc_turbo_gp1_req;
	u32 cpc_turbo_gp2_req;
	u32 reserved73[1];
	u32 cpc_turbo_gp0_resp;	/* 0x2880 */
	u32 cpc_turbo_gp1_resp;
	u32 cpc_turbo_gp2_resp;
	u32 cpc_coh_block_thres;
	u32 cpc_int_status;
	u32 cpc_int_enable;
	u32 pllbuck_group_func;
	u32 cpc_dcm_enable;
	u32 cpc_pllbuck_state;
	u32 cpc_cpu_on_sw_hint;
	u32 cpc_cpu_on_sw_hint_set;
	u32 cpc_cpu_on_sw_hint_clear;
	u32 reserved74[20];
	u32 emi_wfifo;	/* 0x2900 */
	u32 axi1to4_cfg;
	u32 apb_cfg;
	u32 emi_adb_edge_sel;
	u32 reserved75[4];
	u32 sclk_cfg_slow_down_ck;	/* 0x2920 */
	u32 reserved76[3];
	u32 acpwakeup;	/* 0x2930 */
	u32 reserved77[1];
	u32 l3gic_idle_bypass;	/* 0x2938 */
	u32 reserved78[21];
	u32 mcusys_dbg_mon_sel;	/* 0x2990 */
	u32 mcusys_dbg_mon;
	u32 reserved79[58];
	u32 gic_acao_ctl0;	/* 0x2a80 */
	u32 reserved80[1];
	u32 gic_acao_ctl2;	/* 0x2a88 */
	u32 reserved81[29];
	u32 spmc_dbg_setting;	/* 0x2b00 */
	u32 kernel_base_l;
	u32 kernel_base_h;
	u32 systime_base_l;
	u32 systime_base_h;
	u32 trace_data_selection;
	u32 reserved82[2];
	u32 trace_data_entry0_l;	/* 0x2b20 */
	u32 trace_data_entry0_h;
	u32 trace_data_entry1_l;
	u32 trace_data_entry1_h;
	u32 trace_data_entry2_l;
	u32 trace_data_entry2_h;
	u32 trace_data_entry3_l;
	u32 trace_data_entry3_h;
	u32 cpu0_on_off_latency;
	u32 cpu1_on_off_latency;
	u32 cpu2_on_off_latency;
	u32 cpu3_on_off_latency;
	u32 cpu4_on_off_latency;
	u32 cpu5_on_off_latency;
	u32 cpu6_on_off_latency;
	u32 cpu7_on_off_latency;
	u32 cluster_off_latency;
	u32 cluster_on_latency;
	u32 mcusys_on_off_latency;
	u32 reserved83[1];
	u32 cluster_off_dormant_counter;	/* 0x2b70 */
	u32 cluster_off_dormant_counter_clear;
	u32 turbo_gp0_gp1_latency;
	u32 turbo_gp2_latency;
	u32 cpc_wdt_latch_info1;
	u32 cpc_wdt_latch_info2;
	u32 cpc_wdt_latch_info3;
	u32 cpc_wdt_latch_info4;
	u32 cpc_wdt_latch_info5;
	u32 cpc_pmu_ctrl;
	u32 cpc_pmu_cnt_clr;
	u32 cpc_pmu_cnt0;
	u32 reserved84[4];
	u32 cpc_wdt_latch_info6;	/* 0x2bb0 */
	u32 cpc_wdt_latch_info7;
	u32 cpc_ptm_event_en_ctrl;
	u32 cpc_ptm_event_en_ctrl1;
	u32 cpc_ptm_trace_sel;
	u32 cpc_ptm_trace_status_l;
	u32 cpc_ptm_trace_status_h;
	u32 reserved85[29];
	u32 sesv6_bg_ctrl;	/* 0x2c40 */
	u32 reserved86[3];
	u32 dsu_sesv6_ao_reg0;	/* 0x2c50 */
	u32 dsu_sesv6_ao_reg1;
	u32 dsu_sesv6_ao_reg2;
	u32 dsu_sesv6_ao_reg3;
	u32 dsu_sesv6_ao_reg4;
	u32 reserved87[39];
	u32 ildo_dout_sel;	/* 0x2d00 */
	u32 reserved88[31];
	u32 dreq20_little_en;	/* 0x2d80 */
	u32 reserved89[2];
	u32 dreq20_big_vproc_iso;	/* 0x2d8c */
	u32 reserved90[28];
	u32 pikachu_event;	/* 0x2e00 */
	u32 pikachu_status;
	u32 reserved91[126];
	u32 cpu0_drcc_ao_config;	/* 0x3000 */
	u32 reserved92[3];
	u32 cpu0_sesv6_ao_reg0;	/* 0x3010 */
	u32 cpu0_sesv6_ao_reg1;
	u32 cpu0_sesv6_ao_reg2;
	u32 cpu0_sesv6_ao_reg3;
	u32 cpu0_sesv6_ao_reg4;
	u32 reserved93[7];
	u32 cpu0_dfs_cfg;	/* 0x3040 */
	u32 reserved94[3];
	u32 cpu0_turbo_cfg;	/* 0x3050 */
	u32 reserved95[105];
	u32 cpu0_resereved_reg;	/* 0x31f8 */
	u32 cpu0_resereved_reg_rd;
	u32 cpu1_drcc_ao_config;
	u32 reserved96[3];
	u32 cpu1_sesv6_ao_reg0;	/* 0x3210 */
	u32 cpu1_sesv6_ao_reg1;
	u32 cpu1_sesv6_ao_reg2;
	u32 cpu1_sesv6_ao_reg3;
	u32 cpu1_sesv6_ao_reg4;
	u32 reserved97[7];
	u32 cpu1_dfs_cfg;	/* 0x3240 */
	u32 reserved98[3];
	u32 cpu1_turbo_cfg;	/* 0x3250 */
	u32 reserved99[105];
	u32 cpu1_resereved_reg;	/* 0x33f8 */
	u32 cpu1_resereved_reg_rd;
	u32 cpu2_drcc_ao_config;
	u32 reserved100[3];
	u32 cpu2_sesv6_ao_reg0;	/* 0x3410 */
	u32 cpu2_sesv6_ao_reg1;
	u32 cpu2_sesv6_ao_reg2;
	u32 cpu2_sesv6_ao_reg3;
	u32 cpu2_sesv6_ao_reg4;
	u32 reserved101[7];
	u32 cpu2_dfs_cfg;	/* 0x3440 */
	u32 reserved102[3];
	u32 cpu2_turbo_cfg;	/* 0x3450 */
	u32 reserved103[105];
	u32 cpu2_resereved_reg;	/* 0x35f8 */
	u32 cpu2_resereved_reg_rd;
	u32 cpu3_drcc_ao_config;
	u32 reserved104[3];
	u32 cpu3_sesv6_ao_reg0;	/* 0x3610 */
	u32 cpu3_sesv6_ao_reg1;
	u32 cpu3_sesv6_ao_reg2;
	u32 cpu3_sesv6_ao_reg3;
	u32 cpu3_sesv6_ao_reg4;
	u32 reserved105[7];
	u32 cpu3_dfs_cfg;	/* 0x3640 */
	u32 reserved106[3];
	u32 cpu3_turbo_cfg;	/* 0x3650 */
	u32 reserved107[105];
	u32 cpu3_resereved_reg;	/* 0x37f8 */
	u32 cpu3_resereved_reg_rd;
	u32 cpu4_drcc_ao_config;
	u32 reserved108[3];
	u32 cpu4_sesv6_ao_reg0;	/* 0x3810 */
	u32 cpu4_sesv6_ao_reg1;
	u32 cpu4_sesv6_ao_reg2;
	u32 cpu4_sesv6_ao_reg3;
	u32 cpu4_sesv6_ao_reg4;
	u32 reserved109[3];
	u32 cpu4_didt_reg;	/* 0x3830 */
	u32 cpu4_mem_lre_reg;
	u32 reserved110[2];
	u32 cpu4_dfs_cfg;	/* 0x3840 */
	u32 reserved111[3];
	u32 cpu4_turbo_cfg;	/* 0x3850 */
	u32 reserved112[105];
	u32 cpu4_resereved_reg;	/* 0x39f8 */
	u32 cpu4_resereved_reg_rd;
	u32 cpu5_drcc_ao_config;
	u32 reserved113[3];
	u32 cpu5_sesv6_ao_reg0;	/* 0x3a10 */
	u32 cpu5_sesv6_ao_reg1;
	u32 cpu5_sesv6_ao_reg2;
	u32 cpu5_sesv6_ao_reg3;
	u32 cpu5_sesv6_ao_reg4;
	u32 reserved114[3];
	u32 cpu5_didt_reg;	/* 0x3a30 */
	u32 cpu5_mem_lre_reg;
	u32 reserved115[2];
	u32 cpu5_dfs_cfg;	/* 0x3a40 */
	u32 reserved116[3];
	u32 cpu5_turbo_cfg;	/* 0x3a50 */
	u32 reserved117[105];
	u32 cpu5_resereved_reg;	/* 0x3bf8 */
	u32 cpu5_resereved_reg_rd;
	u32 cpu6_drcc_ao_config;
	u32 reserved118[3];
	u32 cpu6_sesv6_ao_reg0;	/* 0x3c10 */
	u32 cpu6_sesv6_ao_reg1;
	u32 cpu6_sesv6_ao_reg2;
	u32 cpu6_sesv6_ao_reg3;
	u32 cpu6_sesv6_ao_reg4;
	u32 reserved119[3];
	u32 cpu6_didt_reg;	/* 0x3c30 */
	u32 cpu6_mem_lre_reg;
	u32 reserved120[2];
	u32 cpu6_dfs_cfg;	/* 0x3c40 */
	u32 reserved121[3];
	u32 cpu6_turbo_cfg;	/* 0x3c50 */
	u32 reserved122[105];
	u32 cpu6_resereved_reg;	/* 0x3df8 */
	u32 cpu6_resereved_reg_rd;
	u32 cpu7_drcc_ao_config;
	u32 reserved123[3];
	u32 cpu7_sesv6_ao_reg0;	/* 0x3e10 */
	u32 cpu7_sesv6_ao_reg1;
	u32 cpu7_sesv6_ao_reg2;
	u32 cpu7_sesv6_ao_reg3;
	u32 cpu7_sesv6_ao_reg4;
	u32 reserved124[3];
	u32 cpu7_didt_reg;	/* 0x3e30 */
	u32 cpu7_mem_lre_reg;
	u32 reserved125[2];
	u32 cpu7_dfs_cfg;	/* 0x3e40 */
	u32 reserved126[3];
	u32 cpu7_turbo_cfg;	/* 0x3e50 */
	u32 reserved127[105];
	u32 cpu7_resereved_reg;	/* 0x3ff8 */
	u32 cpu7_resereved_reg_rd;
	u32 reserved128[512];
	u32 mp0_mbist_cfg;	/* 0x4800 */
	u32 reserved129[15];
	u32 mp0_l3_data_ram_delsel;	/* 0x4840 */
	u32 mp0_l3_tag_ram_delsel;
	u32 mp0_l3_victim_ram_delsel;
	u32 mp0_l3_scu_sf_ram_delsel;
	u32 reserved130[12];
	u32 mp0_dcm_cfg0;	/* 0x4880 */
	u32 mp0_dcm_cfg1;
	u32 mp0_dcm_cfg2;
	u32 mp0_dcm_cfg3;
	u32 mp0_dcm_cfg4;
	u32 mp0_dcm_cfg5;
	u32 mp0_dcm_cfg6;
	u32 mp0_dcm_cfg7;
	u32 mp0_dcm_cfg8;
	u32 reserved131[7];
	u32 mp0_l3_cache_parity1;	/* 0x48c0 */
	u32 mp0_l3_cache_parity2;
	u32 mp0_l3_cache_parity3;
	u32 reserved132[1];
	u32 mp0_cluster_cfg0;	/* 0x48d0 */
	u32 reserved133[3];
	u32 mp0_cluster_cfg4;	/* 0x48e0 */
	u32 mp0_cluster_cfg5;
	u32 mp0_cluster_cfg6;
	u32 mp0_cluster_cfg7;
	u32 reserved134[4];
	u32 mp0_cluster_cfg8;	/* 0x4900 */
	u32 mp0_cluster_cfg9;
	u32 mp0_cluster_cfg10;
	u32 mp0_cluster_cfg11;
	u32 mp0_cluster_cfg12;
	u32 mp0_cluster_cfg13;
	u32 mp0_cluster_cfg14;
	u32 mp0_cluster_cfg15;
	u32 mp0_cluster_cfg16;
	u32 mp0_cluster_cfg17;
	u32 mp0_cluster_cfg18;
	u32 mp0_cluster_cfg19;
	u32 mp0_cluster_cfg20;
	u32 mp0_cluster_cfg21;
	u32 mp0_cluster_cfg22;
	u32 mp0_cluster_cfg23;
	u32 mp0_mem_dreq_cfg;
	u32 mp0_victim_rd_mask;
	u32 reserved135[174];
	u32 cpu_type0_spmc0_cfg;	/* 0x4c00 */
	u32 reserved136[7];
	u32 cpu_type0_ram_delsel0_cfg;	/* 0x4c20 */
	u32 cpu_type0_ram_delsel1_cfg;
	u32 cpu_type0_ram_delsel2_cfg;
	u32 reserved137[53];
	u32 cpu_type1_spmc0_cfg;	/* 0x4d00 */
	u32 reserved138[3];
	u32 cpu_type1_mpmmen;	/* 0x4d10 */
	u32 reserved139[3];
	u32 cpu_type1_ram_delsel0_cfg;	/* 0x4d20 */
	u32 cpu_type1_ram_delsel1_cfg;
	u32 cpu_type1_ram_delsel2_cfg;
	u32 reserved140[5];
	u32 cpu_type1_drss_cfg0;	/* 0x4d40 */
	u32 cpu_type1_drss_cfg1;
	u32 reserved141[302];
	u32 mcusys_pwr_con;	/* 0x5200 */
	u32 mp0_cputop_pwr_con;
	u32 mp0_cpu0_pwr_con;
	u32 mp0_cpu1_pwr_con;
	u32 mp0_cpu2_pwr_con;
	u32 mp0_cpu3_pwr_con;
	u32 mp0_cpu4_pwr_con;
	u32 mp0_cpu5_pwr_con;
	u32 mp0_cpu6_pwr_con;
	u32 mp0_cpu7_pwr_con;
	u32 reserved142[246];
	u32 sec_pol_ctl_en0;	/* 0x5600 */
	u32 sec_pol_ctl_en1;
	u32 sec_pol_ctl_en2;
	u32 sec_pol_ctl_en3;
	u32 sec_pol_ctl_en4;
	u32 sec_pol_ctl_en5;
	u32 sec_pol_ctl_en6;
	u32 sec_pol_ctl_en7;
	u32 sec_pol_ctl_en8;
	u32 sec_pol_ctl_en9;
	u32 sec_pol_ctl_en10;
	u32 sec_pol_ctl_en11;
	u32 sec_pol_ctl_en12;
	u32 sec_pol_ctl_en13;
	u32 sec_pol_ctl_en14;
	u32 sec_pol_ctl_en15;
	u32 sec_pol_ctl_en16;
	u32 sec_pol_ctl_en17;
	u32 sec_pol_ctl_en18;
	u32 reserved143[7];
	u32 int_pol_ctl0;	/* 0x5668 */
	u32 int_pol_ctl1;
	u32 int_pol_ctl2;
	u32 int_pol_ctl3;
	u32 int_pol_ctl4;
	u32 int_pol_ctl5;
	u32 int_pol_ctl6;
	u32 int_pol_ctl7;
	u32 int_pol_ctl8;
	u32 int_pol_ctl9;
	u32 int_pol_ctl10;
	u32 int_pol_ctl11;
	u32 int_pol_ctl12;
	u32 int_pol_ctl13;
	u32 int_pol_ctl14;
	u32 int_pol_ctl15;
	u32 int_pol_ctl16;
	u32 int_pol_ctl17;
	u32 int_pol_ctl18;
	u32 reserved144[7];
	u32 int_msk_ctl0;	/* 0x56d0 */
	u32 int_msk_ctl1;
	u32 int_msk_ctl2;
	u32 int_msk_ctl3;
	u32 int_msk_ctl4;
	u32 int_msk_ctl5;
	u32 int_msk_ctl6;
	u32 int_msk_ctl7;
	u32 int_msk_ctl8;
	u32 int_msk_ctl9;
	u32 int_msk_ctl10;
	u32 int_msk_ctl11;
	u32 int_msk_ctl12;
	u32 int_msk_ctl13;
	u32 int_msk_ctl14;
	u32 int_msk_ctl15;
	u32 int_msk_ctl16;
	u32 int_msk_ctl17;
	u32 int_msk_ctl18;
	u32 reserved145[10];
	u32 int_msk_ctl_all;	/* 0x5744 */
	u32 int_cfg_indirect_access;
	u32 int_cfg_direct_access_en;
	u32 reserved146[572];
	u32 dfd_internal_ctl;	/* 0x6040 */
	u32 dfd_internal_counter;
	u32 dfd_internal_pwr_on;
	u32 dfd_internal_chain_legth_0;
	u32 dfd_internal_shift_clk_ratio;
	u32 dfd_internal_counter_return;
	u32 dfd_internal_sram_access;
	u32 dfd_finish_wait_time;
	u32 dfd_internal_chain_group;
	u32 dfd_internal_chain_inv_info_ll;
	u32 dfd_internal_chain_inv_info_lh;
	u32 dfd_internal_chain_inv_info_hl;
	u32 dfd_internal_chain_inv_info_hh;
	u32 dfd_internal_test_so_over_64;
	u32 dfd_internal_mask_out;
	u32 dfd_internal_sw_ns_trigger;
	u32 dfd_internal_mcsi;
	u32 dfd_internal_mcsi_sel_status;
	u32 dfd_v30_ctl;
	u32 dfd_v30_base_addr;
	u32 dfd_power_ctl;
	u32 dfd_reset_on;
	u32 dfd_test_si_0;
	u32 dfd_test_si_1;
	u32 dfd_status_clean;
	u32 dfd_status_return;
	u32 dfd_v35_enable;
	u32 dfd_v35_tap_number;
	u32 dfd_v35_tap_en;
	u32 dfd_v35_ctl;
	u32 dfd_v35_tap_seq0;
	u32 dfd_v35_tap_seq1;
	u32 dfd_v35_seq0_0;
	u32 dfd_v35_seq0_1;
	u32 dfd_v35_seq1_0;
	u32 dfd_v35_seq1_1;
	u32 dfd_v35_seq2_0;
	u32 dfd_v35_seq2_1;
	u32 reserved147[6];
	u32 dfd_soc_clock_stop_mask;	/* 0x60f0 */
	u32 reserved148[2];
	u32 dfd_hw_trigger_mask;	/* 0x60fc */
	u32 dfd_v50_enable;
	u32 dfd_v50_self_trigger_iteration;
	u32 dfd_v50_start_trigger;
	u32 dfd_v50_cpuck_halt;
	u32 dfd_v50_26m_resume;
	u32 dfd_v50_26m_halt_release;
	u32 dfd_v50_system_halt_time;
	u32 dfd_v50_group_0_1_diff;
	u32 dfd_v50_group_0_2_diff;
	u32 dfd_v50_group_0_3_diff;
	u32 dfd_v50_group_0_4_diff;
	u32 dfd_v50_group_0_5_diff;
	u32 dfd_v50_group_0_6_diff;
	u32 dfd_v50_group_0_7_diff;
	u32 dfd_v50_group_0_8_diff;
	u32 dfd_v50_group_0_9_diff;
	u32 dfd_v50_group_0_10_diff;
	u32 dfd_v50_group_0_11_diff;
	u32 dfd_v50_group_0_12_diff;
	u32 dfd_v50_group_0_13_diff;
	u32 dfd_v50_group_0_14_diff;
	u32 dfd_v50_group_0_15_diff;
	u32 dfd_v50_group_0_16_diff;
	u32 dfd_v50_group_0_17_diff;
	u32 dfd_v50_group_0_18_diff;
	u32 dfd_v50_group_0_19_diff;
	u32 dfd_v50_group_0_20_diff;
	u32 dfd_v50_group_0_21_diff;
	u32 dfd_v50_group_0_22_diff;
	u32 dfd_v50_group_0_23_diff;
	u32 dfd_v50_group_0_24_diff;
	u32 dfd_v50_group_0_25_diff;
	u32 dfd_v50_group_0_26_diff;
	u32 dfd_v50_group_0_27_diff;
	u32 dfd_v50_group_0_28_diff;
	u32 dfd_v50_group_0_29_diff;
	u32 dfd_v50_group_0_30_diff;
	u32 dfd_v50_group_0_31_diff;
	u32 dfd_v50_chain_group_3_0_info;
	u32 dfd_v50_chain_group_7_4_info;
	u32 dfd_v50_chain_group_11_8_info;
	u32 dfd_v50_chain_group_15_12_info;
	u32 dfd_v50_chain_group_19_16_info;
	u32 dfd_v50_chain_group_23_20_info;
	u32 dfd_v50_chain_group_27_24_info;
	u32 dfd_v50_chain_group_31_28_info;
	u32 dfd_v50_chain_group_35_32_info;
	u32 dfd_v50_chain_group_39_36_info;
	u32 dfd_v50_chain_group_43_40_info;
	u32 dfd_v50_chain_group_47_44_info;
	u32 dfd_v50_chain_group_51_48_info;
	u32 dfd_v50_chain_group_55_52_info;
	u32 dfd_v50_chain_group_59_56_info;
	u32 dfd_v50_chain_group_63_60_info;
	u32 dfd_v50_chain_group_67_64_info;
	u32 dfd_v50_chain_group_71_68_info;
	u32 dfd_v50_chain_group_75_72_info;
	u32 dfd_v50_chain_group_79_76_info;
	u32 dfd_v50_chain_group_83_80_info;
	u32 dfd_v50_chain_group_87_84_info;
	u32 dfd_v50_chain_group_91_88_info;
	u32 dfd_v50_chain_group_95_92_info;
	u32 dfd_v50_chain_group_99_96_info;
	u32 dfd_v50_chain_group_103_100_info;
	u32 dfd_v50_chain_group_107_104_info;
	u32 dfd_v50_chain_group_111_108_info;
	u32 dfd_v50_chain_group_115_112_info;
	u32 dfd_v50_chain_group_119_116_info;
	u32 dfd_v50_chain_group_123_120_info;
	u32 dfd_v50_chain_group_127_124_info;
	u32 dfd_test_si_2;
	u32 dfd_test_si_3;
	u32 dfd_test_so;
	u32 dfd_bus_halt_time;
	u32 dfd_read_addr;
	u32 dfd_v50_clk_stop_time;
	u32 dfd_v50_group_0_32_diff;
	u32 dfd_v50_group_0_33_diff;
	u32 dfd_v50_group_0_34_diff;
	u32 dfd_v50_group_0_35_diff;
	u32 dfd_v50_group_0_36_diff;
	u32 dfd_v50_group_0_37_diff;
	u32 dfd_v50_group_0_38_diff;
	u32 dfd_v50_group_0_39_diff;
	u32 dfd_v50_group_0_40_diff;
	u32 dfd_v50_group_0_41_diff;
	u32 dfd_v50_group_0_42_diff;
	u32 dfd_v50_group_0_43_diff;
	u32 dfd_v50_group_0_44_diff;
	u32 dfd_v50_group_0_45_diff;
	u32 dfd_v50_group_0_46_diff;
	u32 dfd_v50_group_0_47_diff;
	u32 dfd_v50_group_0_48_diff;
	u32 dfd_v50_group_0_49_diff;
	u32 dfd_v50_group_0_50_diff;
	u32 dfd_v50_group_0_51_diff;
	u32 dfd_v50_group_0_52_diff;
	u32 dfd_v50_group_0_53_diff;
	u32 dfd_v50_group_0_54_diff;
	u32 dfd_v50_group_0_55_diff;
	u32 dfd_v50_group_0_56_diff;
	u32 dfd_v50_group_0_57_diff;
	u32 dfd_v50_group_0_58_diff;
	u32 dfd_v50_group_0_59_diff;
	u32 dfd_v50_group_0_60_diff;
	u32 dfd_v50_group_0_61_diff;
	u32 dfd_v50_group_0_62_diff;
	u32 dfd_v50_group_0_63_diff;
	u32 reserved149[20];
	u32 dfd_v50_spare;	/* 0x6300 */
	u32 reserved150[1843];
	u32 mcusys_reserved_reg4;	/* 0x7fd0 */
	u32 mcusys_reserved_reg4_rd;
	u32 reserved151[2];
	u32 mcusys_reserved_reg0;	/* 0x7fe0 */
	u32 mcusys_reserved_reg1;
	u32 mcusys_reserved_reg2;
	u32 mcusys_reserved_reg3;
	u32 mcusys_reserved_reg0_rd;
	u32 mcusys_reserved_reg1_rd;
	u32 mcusys_reserved_reg2_rd;
	u32 mcusys_reserved_reg3_rd;
};

check_member(mt8192_mcucfg_regs, mbista_mcsi_sf1_con, 0x0008);
check_member(mt8192_mcucfg_regs, mbist_trigger_mux_ctl, 0x0030);
check_member(mt8192_mcucfg_regs, dfd_ctrl, 0x0040);
check_member(mt8192_mcucfg_regs, mp_top_dbg_mon_sel, 0x0060);
check_member(mt8192_mcucfg_regs, mp0_ptp_sensor_sel, 0x00e0);
check_member(mt8192_mcucfg_regs, armpll_jit_clk_out_sel, 0x00f8);
check_member(mt8192_mcucfg_regs, cci_tra_cfg5, 0x0114);
check_member(mt8192_mcucfg_regs, cci_m0_tra, 0x0200);
check_member(mt8192_mcucfg_regs, cci_s1_tra, 0x0220);
check_member(mt8192_mcucfg_regs, cci_m0_tra_latch, 0x0240);
check_member(mt8192_mcucfg_regs, cci_s1_tra_latch, 0x0260);
check_member(mt8192_mcucfg_regs, cci_m0_if, 0x02c0);
check_member(mt8192_mcucfg_regs, cci_s1_if, 0x02e0);
check_member(mt8192_mcucfg_regs, cci_m0_if_latch, 0x0320);
check_member(mt8192_mcucfg_regs, cci_s1_if_latch, 0x0340);
check_member(mt8192_mcucfg_regs, l3c_share_status0, 0x0400);
check_member(mt8192_mcucfg_regs, mp0_cpu0_nonwfx_ctrl, 0x0500);
check_member(mt8192_mcucfg_regs, mp0_ses_apb_trig, 0x0600);
check_member(mt8192_mcucfg_regs, wfx_ret_met_dbc_sel, 0x0610);
check_member(mt8192_mcucfg_regs, adb_bist_cfg1, 0x0620);
check_member(mt8192_mcucfg_regs, axi2acp_cfg_ctrl, 0x0700);
check_member(mt8192_mcucfg_regs, axi2acp_ar_def_set, 0x0710);
check_member(mt8192_mcucfg_regs, mst_ccim0_inject_fault, 0x0800);
check_member(mt8192_mcucfg_regs, slv_1to2_inject_fault, 0x0860);
check_member(mt8192_mcucfg_regs, slv_1to2_log_wd0, 0x0890);
check_member(mt8192_mcucfg_regs, ildo_vin_big0, 0x0a00);
check_member(mt8192_mcucfg_regs, ildo_vin_big1, 0x0a30);
check_member(mt8192_mcucfg_regs, ildo_vin_big2, 0x0a60);
check_member(mt8192_mcucfg_regs, ildo_vin_big3, 0x0a90);
check_member(mt8192_mcucfg_regs, l3c_share_cfg0, 0x2210);
check_member(mt8192_mcucfg_regs, mcusys_core_status, 0x2230);
check_member(mt8192_mcucfg_regs, mcusys_base, 0x2260);
check_member(mt8192_mcucfg_regs, bus_plldiv_cfg, 0x22e0);
check_member(mt8192_mcucfg_regs, plldiv_ctl0, 0x22f0);
check_member(mt8192_mcucfg_regs, mcsi_ram_delsel0, 0x2300);
check_member(mt8192_mcucfg_regs, etb_cfg0, 0x2380);
check_member(mt8192_mcucfg_regs, bus_parity_gen_en, 0x2390);
check_member(mt8192_mcucfg_regs, dsu2biu_addr_remap_0_cfg0, 0x23b0);
check_member(mt8192_mcucfg_regs, cci_rgu, 0x2400);
check_member(mt8192_mcucfg_regs, mcsi_cfg0, 0x2410);
check_member(mt8192_mcucfg_regs, mcsic_dcm0, 0x2440);
check_member(mt8192_mcucfg_regs, apmcu2emi_early_cke_ctl01, 0x2490);
check_member(mt8192_mcucfg_regs, mp_adb_dcm_cfg0, 0x2500);
check_member(mt8192_mcucfg_regs, etb_ck_ctl, 0x2540);
check_member(mt8192_mcucfg_regs, dcc_cpu_con0, 0x2580);
check_member(mt8192_mcucfg_regs, dcc_bus_con0, 0x25a0);
check_member(mt8192_mcucfg_regs, mcusys_dcm_cfg0, 0x25c0);
check_member(mt8192_mcucfg_regs, fcm_spmc_sw_cfg1, 0x2700);
check_member(mt8192_mcucfg_regs, mcusys_spmc_sw_cfg, 0x2740);
check_member(mt8192_mcucfg_regs, cpc_pllbuck_req_ctrl, 0x2800);
check_member(mt8192_mcucfg_regs, cpc_spmc_pwr_status, 0x2840);
check_member(mt8192_mcucfg_regs, emi_wfifo, 0x2900);
check_member(mt8192_mcucfg_regs, sclk_cfg_slow_down_ck, 0x2920);
check_member(mt8192_mcucfg_regs, acpwakeup, 0x2930);
check_member(mt8192_mcucfg_regs, mcusys_dbg_mon_sel, 0x2990);
check_member(mt8192_mcucfg_regs, gic_acao_ctl0, 0x2a80);
check_member(mt8192_mcucfg_regs, spmc_dbg_setting, 0x2b00);
check_member(mt8192_mcucfg_regs, trace_data_entry0_l, 0x2b20);
check_member(mt8192_mcucfg_regs, cpc_wdt_latch_info6, 0x2bb0);
check_member(mt8192_mcucfg_regs, sesv6_bg_ctrl, 0x2c40);
check_member(mt8192_mcucfg_regs, dsu_sesv6_ao_reg0, 0x2c50);
check_member(mt8192_mcucfg_regs, ildo_dout_sel, 0x2d00);
check_member(mt8192_mcucfg_regs, dreq20_little_en, 0x2d80);
check_member(mt8192_mcucfg_regs, dreq20_big_vproc_iso, 0x2d8c);
check_member(mt8192_mcucfg_regs, pikachu_event, 0x2e00);
check_member(mt8192_mcucfg_regs, cpu0_drcc_ao_config, 0x3000);
check_member(mt8192_mcucfg_regs, cpu0_sesv6_ao_reg0, 0x3010);
check_member(mt8192_mcucfg_regs, cpu0_dfs_cfg, 0x3040);
check_member(mt8192_mcucfg_regs, cpu0_turbo_cfg, 0x3050);
check_member(mt8192_mcucfg_regs, cpu0_resereved_reg, 0x31f8);
check_member(mt8192_mcucfg_regs, cpu1_sesv6_ao_reg0, 0x3210);
check_member(mt8192_mcucfg_regs, cpu1_dfs_cfg, 0x3240);
check_member(mt8192_mcucfg_regs, cpu1_turbo_cfg, 0x3250);
check_member(mt8192_mcucfg_regs, cpu1_resereved_reg, 0x33f8);
check_member(mt8192_mcucfg_regs, cpu2_sesv6_ao_reg0, 0x3410);
check_member(mt8192_mcucfg_regs, cpu2_dfs_cfg, 0x3440);
check_member(mt8192_mcucfg_regs, cpu2_turbo_cfg, 0x3450);
check_member(mt8192_mcucfg_regs, cpu2_resereved_reg, 0x35f8);
check_member(mt8192_mcucfg_regs, cpu3_sesv6_ao_reg0, 0x3610);
check_member(mt8192_mcucfg_regs, cpu3_dfs_cfg, 0x3640);
check_member(mt8192_mcucfg_regs, cpu3_turbo_cfg, 0x3650);
check_member(mt8192_mcucfg_regs, cpu3_resereved_reg, 0x37f8);
check_member(mt8192_mcucfg_regs, cpu4_sesv6_ao_reg0, 0x3810);
check_member(mt8192_mcucfg_regs, cpu4_didt_reg, 0x3830);
check_member(mt8192_mcucfg_regs, cpu4_dfs_cfg, 0x3840);
check_member(mt8192_mcucfg_regs, cpu4_turbo_cfg, 0x3850);
check_member(mt8192_mcucfg_regs, cpu4_resereved_reg, 0x39f8);
check_member(mt8192_mcucfg_regs, cpu5_sesv6_ao_reg0, 0x3a10);
check_member(mt8192_mcucfg_regs, cpu5_didt_reg, 0x3a30);
check_member(mt8192_mcucfg_regs, cpu5_dfs_cfg, 0x3a40);
check_member(mt8192_mcucfg_regs, cpu5_turbo_cfg, 0x3a50);
check_member(mt8192_mcucfg_regs, cpu5_resereved_reg, 0x3bf8);
check_member(mt8192_mcucfg_regs, cpu6_sesv6_ao_reg0, 0x3c10);
check_member(mt8192_mcucfg_regs, cpu6_didt_reg, 0x3c30);
check_member(mt8192_mcucfg_regs, cpu6_dfs_cfg, 0x3c40);
check_member(mt8192_mcucfg_regs, cpu6_turbo_cfg, 0x3c50);
check_member(mt8192_mcucfg_regs, cpu6_resereved_reg, 0x3df8);
check_member(mt8192_mcucfg_regs, cpu7_sesv6_ao_reg0, 0x3e10);
check_member(mt8192_mcucfg_regs, cpu7_didt_reg, 0x3e30);
check_member(mt8192_mcucfg_regs, cpu7_dfs_cfg, 0x3e40);
check_member(mt8192_mcucfg_regs, cpu7_turbo_cfg, 0x3e50);
check_member(mt8192_mcucfg_regs, cpu7_resereved_reg, 0x3ff8);
check_member(mt8192_mcucfg_regs, mp0_mbist_cfg, 0x4800);
check_member(mt8192_mcucfg_regs, mp0_l3_data_ram_delsel, 0x4840);
check_member(mt8192_mcucfg_regs, mp0_dcm_cfg0, 0x4880);
check_member(mt8192_mcucfg_regs, mp0_l3_cache_parity1, 0x48c0);
check_member(mt8192_mcucfg_regs, mp0_cluster_cfg4, 0x48e0);
check_member(mt8192_mcucfg_regs, mp0_cluster_cfg8, 0x4900);
check_member(mt8192_mcucfg_regs, cpu_type0_spmc0_cfg, 0x4c00);
check_member(mt8192_mcucfg_regs, cpu_type0_ram_delsel0_cfg, 0x4c20);
check_member(mt8192_mcucfg_regs, cpu_type1_spmc0_cfg, 0x4d00);
check_member(mt8192_mcucfg_regs, cpu_type1_mpmmen, 0x4d10);
check_member(mt8192_mcucfg_regs, cpu_type1_ram_delsel0_cfg, 0x4d20);
check_member(mt8192_mcucfg_regs, cpu_type1_drss_cfg0, 0x4d40);
check_member(mt8192_mcucfg_regs, mcusys_pwr_con, 0x5200);
check_member(mt8192_mcucfg_regs, sec_pol_ctl_en0, 0x5600);
check_member(mt8192_mcucfg_regs, int_pol_ctl0, 0x5668);
check_member(mt8192_mcucfg_regs, int_msk_ctl0, 0x56d0);
check_member(mt8192_mcucfg_regs, int_msk_ctl_all, 0x5744);
check_member(mt8192_mcucfg_regs, dfd_internal_ctl, 0x6040);
check_member(mt8192_mcucfg_regs, dfd_soc_clock_stop_mask, 0x60f0);
check_member(mt8192_mcucfg_regs, dfd_hw_trigger_mask, 0x60fc);
check_member(mt8192_mcucfg_regs, dfd_v50_spare, 0x6300);
check_member(mt8192_mcucfg_regs, mcusys_reserved_reg4, 0x7fd0);
check_member(mt8192_mcucfg_regs, mcusys_reserved_reg0, 0x7fe0);
check_member(mt8192_mcucfg_regs, mcusys_reserved_reg3_rd, 0x7ffc);

static struct mt8192_mcucfg_regs *const mtk_mcucfg = (void *)MCUCFG_BASE;

#endif  /* SOC_MEDIATEK_MT8192_MCUCFG_H */
