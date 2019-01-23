/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _DRAMC_REGISTER_H_
#define _DRAMC_REGISTER_H_

#include <types.h>
#include <soc/addressmap.h>

struct dramc_no_regs_rk_counter {
	uint32_t pre_standby_counter;
	uint32_t pre_powerdown_counter;
	uint32_t act_standby_counter;
	uint32_t act_powerdown_counter;
};

struct dramc_nao_regs_rk {
	uint32_t dqsosc_status;
	uint32_t dqsosc_delta;
	uint32_t dqsosc_delta2;
	uint32_t rsvd_1[1];
	uint32_t current_tx_setting1;
	uint32_t current_tx_setting2;
	uint32_t current_tx_setting3;
	uint32_t current_tx_setting4;
	uint32_t dummy_rd_data[4];
	uint32_t b0_stb_max_min_dly;
	uint32_t b1_stb_max_min_dly;
	uint32_t b2_stb_max_min_dly;
	uint32_t b3_stb_max_min_dly;
	uint32_t dqsiendly;
	uint32_t dqsienuidly;
	uint32_t dqsienuidly_p1;
	uint32_t rsvd_2[1];
	uint32_t dqs_stbcaldec_cnt1;
	uint32_t dqs_stbcaldec_cnt2;
	uint32_t dqs_stbcalinc_cnt1;
	uint32_t dqs_stbcalinc_cnt2;
	uint32_t fine_tune_dq_cal;
	uint32_t dqsg_retry_flag;
	uint32_t fine_tune_dqm_cal;
	uint32_t rsvd_3[1];
	uint32_t dqs0_stbcal_cnt;
	uint32_t dqs1_stbcal_cnt;
	uint32_t dqs2_stbcal_cnt;
	uint32_t dqs3_stbcal_cnt;
	uint32_t b01_stb_dbg_info[16];
	uint32_t b23_stb_dbg_info[16];
};

struct dramc_nao_regs {
	uint32_t testmode;
	uint32_t lbwdat0;
	uint32_t lbwdat1;
	uint32_t lbwdat2;
	uint32_t lbwdat3;
	uint32_t reserved0[3];
	uint32_t ckphchk;
	uint32_t dmmonitor;
	uint32_t reserved1[2];
	uint32_t testchip_dma1;
	uint32_t reserved2[19];
	uint32_t misc_statusa;
	uint32_t special_status;
	uint32_t spcmdresp;
	uint32_t mrr_status;
	uint32_t mrr_status2;
	uint32_t mrrdata0;
	uint32_t mrrdata1;
	uint32_t mrrdata2;
	uint32_t mrrdata3;
	uint32_t reserved3[1];
	uint32_t drs_status;
	uint32_t reserved4[4];
	uint32_t jmeter_st;
	uint32_t tcmdo1lat;
	uint32_t rdqc_cmp;
	uint32_t ckphchk_status;
	uint32_t reserved5[16];
	uint32_t hwmrr_push2pop_cnt;
	uint32_t hwmrr_status;
	uint32_t hw_refrate_mon;
	uint32_t reserved6[2];
	uint32_t testrpt;
	uint32_t cmp_err;
	uint32_t test_abit_status1;
	uint32_t test_abit_status2;
	uint32_t test_abit_status3;
	uint32_t test_abit_status4;
	uint32_t reserved7[6];
	uint32_t dqsdly0;
	uint32_t dq_cal_max[8];
	uint32_t dqs_cal_min[8];
	uint32_t dqs_cal_max[8];
	uint32_t dqical0;
	uint32_t dqical1;
	uint32_t dqical2;
	uint32_t dqical3;
	uint32_t reserved8[15];
	uint32_t testchip_dma_status[34];
	uint32_t reserved9[30];
	uint32_t refresh_pop_counter;
	uint32_t freerun_26m_counter;
	uint32_t dramc_idle_counter;
	uint32_t r2r_page_hit_counter;
	uint32_t r2r_page_miss_counter;
	uint32_t r2r_interbank_counter;
	uint32_t r2w_page_hit_counter;
	uint32_t r2w_page_miss_counter;
	uint32_t r2w_interbank_counter;
	uint32_t w2r_page_hit_counter;
	uint32_t w2r_page_miss_counter;
	uint32_t w2r_interbank_counter;
	uint32_t w2w_page_hit_counter;
	uint32_t w2w_page_miss_counter;
	uint32_t w2w_interbank_counter;
	struct dramc_no_regs_rk_counter rk_counter[3];
	uint32_t dq0_toggle_counter;
	uint32_t dq1_toggle_counter;
	uint32_t dq2_toggle_counter;
	uint32_t dq3_toggle_counter;
	uint32_t dq0_toggle_counter_r;
	uint32_t dq1_toggle_counter_r;
	uint32_t dq2_toggle_counter_r;
	uint32_t dq3_toggle_counter_r;
	uint32_t read_bytes_counter;
	uint32_t write_bytes_counter;
	uint32_t max_sref_req_to_ack_latency_counter;
	uint32_t max_rk1_drs_long_req_to_ack_latency_counter;
	uint32_t max_rk1_drs_req_to_ack_latency_counter;
	uint32_t reserved10[8];
	uint32_t lat_counter_cmd[8];
	uint32_t lat_counter_aver;
	uint32_t lat_counter_num;
	uint32_t lat_counter_block_ale;
	uint32_t reserved11[5];
	uint32_t dqssamplev;
	uint32_t reserved12[1];
	uint32_t dqsgnwcnt[6];
	uint32_t toggle_cnt;
	uint32_t dqs0_err_cnt;
	uint32_t dq_err_cnt0;
	uint32_t dqs1_err_cnt;
	uint32_t dq_err_cnt1;
	uint32_t dqs2_err_cnt;
	uint32_t dq_err_cnt2;
	uint32_t dqs3_err_cnt;
	uint32_t dq_err_cnt3;
	uint32_t reserved13[3];
	uint32_t iorgcnt;
	uint32_t dqsg_retry_state;
	uint32_t dqsg_retry_state1;
	uint32_t reserved14[1];
	uint32_t impcal_status1;
	uint32_t impcal_status2;
	uint32_t dqdrv_status;
	uint32_t cmddrv_status;
	uint32_t cmddrv1;
	uint32_t cmddrv2;
	uint32_t reserved15[98];
	struct dramc_nao_regs_rk rk[3];
	uint32_t reserved25[192];
	uint32_t dvfs_dbg0;
	uint32_t dvfs_dbg1;
	uint32_t dramc_nao_regs_end;
};

check_member(dramc_nao_regs, testmode, 0x0000);
check_member(dramc_nao_regs, lbwdat0, 0x0004);
check_member(dramc_nao_regs, lbwdat1, 0x0008);
check_member(dramc_nao_regs, lbwdat2, 0x000c);
check_member(dramc_nao_regs, lbwdat3, 0x0010);
check_member(dramc_nao_regs, ckphchk, 0x0020);
check_member(dramc_nao_regs, dmmonitor, 0x0024);
check_member(dramc_nao_regs, testchip_dma1, 0x0030);
check_member(dramc_nao_regs, misc_statusa, 0x0080);
check_member(dramc_nao_regs, special_status, 0x0084);
check_member(dramc_nao_regs, spcmdresp, 0x0088);
check_member(dramc_nao_regs, mrr_status, 0x008c);
check_member(dramc_nao_regs, mrr_status2, 0x0090);
check_member(dramc_nao_regs, mrrdata0, 0x0094);
check_member(dramc_nao_regs, mrrdata1, 0x0098);
check_member(dramc_nao_regs, mrrdata2, 0x009c);
check_member(dramc_nao_regs, mrrdata3, 0x00a0);
check_member(dramc_nao_regs, drs_status, 0x00a8);
check_member(dramc_nao_regs, jmeter_st, 0x00bc);
check_member(dramc_nao_regs, tcmdo1lat, 0x00c0);
check_member(dramc_nao_regs, rdqc_cmp, 0x00c4);
check_member(dramc_nao_regs, ckphchk_status, 0x00c8);
check_member(dramc_nao_regs, hwmrr_push2pop_cnt, 0x010c);
check_member(dramc_nao_regs, hwmrr_status, 0x0110);
check_member(dramc_nao_regs, testrpt, 0x0120);
check_member(dramc_nao_regs, cmp_err, 0x0124);
check_member(dramc_nao_regs, test_abit_status1, 0x0128);
check_member(dramc_nao_regs, test_abit_status2, 0x012c);
check_member(dramc_nao_regs, test_abit_status3, 0x0130);
check_member(dramc_nao_regs, test_abit_status4, 0x0134);
check_member(dramc_nao_regs, dqsdly0, 0x0150);
check_member(dramc_nao_regs, dq_cal_max[0], 0x0154);
check_member(dramc_nao_regs, dqs_cal_min[0], 0x0174);
check_member(dramc_nao_regs, dqs_cal_max[0], 0x0194);
check_member(dramc_nao_regs, dqical0, 0x01b4);
check_member(dramc_nao_regs, dqical1, 0x01b8);
check_member(dramc_nao_regs, dqical2, 0x01bc);
check_member(dramc_nao_regs, dqical3, 0x01c0);
check_member(dramc_nao_regs, testchip_dma_status[0], 0x0200);
check_member(dramc_nao_regs, refresh_pop_counter, 0x0300);
check_member(dramc_nao_regs, freerun_26m_counter, 0x0304);
check_member(dramc_nao_regs, dramc_idle_counter, 0x0308);
check_member(dramc_nao_regs, r2r_page_hit_counter, 0x030c);
check_member(dramc_nao_regs, r2r_page_miss_counter, 0x0310);
check_member(dramc_nao_regs, r2r_interbank_counter, 0x0314);
check_member(dramc_nao_regs, r2w_page_hit_counter, 0x0318);
check_member(dramc_nao_regs, r2w_page_miss_counter, 0x031c);
check_member(dramc_nao_regs, r2w_interbank_counter, 0x0320);
check_member(dramc_nao_regs, w2r_page_hit_counter, 0x0324);
check_member(dramc_nao_regs, w2r_page_miss_counter, 0x0328);
check_member(dramc_nao_regs, w2r_interbank_counter, 0x032c);
check_member(dramc_nao_regs, w2w_page_hit_counter, 0x0330);
check_member(dramc_nao_regs, w2w_page_miss_counter, 0x0334);
check_member(dramc_nao_regs, w2w_interbank_counter, 0x0338);
check_member(dramc_nao_regs, dq0_toggle_counter, 0x036c);
check_member(dramc_nao_regs, dq1_toggle_counter, 0x0370);
check_member(dramc_nao_regs, dq2_toggle_counter, 0x0374);
check_member(dramc_nao_regs, dq3_toggle_counter, 0x0378);
check_member(dramc_nao_regs, dq0_toggle_counter_r, 0x037c);
check_member(dramc_nao_regs, dq1_toggle_counter_r, 0x0380);
check_member(dramc_nao_regs, dq2_toggle_counter_r, 0x0384);
check_member(dramc_nao_regs, dq3_toggle_counter_r, 0x0388);
check_member(dramc_nao_regs, read_bytes_counter, 0x038c);
check_member(dramc_nao_regs, write_bytes_counter, 0x0390);
check_member(dramc_nao_regs, dqssamplev, 0x0400);
check_member(dramc_nao_regs, dqsgnwcnt[0], 0x0408);
check_member(dramc_nao_regs, toggle_cnt, 0x0420);
check_member(dramc_nao_regs, dqs0_err_cnt, 0x0424);
check_member(dramc_nao_regs, dq_err_cnt0, 0x0428);
check_member(dramc_nao_regs, dqs1_err_cnt, 0x042c);
check_member(dramc_nao_regs, dq_err_cnt1, 0x0430);
check_member(dramc_nao_regs, dqs2_err_cnt, 0x0434);
check_member(dramc_nao_regs, dq_err_cnt2, 0x0438);
check_member(dramc_nao_regs, dqs3_err_cnt, 0x043c);
check_member(dramc_nao_regs, dq_err_cnt3, 0x0440);
check_member(dramc_nao_regs, iorgcnt, 0x0450);
check_member(dramc_nao_regs, dqsg_retry_state, 0x0454);
check_member(dramc_nao_regs, dqsg_retry_state1, 0x0458);
check_member(dramc_nao_regs, impcal_status1, 0x0460);
check_member(dramc_nao_regs, impcal_status2, 0x0464);
check_member(dramc_nao_regs, dqdrv_status, 0x0468);
check_member(dramc_nao_regs, cmddrv_status, 0x046c);
check_member(dramc_nao_regs, cmddrv1, 0x0470);
check_member(dramc_nao_regs, cmddrv2, 0x0474);

struct dramc_ao_regs_rk {
	uint32_t dqsosc;
	uint32_t rsvd_1[5];
	uint32_t dummy_rd_wdata[4];
	uint32_t dummy_rd_adr;
	uint32_t dummy_rd_bk;
	uint32_t pre_tdqsck[12];
	uint32_t rsvd_2[40];
};

struct dramc_ao_regs_shu_rk {
	uint32_t dqsctl;
	uint32_t dqsien;
	uint32_t dqscal;
	uint32_t fine_tune;
	uint32_t dqsosc;
	uint32_t rsvd_1[2];
	uint32_t selph_odten0;
	uint32_t selph_odten1;
	uint32_t selph_dqsg0;
	uint32_t selph_dqsg1;
	uint32_t selph_dq[4];
	uint32_t rsvd_2[1];
	uint32_t dqs2dq_cal1;
	uint32_t dqs2dq_cal2;
	uint32_t dqs2dq_cal3;
	uint32_t dqs2dq_cal4;
	uint32_t dqs2dq_cal5;
	uint32_t rsvd_3[43];
};

struct dramc_ao_regs {
	uint32_t ddrconf0;
	uint32_t dramctrl;
	uint32_t misctl0;
	uint32_t perfctl0;
	uint32_t arbctl;
	uint32_t reserved0[2];
	uint32_t rstmask;
	uint32_t padctrl;
	uint32_t ckectrl;
	uint32_t drsctrl;
	uint32_t reserved1[2];
	uint32_t rkcfg;
	uint32_t dramc_pd_ctrl;
	uint32_t clkar;
	uint32_t clkctrl;
	uint32_t selfref_hwsave_flag;
	uint32_t srefctrl;
	uint32_t refctrl0;
	uint32_t refctrl1;
	uint32_t refratre_filter;
	uint32_t zqcs;
	uint32_t mrs;
	uint32_t spcmd;
	uint32_t spcmdctrl;
	uint32_t ppr_ctrl;
	uint32_t mpc_option;
	uint32_t refque_cnt;
	uint32_t hw_mrr_fun;
	uint32_t mrr_bit_mux1;
	uint32_t mrr_bit_mux2;
	uint32_t mrr_bit_mux3;
	uint32_t mrr_bit_mux4;
	uint32_t reserved2[1];
	uint32_t test2_5;
	uint32_t test2_0;
	uint32_t test2_1;
	uint32_t test2_2;
	uint32_t test2_3;
	uint32_t test2_4;
	uint32_t wdt_dbg_signal;
	uint32_t reserved3[1];
	uint32_t lbtest;
	uint32_t catraining1;
	uint32_t catraining2;
	uint32_t reserved4[1];
	uint32_t write_lev;
	uint32_t mr_golden;
	uint32_t slp4_testmode;
	uint32_t dqsoscr;
	uint32_t reserved5[1];
	uint32_t dummy_rd;
	uint32_t shuctrl;
	uint32_t shuctrl1;
	uint32_t shuctrl2;
	uint32_t shuctrl3;
	uint32_t shustatus;
	uint32_t reserved6[70];
	uint32_t stbcal;
	uint32_t stbcal1;
	uint32_t stbcal2;
	uint32_t eyescan;
	uint32_t dvfsdll;
	uint32_t reserved7[1];
	uint32_t pre_tdqsck[4];
	uint32_t reserved8[1];
	uint32_t impcal;
	uint32_t impedamce_ctrl1;
	uint32_t impedamce_ctrl2;
	uint32_t impedamce_ctrl3;
	uint32_t impedamce_ctrl4;
	uint32_t dramc_dbg_sel1;
	uint32_t dramc_dbg_sel2;
	uint32_t rsvd_10[46];
	struct dramc_ao_regs_rk rk[3];
	uint32_t rsvd_16[64];
	struct {
		uint32_t rsvd0[64];
		uint32_t actim[7];
		uint32_t actim_xrt;
		uint32_t ac_time_05t;
		uint32_t ac_derating0;
		uint32_t ac_derating1;
		uint32_t rsvd1[1];
		uint32_t ac_derating_05t;
		uint32_t rsvd2[3];
		uint32_t conf[4];
		uint32_t stbcal;
		uint32_t dqsoscthrd;
		uint32_t rankctl;
		uint32_t ckectrl;
		uint32_t odtctrl;
		uint32_t impcal1;
		uint32_t dqsosc_prd;
		uint32_t dqsoscr;
		uint32_t dqsoscr2;
		uint32_t rodtenstb;
		uint32_t pipe;
		uint32_t test1;
		uint32_t selph_ca1;
		uint32_t selph_ca2;
		uint32_t selph_ca3;
		uint32_t selph_ca4;
		uint32_t selph_ca5;
		uint32_t selph_ca6;
		uint32_t selph_ca7;
		uint32_t selph_ca8;
		uint32_t selph_dqs0;
		uint32_t selph_dqs1;
		uint32_t drving[6];
		uint32_t wodt;
		uint32_t dqsg;
		uint32_t scintv;
		uint32_t misc;
		uint32_t dqs2dq_tx;
		uint32_t hwset_mr2;
		uint32_t hwset_mr13;
		uint32_t hwset_vrcg;
		uint32_t rsvd3[72];
		union {
			struct dramc_ao_regs_shu_rk rk[3];
			struct {
				uint32_t rsvd_63[149];
				uint32_t dqsg_retry;
			};
		};
	} shu[4];
	uint32_t dramc_ao_regs_end;
};

check_member(dramc_ao_regs, ddrconf0, 0x0000);
check_member(dramc_ao_regs, dramctrl, 0x0004);
check_member(dramc_ao_regs, misctl0, 0x0008);
check_member(dramc_ao_regs, perfctl0, 0x000c);
check_member(dramc_ao_regs, arbctl, 0x0010);
check_member(dramc_ao_regs, rstmask, 0x001c);
check_member(dramc_ao_regs, padctrl, 0x0020);
check_member(dramc_ao_regs, ckectrl, 0x0024);
check_member(dramc_ao_regs, drsctrl, 0x0028);
check_member(dramc_ao_regs, rkcfg, 0x0034);
check_member(dramc_ao_regs, dramc_pd_ctrl, 0x0038);
check_member(dramc_ao_regs, clkar, 0x003c);
check_member(dramc_ao_regs, clkctrl, 0x0040);
check_member(dramc_ao_regs, selfref_hwsave_flag, 0x0044);
check_member(dramc_ao_regs, srefctrl, 0x0048);
check_member(dramc_ao_regs, refctrl0, 0x004c);
check_member(dramc_ao_regs, refctrl1, 0x0050);
check_member(dramc_ao_regs, refratre_filter, 0x0054);
check_member(dramc_ao_regs, zqcs, 0x0058);
check_member(dramc_ao_regs, mrs, 0x005c);
check_member(dramc_ao_regs, spcmd, 0x0060);
check_member(dramc_ao_regs, spcmdctrl, 0x0064);
check_member(dramc_ao_regs, ppr_ctrl, 0x0068);
check_member(dramc_ao_regs, mpc_option, 0x006c);
check_member(dramc_ao_regs, refque_cnt, 0x0070);
check_member(dramc_ao_regs, hw_mrr_fun, 0x0074);
check_member(dramc_ao_regs, mrr_bit_mux1, 0x0078);
check_member(dramc_ao_regs, mrr_bit_mux2, 0x007c);
check_member(dramc_ao_regs, mrr_bit_mux3, 0x0080);
check_member(dramc_ao_regs, mrr_bit_mux4, 0x0084);
check_member(dramc_ao_regs, test2_5, 0x008c);
check_member(dramc_ao_regs, test2_0, 0x0090);
check_member(dramc_ao_regs, test2_1, 0x0094);
check_member(dramc_ao_regs, test2_2, 0x0098);
check_member(dramc_ao_regs, test2_3, 0x009c);
check_member(dramc_ao_regs, test2_4, 0x00a0);
check_member(dramc_ao_regs, wdt_dbg_signal, 0x00a4);
check_member(dramc_ao_regs, lbtest, 0x00ac);
check_member(dramc_ao_regs, catraining1, 0x00b0);
check_member(dramc_ao_regs, catraining2, 0x00b4);
check_member(dramc_ao_regs, write_lev, 0x00bc);
check_member(dramc_ao_regs, mr_golden, 0x00c0);
check_member(dramc_ao_regs, slp4_testmode, 0x00c4);
check_member(dramc_ao_regs, dqsoscr, 0x00c8);
check_member(dramc_ao_regs, dummy_rd, 0x00d0);
check_member(dramc_ao_regs, shuctrl, 0x00d4);
check_member(dramc_ao_regs, shuctrl1, 0x00d8);
check_member(dramc_ao_regs, shuctrl2, 0x00dc);
check_member(dramc_ao_regs, shuctrl3, 0x00e0);
check_member(dramc_ao_regs, shustatus, 0x00e4);
check_member(dramc_ao_regs, stbcal, 0x0200);
check_member(dramc_ao_regs, stbcal1, 0x0204);
check_member(dramc_ao_regs, stbcal2, 0x0208);
check_member(dramc_ao_regs, eyescan, 0x020c);
check_member(dramc_ao_regs, dvfsdll, 0x0210);
check_member(dramc_ao_regs, pre_tdqsck[0], 0x0218);
check_member(dramc_ao_regs, pre_tdqsck[1], 0x021c);
check_member(dramc_ao_regs, pre_tdqsck[2], 0x0220);
check_member(dramc_ao_regs, pre_tdqsck[3], 0x0224);
check_member(dramc_ao_regs, impcal, 0x022c);
check_member(dramc_ao_regs, impedamce_ctrl1, 0x0230);
check_member(dramc_ao_regs, impedamce_ctrl2, 0x0234);
check_member(dramc_ao_regs, impedamce_ctrl3, 0x0238);
check_member(dramc_ao_regs, impedamce_ctrl4, 0x023c);
check_member(dramc_ao_regs, dramc_dbg_sel1, 0x0240);
check_member(dramc_ao_regs, dramc_dbg_sel2, 0x0244);
check_member(dramc_ao_regs, shu[0].actim[0], 0x0800);
check_member(dramc_ao_regs, shu[0].actim_xrt, 0x081c);
check_member(dramc_ao_regs, shu[0].ac_time_05t, 0x0820);
check_member(dramc_ao_regs, shu[0].ac_derating0, 0x0824);
check_member(dramc_ao_regs, shu[0].ac_derating1, 0x0828);
check_member(dramc_ao_regs, shu[0].ac_derating_05t, 0x0830);
check_member(dramc_ao_regs, shu[0].conf[0], 0x0840);
check_member(dramc_ao_regs, shu[0].rankctl, 0x0858);
check_member(dramc_ao_regs, shu[0].ckectrl, 0x085c);
check_member(dramc_ao_regs, shu[0].odtctrl, 0x0860);
check_member(dramc_ao_regs, shu[0].impcal1, 0x0864);
check_member(dramc_ao_regs, shu[0].dqsosc_prd, 0x0868);
check_member(dramc_ao_regs, shu[0].dqsoscr, 0x086c);
check_member(dramc_ao_regs, shu[0].dqsoscr2, 0x0870);
check_member(dramc_ao_regs, shu[0].rodtenstb, 0x0874);
check_member(dramc_ao_regs, shu[0].pipe, 0x0878);
check_member(dramc_ao_regs, shu[0].test1, 0x087c);
check_member(dramc_ao_regs, shu[0].selph_ca1, 0x0880);
check_member(dramc_ao_regs, shu[0].selph_dqs0, 0x08a0);
check_member(dramc_ao_regs, shu[0].selph_dqs1, 0x08a4);
check_member(dramc_ao_regs, shu[0].drving[0], 0x08a8);
check_member(dramc_ao_regs, shu[0].wodt, 0x08c0);
check_member(dramc_ao_regs, shu[0].dqsg, 0x08c4);
check_member(dramc_ao_regs, shu[0].scintv, 0x08c8);
check_member(dramc_ao_regs, shu[0].misc, 0x08cc);
check_member(dramc_ao_regs, shu[0].dqs2dq_tx, 0x08d0);
check_member(dramc_ao_regs, shu[0].hwset_mr2, 0x08d4);
check_member(dramc_ao_regs, shu[0].hwset_mr13, 0x08d8);
check_member(dramc_ao_regs, shu[0].hwset_vrcg, 0x08dc);
check_member(dramc_ao_regs, shu[0].rk[0].dqsctl, 0x0A00);
check_member(dramc_ao_regs, shu[0].dqsg_retry, 0x0c54);
check_member(dramc_ao_regs, shu[1].dqsg_retry, 0x1254);
check_member(dramc_ao_regs, shu[2].dqsg_retry, 0x1854);
check_member(dramc_ao_regs, shu[3].dqsg_retry, 0x1e54);

struct dramc_ddrphy_regs_misc_stberr_rk {
	uint32_t r;
	uint32_t f;
};

struct dramc_ddrphy_regs_shu_rk {
	struct {
		uint32_t dq[8];
		uint32_t rsvd_20[12];
	} b[2];
	uint32_t ca_cmd[10];
	uint32_t rsvd_22[14];
};

struct dramc_ddrphy_ao_regs {
	uint32_t pll1;
	uint32_t pll2;
	uint32_t pll3;
	uint32_t pll4;
	uint32_t pll5;
	uint32_t pll6;
	uint32_t pll7;
	uint32_t pll8;
	uint32_t pll9;
	uint32_t pll10;
	uint32_t pll11;
	uint32_t pll12;
	uint32_t pll13;
	uint32_t pll14;
	uint32_t pll15;
	uint32_t pll16;
	uint32_t reserved0[16];
	struct {
		uint32_t dll_fine_tune[6];
		uint32_t dq[10];
		uint32_t _rsvd_0[4];
		uint32_t tx_mck;
		uint32_t _rsvd_1[11];
	} b[2];
	uint32_t ca_dll_fine_tune[6];
	uint32_t ca_cmd[11];
	uint32_t rfu_0x1c4;
	uint32_t rfu_0x1c8;
	uint32_t rfu_0x1cc;
	uint32_t ca_tx_mck;
	uint32_t reserved3[11];
	uint32_t misc_extlb[24];
	uint32_t dvfs_emi_clk;
	uint32_t misc_vref_ctrl;
	uint32_t misc_imp_ctrl0;
	uint32_t misc_imp_ctrl1;
	uint32_t misc_shu_opt;
	uint32_t misc_spm_ctrl0;
	uint32_t misc_spm_ctrl1;
	uint32_t misc_spm_ctrl2;
	uint32_t misc_spm_ctrl3;
	uint32_t misc_cg_ctrl0;
	uint32_t misc_cg_ctrl1;
	uint32_t misc_cg_ctrl2;
	uint32_t misc_cg_ctrl3;
	uint32_t misc_cg_ctrl4;
	uint32_t misc_cg_ctrl5;
	uint32_t misc_ctrl0;
	uint32_t misc_ctrl1;
	uint32_t misc_ctrl2;
	uint32_t misc_ctrl3;
	uint32_t misc_ctrl4;
	uint32_t misc_ctrl5;
	uint32_t misc_extlb_rx[21];
	uint32_t ckmux_sel;
	uint32_t reserved4[129];
	uint32_t misc_stberr_rk0_r;
	uint32_t misc_stberr_rk0_f;
	uint32_t misc_stberr_rk1_r;
	uint32_t misc_stberr_rk1_f;
	uint32_t misc_stberr_rk2_r;
	uint32_t misc_stberr_rk2_f;
	uint32_t reserved5[46];
	uint32_t misc_rxdvs[3];
	uint32_t rfu_0x5ec;
	uint32_t b0_rxdvs[2];
	uint32_t rfu_0x5f8;
	uint32_t rfu_0x5fc;
	union {
		struct {
			struct {
				uint32_t rxdvs[8];
				uint32_t _rsvd[24];
			} b[2];
			uint32_t rxdvs[10];
			uint32_t _rsvd_b[54];
		} r[3];
		struct {
			uint32_t rsvd_2[28];
			uint32_t b1_rxdvs[2];
			uint32_t rsvd_3[30];
			uint32_t ca_rxdvs0;
			uint32_t ca_rxdvs1;
			uint32_t rsvd_4[2];
			uint32_t r0_ca_rxdvs[10];
		};
	};

	struct ddrphy_ao_shu {
		struct {
			uint32_t dq[13];
			uint32_t dll[2];
			uint32_t rsvd_16[17];
		} b[2];
		uint32_t ca_cmd[13];
		uint32_t ca_dll[2];
		uint32_t rsvd_18[17];
		uint32_t pll[16];
		uint32_t rsvd_19[4];
		uint32_t pll20;
		uint32_t pll21;
		uint32_t rsvd_20[6];
		uint32_t misc0;
		uint32_t rsvd_21[3];
		struct dramc_ddrphy_regs_shu_rk rk[3];
	} shu[4];
};

check_member(dramc_ddrphy_ao_regs, pll1, 0x0000);
check_member(dramc_ddrphy_ao_regs, b[0].dll_fine_tune[0], 0x0080);
check_member(dramc_ddrphy_ao_regs, b[0].dq[0], 0x0098);
check_member(dramc_ddrphy_ao_regs, b[0].tx_mck, 0x00d0);
check_member(dramc_ddrphy_ao_regs, ca_dll_fine_tune[0], 0x0180);
check_member(dramc_ddrphy_ao_regs, ca_cmd[0], 0x0198);
check_member(dramc_ddrphy_ao_regs, ca_tx_mck, 0x01d0);
check_member(dramc_ddrphy_ao_regs, misc_extlb[0], 0x0200);
check_member(dramc_ddrphy_ao_regs, dvfs_emi_clk, 0x0260);
check_member(dramc_ddrphy_ao_regs, misc_vref_ctrl, 0x0264);
check_member(dramc_ddrphy_ao_regs, misc_imp_ctrl0, 0x0268);
check_member(dramc_ddrphy_ao_regs, misc_imp_ctrl1, 0x026c);
check_member(dramc_ddrphy_ao_regs, misc_shu_opt, 0x0270);
check_member(dramc_ddrphy_ao_regs, misc_spm_ctrl0, 0x0274);
check_member(dramc_ddrphy_ao_regs, misc_spm_ctrl1, 0x0278);
check_member(dramc_ddrphy_ao_regs, misc_spm_ctrl2, 0x027c);
check_member(dramc_ddrphy_ao_regs, misc_spm_ctrl3, 0x0280);
check_member(dramc_ddrphy_ao_regs, misc_cg_ctrl0, 0x0284);
check_member(dramc_ddrphy_ao_regs, misc_cg_ctrl1, 0x0288);
check_member(dramc_ddrphy_ao_regs, misc_cg_ctrl2, 0x028c);
check_member(dramc_ddrphy_ao_regs, misc_cg_ctrl3, 0x0290);
check_member(dramc_ddrphy_ao_regs, misc_cg_ctrl4, 0x0294);
check_member(dramc_ddrphy_ao_regs, misc_cg_ctrl5, 0x0298);
check_member(dramc_ddrphy_ao_regs, misc_ctrl0, 0x029c);
check_member(dramc_ddrphy_ao_regs, misc_ctrl1, 0x02a0);
check_member(dramc_ddrphy_ao_regs, misc_ctrl2, 0x02a4);
check_member(dramc_ddrphy_ao_regs, misc_ctrl3, 0x02a8);
check_member(dramc_ddrphy_ao_regs, misc_ctrl4, 0x02ac);
check_member(dramc_ddrphy_ao_regs, misc_ctrl5, 0x02b0);
check_member(dramc_ddrphy_ao_regs, misc_extlb_rx[0], 0x02b4);
check_member(dramc_ddrphy_ao_regs, ckmux_sel, 0x0308);
check_member(dramc_ddrphy_ao_regs, misc_rxdvs[0], 0x05e0);
check_member(dramc_ddrphy_ao_regs, misc_rxdvs[1], 0x05e4);
check_member(dramc_ddrphy_ao_regs, misc_rxdvs[2], 0x05e8);
check_member(dramc_ddrphy_ao_regs, rfu_0x5ec, 0x05ec);
check_member(dramc_ddrphy_ao_regs, b0_rxdvs[0], 0x05f0);
check_member(dramc_ddrphy_ao_regs, r[0].b[0].rxdvs[0], 0x0600);
check_member(dramc_ddrphy_ao_regs, b1_rxdvs[0], 0x0670);
check_member(dramc_ddrphy_ao_regs, r[0].b[1].rxdvs[0], 0x0680);
check_member(dramc_ddrphy_ao_regs, ca_rxdvs0, 0x06F0);
check_member(dramc_ddrphy_ao_regs, r0_ca_rxdvs[0], 0x0700);
check_member(dramc_ddrphy_ao_regs, r[1].b[1].rxdvs[0], 0x0880);
check_member(dramc_ddrphy_ao_regs, r[1].rxdvs[0], 0x0900);
check_member(dramc_ddrphy_ao_regs, shu[0].b[0].dq[0], 0x0c00);
check_member(dramc_ddrphy_ao_regs, shu[0].b[1].dq[6], 0x0C98);
check_member(dramc_ddrphy_ao_regs, shu[0].ca_cmd[0], 0x0d00);
check_member(dramc_ddrphy_ao_regs, shu[0].ca_dll[0], 0x0d34);
check_member(dramc_ddrphy_ao_regs, shu[0].pll[0], 0x0d80);
check_member(dramc_ddrphy_ao_regs, shu[0].misc0, 0x0DF0);
check_member(dramc_ddrphy_ao_regs, shu[0].rk[0].b[0].dq[0], 0x0e00);
check_member(dramc_ddrphy_ao_regs, shu[0].rk[0].ca_cmd[9], 0x0ec4);
check_member(dramc_ddrphy_ao_regs, shu[0].rk[1].b[0].dq[0], 0x0f00);
check_member(dramc_ddrphy_ao_regs, shu[0].rk[1].ca_cmd[9], 0x0fc4);
check_member(dramc_ddrphy_ao_regs, shu[0].rk[2].b[0].dq[0], 0x1000);
check_member(dramc_ddrphy_ao_regs, shu[0].rk[2].ca_cmd[9], 0x10c4);
check_member(dramc_ddrphy_ao_regs, shu[2].b[0].dq[0], 0x1600);
check_member(dramc_ddrphy_ao_regs, shu[2].b[1].dq[0], 0x1680);
check_member(dramc_ddrphy_ao_regs, shu[2].ca_cmd[0], 0x1700);
check_member(dramc_ddrphy_ao_regs, shu[2].ca_dll[0], 0x1734);
check_member(dramc_ddrphy_ao_regs, shu[2].pll[0], 0x1780);
check_member(dramc_ddrphy_ao_regs, shu[2].misc0, 0x17F0);
check_member(dramc_ddrphy_ao_regs, shu[2].rk[0].b[0].dq[0], 0x1800);
check_member(dramc_ddrphy_ao_regs, shu[2].rk[0].ca_cmd[0], 0x18A0);
check_member(dramc_ddrphy_ao_regs, shu[2].rk[1].b[0].dq[0], 0x1900);
check_member(dramc_ddrphy_ao_regs, shu[2].rk[1].ca_cmd[0], 0x19A0);
check_member(dramc_ddrphy_ao_regs, shu[2].rk[2].b[0].dq[0], 0x1A00);
check_member(dramc_ddrphy_ao_regs, shu[2].rk[2].ca_cmd[0], 0x1AA0);
check_member(dramc_ddrphy_ao_regs, shu[3].ca_cmd[0], 0x1C00);
check_member(dramc_ddrphy_ao_regs, shu[3].pll[0], 0x1C80);
check_member(dramc_ddrphy_ao_regs, shu[3].pll20, 0x1CD0);
check_member(dramc_ddrphy_ao_regs, shu[3].misc0, 0x1CF0);
check_member(dramc_ddrphy_ao_regs, shu[3].rk[0].ca_cmd[9], 0x1DC4);
check_member(dramc_ddrphy_ao_regs, shu[3].rk[1].ca_cmd[9], 0x1EC4);
check_member(dramc_ddrphy_ao_regs, shu[3].rk[2].ca_cmd[9], 0x1FC4);

struct dramc_ddrphy_nao_regs {
	uint32_t misc_sta_extlb[3];
	uint32_t reserved0[29];
	uint32_t misc_dq_rxdly_trro[32];
	uint32_t misc_ca_rxdly_trro[32];
	uint32_t misc_dqo1;
	uint32_t misc_cao1;
	uint32_t misc_ad_rx_dq_o1;
	uint32_t misc_ad_rx_cmd_o1;
	uint32_t misc_phy_rgs_dq;
	uint32_t misc_phy_rgs_cmd;
	uint32_t misc_phy_stben_b[2];
	uint32_t misc_phy_rgs_stben_cmd;
	uint32_t dramc_ddrphy_nao_regs_end;
};

check_member(dramc_ddrphy_nao_regs, misc_sta_extlb[0], 0x0);
check_member(dramc_ddrphy_nao_regs, misc_dq_rxdly_trro[0], 0x080);
check_member(dramc_ddrphy_nao_regs, misc_dqo1, 0x0180);
check_member(dramc_ddrphy_nao_regs, misc_cao1, 0x0184);
check_member(dramc_ddrphy_nao_regs, misc_phy_rgs_dq, 0x0190);
check_member(dramc_ddrphy_nao_regs, misc_phy_rgs_cmd, 0x0194);
check_member(dramc_ddrphy_nao_regs, misc_phy_stben_b[0], 0x0198);
check_member(dramc_ddrphy_nao_regs, misc_phy_rgs_stben_cmd, 0x01A0);

struct emi_regs {
	uint32_t cona;
	uint32_t reserved0[1];
	uint32_t conb;
	uint32_t reserved1[1];
	uint32_t conc;
	uint32_t reserved2[1];
	uint32_t cond;
	uint32_t reserved3[1];
	uint32_t cone;
	uint32_t reserved4[1];
	uint32_t conf;
	uint32_t reserved5[1];
	uint32_t cong;
	uint32_t reserved6[1];
	uint32_t conh;
	uint32_t conh_2nd;
	uint32_t coni;
	uint32_t reserved7[1];
	uint32_t conj;
	uint32_t reserved8[5];
	uint32_t conm;
	uint32_t reserved9[1];
	uint32_t conn;
	uint32_t reserved10[1];
	uint32_t cono;
	uint32_t reserved11[1];
	uint32_t mdct;
	uint32_t mdct_2nd;
	uint32_t reserved12[20];
	uint32_t iocl;
	uint32_t iocl_2nd;
	uint32_t iocm;
	uint32_t iocm_2nd;
	uint32_t reserved13[2];
	uint32_t testb;
	uint32_t reserved14[1];
	uint32_t testc;
	uint32_t reserved15[1];
	uint32_t testd;
	uint32_t reserved16[1];
	uint32_t arba;
	uint32_t reserved17[1];
	uint32_t arbb;
	uint32_t reserved18[1];
	uint32_t arbc;
	uint32_t reserved19[1];
	uint32_t arbd;
	uint32_t reserved20[1];
	uint32_t arbe;
	uint32_t reserved21[1];
	uint32_t arbf;
	uint32_t reserved22[1];
	uint32_t arbg;
	uint32_t reserved23[1];
	uint32_t arbh;
	uint32_t reserved24[1];
	uint32_t arbi;
	uint32_t arbi_2nd;
	uint32_t reserved25[2];
	uint32_t arbk;
	uint32_t arbk_2nd;
	uint32_t slct;
	uint32_t reserved26[277];
	uint32_t bwct0;
	uint32_t reserved27[59];
	uint32_t bwct0_2nd;
	uint32_t reserved28[43];
	uint32_t ltct0_2nd;
	uint32_t ltct1_2nd;
	uint32_t ltct2_2nd;
	uint32_t ltct3_2nd;
	uint32_t reserved29[4];
	uint32_t bwct0_3rd;
	uint32_t reserved30[3];
	uint32_t bwct0_4th;
	uint32_t reserved31[11];
	uint32_t bwct0_5th;
	uint32_t reserved32[19];
	uint32_t slva;
};

check_member(emi_regs, cona, 0x0000);
check_member(emi_regs, conb, 0x0008);
check_member(emi_regs, conc, 0x0010);
check_member(emi_regs, cond, 0x0018);
check_member(emi_regs, cone, 0x0020);
check_member(emi_regs, conf, 0x0028);
check_member(emi_regs, cong, 0x0030);
check_member(emi_regs, conh, 0x0038);
check_member(emi_regs, conh_2nd, 0x003c);
check_member(emi_regs, coni, 0x0040);
check_member(emi_regs, conj, 0x0048);
check_member(emi_regs, conm, 0x0060);
check_member(emi_regs, conn, 0x0068);
check_member(emi_regs, cono, 0x0070);
check_member(emi_regs, mdct, 0x0078);
check_member(emi_regs, mdct_2nd, 0x007c);
check_member(emi_regs, iocl, 0x00d0);
check_member(emi_regs, iocl_2nd, 0x00d4);
check_member(emi_regs, iocm, 0x00d8);
check_member(emi_regs, iocm_2nd, 0x00dc);
check_member(emi_regs, testb, 0x00e8);
check_member(emi_regs, testc, 0x00f0);
check_member(emi_regs, testd, 0x00f8);
check_member(emi_regs, arba, 0x0100);
check_member(emi_regs, arbb, 0x0108);
check_member(emi_regs, arbc, 0x0110);
check_member(emi_regs, arbd, 0x0118);
check_member(emi_regs, arbe, 0x0120);
check_member(emi_regs, arbf, 0x0128);
check_member(emi_regs, arbg, 0x0130);
check_member(emi_regs, arbh, 0x0138);
check_member(emi_regs, arbi, 0x0140);
check_member(emi_regs, arbi_2nd, 0x0144);
check_member(emi_regs, slct, 0x0158);
check_member(emi_regs, bwct0, 0x05B0);
check_member(emi_regs, bwct0_2nd, 0x06A0);
check_member(emi_regs, bwct0_3rd, 0x0770);
check_member(emi_regs, bwct0_4th, 0x0780);
check_member(emi_regs, bwct0_5th, 0x07B0);

struct chn_emi_regs {
	uint32_t chn_cona;
	uint32_t rsvd_1[1];
	uint32_t chn_conb;
	uint32_t rsvd_2[1];
	uint32_t chn_conc;
	uint32_t rsvd_3[1];
	uint32_t chn_mdct;
	uint32_t rsvd_4[11];
	uint32_t chn_testb;
	uint32_t rsvd_5[1];
	uint32_t chn_testc;
	uint32_t rsvd_6[1];
	uint32_t chn_testd;
	uint32_t rsvd_7[9];
	uint32_t chn_md_pre_mask;
	uint32_t rsvd_8[1];
	uint32_t chn_md_pre_mask_shf;
	uint32_t rsvd_9[45];
	uint32_t chn_arbi;
	uint32_t chn_arbi_2nd;
	uint32_t chn_arbj;
	uint32_t chn_arbj_2nd;
	uint32_t chn_arbk;
	uint32_t chn_arbk_2nd;
	uint32_t chn_slct;
	uint32_t chn_arb_ref;
	uint32_t rsvd_10[20];
	uint32_t chn_rkarb0;
	uint32_t chn_rkarb1;
	uint32_t chn_rkarb2;
	uint32_t rsvd_11[144];
	uint32_t chn_eco3;
	uint32_t rsvd_12[196];
	uint32_t chn_emi_shf0;
	uint32_t chn_emi_regs_end;

};

check_member(chn_emi_regs, chn_cona, 0x0000);
check_member(chn_emi_regs, chn_conb, 0x0008);
check_member(chn_emi_regs, chn_conc, 0x0010);
check_member(chn_emi_regs, chn_mdct, 0x0018);
check_member(chn_emi_regs, chn_testb, 0x0048);
check_member(chn_emi_regs, chn_testc, 0x0050);
check_member(chn_emi_regs, chn_testd, 0x0058);
check_member(chn_emi_regs, chn_md_pre_mask, 0x0080);
check_member(chn_emi_regs, chn_md_pre_mask_shf, 0x0088);
check_member(chn_emi_regs, chn_arbi, 0x0140);
check_member(chn_emi_regs, chn_arbi_2nd, 0x0144);
check_member(chn_emi_regs, chn_arbj, 0x0148);
check_member(chn_emi_regs, chn_arbj_2nd, 0x014c);
check_member(chn_emi_regs, chn_arbk, 0x0150);
check_member(chn_emi_regs, chn_arbk_2nd, 0x0154);
check_member(chn_emi_regs, chn_slct, 0x0158);
check_member(chn_emi_regs, chn_arb_ref, 0x015c);
check_member(chn_emi_regs, chn_rkarb0, 0x01b0);
check_member(chn_emi_regs, chn_rkarb1, 0x01b4);
check_member(chn_emi_regs, chn_rkarb2, 0x01b8);
check_member(chn_emi_regs, chn_eco3, 0x03fc);
check_member(chn_emi_regs, chn_emi_shf0, 0x0710);

struct emi_mpu_regs {
	uint32_t mpu_ctrl;
	uint32_t rsvd[511];
	uint32_t mpu_ctrl_d[16];
};

check_member(emi_mpu_regs, mpu_ctrl, 0x0000);
check_member(emi_mpu_regs, mpu_ctrl_d[0], 0x0800);

enum {
	TESTCHIP_DMA1_DMA_LP4MATAB_OPT_SHIFT = 12,
};

enum {
	MISC_STATUSA_REFRESH_QUEUE_CNT_SHIFT = 24,
	MISC_STATUSA_REFRESH_QUEUE_CNT_MASK = 0x0f000000,
};

enum {
	SPCMDRESP_RDDQC_RESPONSE_SHIFT = 7,
};

enum {
	DDRCONF0_DM4TO1MODE_SHIFT = 22,
	DDRCONF0_RDATRST_SHIFT = 0,
};

enum {
	PERFCTL0_RWOFOEN_SHIFT = 4,
};

enum {
	PADCTRL_DQIENLATEBEGIN_SHIFT = 3,
	PADCTRL_DQIENQKEND_SHIFT = 0,
	PADCTRL_DQIENQKEND_MASK = 0x00000003,
};

enum {
	REFCTRL0_REFDIS_SHIFT = 29,
	REFCTRL0_PBREFEN_SHIFT = 18,
};

enum {
	MRS_MRSRK_SHIFT = 24,
	MRS_MRSRK_MASK = 0x03000000,
	MRS_MRSMA_SHIFT = 8,
	MRS_MRSMA_MASK = 0x001fff00,
	MRS_MRSOP_SHIFT = 0,
	MRS_MRSOP_MASK = 0x000000ff,
};

enum {
	SPCMD_DQSGCNTRST_SHIFT = 9,
	SPCMD_DQSGCNTEN_SHIFT = 8,
	SPCMD_RDDQCEN_SHIFT = 7,
	SPCMD_MRWEN_SHIFT = 0,
};

enum {
	SPCMDCTRL_RDDQCDIS_SHIFT = 11,
};

enum {
	MPC_OPTION_MPCRKEN_SHIFT = 17,
};

enum {
	TEST2_0_PAT0_SHIFT = 8,
	TEST2_0_PAT0_MASK = 0x0000ff00,
	TEST2_0_PAT1_SHIFT = 0,
	TEST2_0_PAT1_MASK = 0x000000ff,
};

enum {
	TEST2_3_TEST2W_SHIFT = 31,
	TEST2_3_TEST2R_SHIFT = 30,
	TEST2_3_TEST1_SHIFT = 29,
	TEST2_3_TESTAUDPAT_SHIFT = 7,
	TEST2_3_TESTCNT_SHIFT = 0,
	TEST2_3_TESTCNT_MASK = 0x0000000f,
};

enum {
	TEST2_4_TESTAGENTRKSEL_MASK = 0x70000000,
	TEST2_4_TESTAGENTRK_SHIFT = 24,
	TEST2_4_TESTAGENTRK_MASK = 0x03000000,
	TEST2_4_TEST_REQ_LEN1_SHIFT = 17,
	TEST2_4_TESTXTALKPAT_SHIFT = 16,
	TEST2_4_TESTAUDMODE_SHIFT = 15,
	TEST2_4_TESTAUDBITINV_SHIFT = 14,
	TEST2_4_TESTAUDINIT_SHIFT = 8,
	TEST2_4_TESTAUDINIT_MASK = 0x00001f00,
	TEST2_4_TESTSSOXTALKPAT_SHIFT = 7,
	TEST2_4_TESTSSOPAT_SHIFT = 6,
	TEST2_4_TESTAUDINC_SHIFT = 0,
	TEST2_4_TESTAUDINC_MASK = 0x0000001f,
};

enum {
	MR_GOLDEN_MR15_GOLDEN_MASK = 0x0000ff00,
	MR_GOLDEN_MR20_GOLDEN_MASK = 0x000000ff,
};

enum {
	DQSOSCR_AR_COARSE_TUNE_DQ_SW_SHIFT = 7,
};

enum {
	DUMMY_RD_DQSG_DMYWR_EN_SHIFT = 23,
	DUMMY_RD_DQSG_DMYRD_EN_SHIFT = 22,
	DUMMY_RD_SREF_DMYRD_EN_SHIFT = 21,
	DUMMY_RD_DUMMY_RD_EN_SHIFT = 20,
	DUMMY_RD_DMY_RD_DBG_SHIFT = 7,
	DUMMY_RD_DMY_WR_DBG_SHIFT = 6,
};

enum {
	STBCAL1_STBCNT_LATCH_EN_SHIFT = 11,
	STBCAL1_STBENCMPEN_SHIFT = 10,
};

enum {
	SHU_ACTIM_XRT_XRTR2R_SHIFT = 0,
	SHU_ACTIM_XRT_XRTR2R_MASK = 0x0000000f,
};

enum {
	SHU_CONF1_DATLAT_DSEL_PHY_SHIFT = 26,
	SHU_CONF1_DATLAT_DSEL_PHY_MASK = 0x7c000000,
	SHU_CONF1_DATLAT_DSEL_SHIFT = 8,
	SHU_CONF1_DATLAT_DSEL_MASK = 0x00001f00,
	SHU_CONF1_DATLAT_SHIFT = 0,
	SHU_CONF1_DATLAT_MASK = 0x0000001f,
};

enum {
	SHU_RANKCTL_RANKINCTL_PHY_SHIFT = 28,
	SHU_RANKCTL_RANKINCTL_PHY_MASK = 0xf0000000,
	SHU_RANKCTL_RANKINCTL_ROOT1_SHIFT = 24,
	SHU_RANKCTL_RANKINCTL_ROOT1_MASK = 0x0f000000,
	SHU_RANKCTL_RANKINCTL_SHIFT = 20,
	SHU_RANKCTL_RANKINCTL_MASK = 0x00f00000,
};

enum {
	SHU1_WODT_DBIWR_SHIFT = 29,
};

enum {
	SHURK_DQSCTL_DQSINCTL_SHIFT = 0,
	SHURK_DQSCTL_DQSINCTL_MASK = 0x0000000f,
};

enum {
	SHURK_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1_SHIFT = 12,
	SHURK_SELPH_ODTEN0_TXDLY_B1_RODTEN_SHIFT = 8,
	SHURK_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1_SHIFT = 4,
	SHURK_SELPH_ODTEN0_TXDLY_B0_RODTEN_SHIFT = 0,
};

enum {
	SHURK_SELPH_ODTEN1_DLY_B1_RODTEN_P1_SHIFT = 12,
	SHURK_SELPH_ODTEN1_DLY_B1_RODTEN_SHIFT = 8,
	SHURK_SELPH_ODTEN1_DLY_B0_RODTEN_P1_SHIFT = 4,
	SHURK_SELPH_ODTEN1_DLY_B0_RODTEN_SHIFT = 0,
};

enum {
	SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1_SHIFT = 12,
	SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_SHIFT = 8,
	SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_SHIFT = 4,
	SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_MASK = 0x00000070,
	SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_SHIFT = 0,
	SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_MASK = 0x00000007,
};

enum {
	SHURK_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1_SHIFT = 12,
	SHURK_SELPH_DQSG1_REG_DLY_DQS1_GATED_SHIFT = 8,
	SHURK_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1_SHIFT = 4,
	SHURK_SELPH_DQSG1_REG_DLY_DQS0_GATED_SHIFT = 0,
};

enum {
	B0_DQ5_RG_RX_ARDQ_VREF_EN_B0_SHIFT = 16,
};

enum {
	B1_DQ5_RG_RX_ARDQ_VREF_EN_B1_SHIFT = 16,
};

enum {
	MISC_CTRL1_R_DMSTBENCMP_RK_OPT_SHIFT = 25,
	MISC_CTRL1_R_DMAR_FINE_TUNE_DQ_SW_SHIFT = 7,
	MISC_CTRL1_R_DMPHYRST_SHIFT = 1,
};

enum {
	MISC_STBERR_RK_R_STBERR_RK_R_MASK = 0x0000ffff,
};

enum {
	MISC_STBERR_RK_F_STBERR_RK_F_MASK = 0x0000ffff,
};

enum {
	SHU1_BX_DQ5_RG_RX_ARDQ_VREF_SEL_B0_SHIFT = 0,
	SHU1_BX_DQ5_RG_RX_ARDQ_VREF_SEL_B0_MASK = 0x0000003f,
};

enum {
	SHU1_BX_DQ7_R_DMDQMDBI_SHIFT = 7,
	SHU1_BX_DQ7_R_DMRANKRXDVS_SHIFT = 0,
	SHU1_BX_DQ7_R_DMRANKRXDVS_MASK = 0x0000000f,
};

enum {
	SHU1_CA_CMD0_RG_TX_ARCLK_DRVN_PRE_SHIFT = 12,
	SHU1_CA_CMD0_RG_TX_ARCLK_DRVN_PRE_MASK = 0x00007000,
};

enum {
	SHU1_CA_CMD3_RG_TX_ARCMD_PU_PRE_SHIFT = 10,
	SHU1_CA_CMD3_RG_TX_ARCMD_PU_PRE_MASK = 0x00000c00,
};

enum {
	SHU1_B0_DQ6_RK_RX_ARDQS0_F_DLY_B0_MASK = 0x7f000000,
	SHU1_B0_DQ6_RK_RX_ARDQS0_R_DLY_B0_MASK = 0x007f0000,
	SHU1_B0_DQ6_RK_RX_ARDQM0_F_DLY_B0_MASK = 0x00003f00,
	SHU1_B0_DQ6_RK_RX_ARDQM0_R_DLY_B0_MASK = 0x0000003f,
};

enum {
	FINE_TUNE_PBYTE_SHIFT = 24,
	FINE_TUNE_PBYTE_MASK = 0x3f000000,
	FINE_TUNE_DQM_SHIFT = 16,
	FINE_TUNE_DQM_MASK = 0x003f0000,
	FINE_TUNE_DQ_SHIFT = 8,
	FINE_TUNE_DQ_MASK = 0x00003f00,
};

enum {
	SHU1_CA_CMD9_RG_RK_ARFINE_TUNE_CLK_SHIFT = 24,
	SHU1_CA_CMD9_RG_RK_ARFINE_TUNE_CLK_MASK = 0x3f000000,
	SHU1_CA_CMD9_RG_RK_ARFINE_TUNE_CS_MASK = 0x0000003f,
};

struct dramc_channel_regs {
	union {
		struct dramc_ddrphy_ao_regs phy;
		uint8_t raw_ddrphy_ao_regs[0x2000];
	};

	union {
		struct dramc_ao_regs ao;
		uint8_t raw_ao_regs[0x2000];
	};

	union {
		struct dramc_nao_regs nao;
		uint8_t raw_nao_regs[0x1000];
	};

	union {
		struct chn_emi_regs emi;
		uint8_t raw_emi_regs[0x1000];
	};

	union {
		struct dramc_ddrphy_nao_regs phy_nao;
		uint8_t raw_ddrphy_nao_regs[0x2000];
	};
};

static struct dramc_channel_regs *const ch = (void *)DRAMC_CH_BASE;
static struct emi_mpu_regs *const emi_mpu = (void *)EMI_MPU_BASE;

#endif /* _DRAMC_REGISTER_H_ */
