/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRAMC_REGISTER_H_
#define _DRAMC_REGISTER_H_

#include <device/mmio.h>
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
	struct dramc_ao_regs_shu {
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
check_member(chn_emi_regs, chn_emi_shf0, 0x0710);

struct emi_mpu_regs {
	uint32_t mpu_ctrl;
	uint32_t rsvd[511];
	uint32_t mpu_ctrl_d[16];
};

check_member(emi_mpu_regs, mpu_ctrl, 0x0000);
check_member(emi_mpu_regs, mpu_ctrl_d[0], 0x0800);

DEFINE_BITFIELD(MISC_STATUSA_REFRESH_QUEUE_CNT, 27, 24)
DEFINE_BIT(SPCMDRESP_DQSOSCEN_RESPONSE, 10)
DEFINE_BIT(SPCMDRESP_MRR_RESPONSE, 1)
DEFINE_BIT(SPCMDRESP_MRW_RESPONSE, 0)
DEFINE_BITFIELD(MRR_STATUS_MRR_REG, 15, 0)

DEFINE_BIT(DDRCONF0_DM4TO1MODE, 22)
DEFINE_BIT(DDRCONF0_RDATRST, 0)
DEFINE_BIT(PERFCTL0_RWOFOEN, 4)

DEFINE_BITFIELD(RKCFG_TXRANK, 1, 0)
DEFINE_BIT(RKCFG_TXRANKFIX, 3)
DEFINE_BIT(RKCFG_DQSOSC2RK, 11)

DEFINE_BIT(PADCTRL_DQIENLATEBEGIN, 3)
DEFINE_BITFIELD(PADCTRL_DQIENQKEND, 1, 0)

DEFINE_BITFIELD(SHURK_DQSIEN_DQS0IEN, 6, 0)
DEFINE_BITFIELD(SHURK_DQSIEN_DQS1IEN, 14, 8)
DEFINE_BITFIELD(SHURK_DQSIEN_DQS2IEN, 22, 16)
DEFINE_BITFIELD(SHURK_DQSIEN_DQS3IEN, 30, 24)

DEFINE_BIT(REFCTRL0_REFDIS, 29)
DEFINE_BIT(REFCTRL0_PBREFEN, 18)

DEFINE_BIT(CKECTRL_CKEFIXON, 6)
DEFINE_BIT(CKECTRL_CKEFIXOFF, 7)

DEFINE_BITFIELD(MRS_MRSBG, 31, 30)
DEFINE_BITFIELD(MRS_MPCRK, 29, 28)
DEFINE_BITFIELD(MRS_MRRRK, 27, 26)
DEFINE_BITFIELD(MRS_MRSRK, 25, 24)
DEFINE_BITFIELD(MRS_MRSBA, 23, 21)
DEFINE_BITFIELD(MRS_MRSMA, 20, 8)
DEFINE_BITFIELD(MRS_MRSOP, 7, 0)

DEFINE_BIT(SPCMD_DQSOSCENEN, 10)
DEFINE_BIT(SPCMD_DQSGCNTRST, 9)
DEFINE_BIT(SPCMD_DQSGCNTEN, 8)
DEFINE_BIT(SPCMD_ZQLATEN, 6)
DEFINE_BIT(SPCMD_RDDQCEN, 7)
DEFINE_BIT(SPCMD_ZQCEN, 4)
DEFINE_BIT(SPCMD_MRREN, 1)
DEFINE_BIT(SPCMD_MRWEN, 0)

DEFINE_BIT(SPCMDCTRL_RDDQCDIS, 11)

DEFINE_BIT(MPC_OPTION_MPCRKEN, 17)
DEFINE_BIT(MPC_OPTION_MPC_BLOCKALE_OPT, 0)

DEFINE_BIT(DVFSDLL_R_BYPASS_1ST_DLL_SHU1, 1)

DEFINE_BITFIELD(MISC_CG_CTRL0_CLK_MEM_SEL, 5, 4)

DEFINE_BIT(DRAMCTRL_ADRDECEN_TARKMODE, 1)

DEFINE_BIT(TESTCHIP_DMA1_DMA_LP4MATAB_OPT, 12)
DEFINE_BITFIELD(TEST2_0_PAT0, 15, 8)
DEFINE_BITFIELD(TEST2_0_PAT1, 7, 0)

DEFINE_BITFIELD(TEST2_1_TEST2_BASE, 31, 4)
DEFINE_BITFIELD(TEST2_2_TEST2_OFF, 31, 4)

DEFINE_BIT(TEST2_3_TEST2W, 31)
DEFINE_BIT(TEST2_3_TEST2R, 30)
DEFINE_BIT(TEST2_3_TEST1, 29)
DEFINE_BIT(TEST2_3_TESTAUDPAT, 7)
DEFINE_BITFIELD(TEST2_3_TESTCNT, 3, 0)

DEFINE_BITFIELD(TEST2_4_TESTAGENTRKSEL, 30, 28)
DEFINE_BITFIELD(TEST2_4_TESTAGENTRK, 25, 24)
DEFINE_BIT(TEST2_4_TEST_REQ_LEN1, 17)
DEFINE_BIT(TEST2_4_TESTXTALKPAT, 16)
DEFINE_BIT(TEST2_4_TESTAUDMODE, 15)
DEFINE_BIT(TEST2_4_TESTAUDBITINV, 14)
DEFINE_BITFIELD(TEST2_4_TESTAUDINIT, 12, 8)
DEFINE_BIT(TEST2_4_TESTSSOXTALKPAT, 7)
DEFINE_BIT(TEST2_4_TESTSSOPAT, 6)
DEFINE_BITFIELD(TEST2_4_TESTAUDINC, 4, 0)

DEFINE_BITFIELD(MR_GOLDEN_MR15_GOLDEN, 15, 8)
DEFINE_BITFIELD(MR_GOLDEN_MR20_GOLDEN, 7, 0)

DEFINE_BIT(DUMMY_RD_DQSG_DMYWR_EN, 23)
DEFINE_BIT(DUMMY_RD_DQSG_DMYRD_EN, 22)
DEFINE_BIT(DUMMY_RD_SREF_DMYRD_EN, 21)
DEFINE_BIT(DUMMY_RD_DUMMY_RD_EN, 20)
DEFINE_BIT(DUMMY_RD_DMY_RD_DBG, 7)
DEFINE_BIT(DUMMY_RD_DMY_WR_DBG, 6)

DEFINE_BIT(STBCAL1_STBCNT_LATCH_EN, 11)
DEFINE_BIT(STBCAL1_STBENCMPEN, 10)

DEFINE_BITFIELD(SHU_ACTIM_XRT_XRTR2R, 3, 0)

DEFINE_BITFIELD(SHU_CONF1_DATLAT_DSEL_PHY, 30, 26)
DEFINE_BITFIELD(SHU_CONF1_DATLAT_DSEL, 12, 8)
DEFINE_BITFIELD(SHU_CONF1_DATLAT, 4, 0)

DEFINE_BIT(SHU_PIPE_READ_START_EXTEND1, 31)
DEFINE_BIT(SHU_PIPE_DLE_LAST_EXTEND1, 30)
DEFINE_BIT(SHU_PIPE_READ_START_EXTEND2, 29)
DEFINE_BIT(SHU_PIPE_DLE_LAST_EXTEND2, 28)
DEFINE_BIT(SHU_PIPE_READ_START_EXTEND3, 27)
DEFINE_BIT(SHU_PIPE_DLE_LAST_EXTEND3, 26)

DEFINE_BITFIELD(SHU_RANKCTL_RANKINCTL_PHY, 31, 28)
DEFINE_BITFIELD(SHU_RANKCTL_RANKINCTL_ROOT1, 27, 24)
DEFINE_BITFIELD(SHU_RANKCTL_RANKINCTL, 23, 20)

DEFINE_BIT(SHU1_WODT_DBIWR, 29)
DEFINE_BIT(SHU_SCINTV_DQSOSCENDIS, 30)
DEFINE_BITFIELD(SHU_SCINTV_DQS2DQ_SHU_PITHRD, 23, 18)
DEFINE_BITFIELD(SHURK_DQSCTL_DQSINCTL, 3, 0)
DEFINE_BIT(RK0_DQSOSC_R_DMDQS2DQ_FILT_OPT, 29)
DEFINE_BIT(RK0_DQSOSC_DQSOSCR_RK0EN, 30)
DEFINE_BIT(RK1_DQSOSC_DQSOSCR_RK1EN, 30)

DEFINE_BITFIELD(RK2_DQSOSC_FREQ_RATIO_TX_0, 4, 0)
DEFINE_BITFIELD(RK2_DQSOSC_FREQ_RATIO_TX_1, 9, 5)
DEFINE_BITFIELD(RK2_DQSOSC_FREQ_RATIO_TX_3, 19, 15)
DEFINE_BITFIELD(RK2_DQSOSC_FREQ_RATIO_TX_4, 24, 20)
DEFINE_BITFIELD(RK2_DUMMY_RD_BK_FREQ_RATIO_TX_6, 7, 3)
DEFINE_BITFIELD(RK2_DUMMY_RD_BK_FREQ_RATIO_TX_7, 12, 8)

DEFINE_BIT(PRE_TDQSCK1_SW_UP_TX_NOW_CASE, 16)
DEFINE_BIT(PRE_TDQSCK1_SHU_PRELOAD_TX_START, 18)
DEFINE_BIT(PRE_TDQSCK1_SHU_PRELOAD_TX_HW, 19)

DEFINE_BITFIELD(SHU_SCINTV_DQS2DQ_FILT_PITHRD, 29, 24)
DEFINE_BITFIELD(SHU1_WODT_TXUPD_W2R_SEL, 16, 14)
DEFINE_BITFIELD(SHU1_WODT_TXUPD_SEL, 13, 12)
DEFINE_BITFIELD(SHU1_DQSOSC_PRD_DQSOSC_PRDCNT, 9, 0)
DEFINE_BITFIELD(SHU_DQSOSCR_DQSOSCRCNT, 7, 0)
DEFINE_BIT(DQSOSCR_ARUIDQ_SW, 7)
DEFINE_BIT(DQSOSCR_DQSOSCRDIS, 24)
DEFINE_BIT(DQSOSCR_DQSOSC_CALEN, 31)

DEFINE_BITFIELD(SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK0, 11, 0)
DEFINE_BITFIELD(SHU_DQSOSCTHRD_DQSOSCTHRD_DEC_RK0, 23, 12)
DEFINE_BITFIELD(SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK1_7TO0, 31, 24)
DEFINE_BITFIELD(SHU1_DQSOSC_PRD_DQSOSCTHRD_INC_RK1_11TO8, 19, 16)
DEFINE_BITFIELD(SHU1_DQSOSC_PRD_DQSOSCTHRD_DEC_RK1, 31, 20)
DEFINE_BITFIELD(SHU_DQSOSCR2_DQSOSCENCNT, 8, 0)

DEFINE_BITFIELD(SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1, 14, 12)
DEFINE_BITFIELD(SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1, 6, 4)
DEFINE_BITFIELD(SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED, 2, 0)

DEFINE_BITFIELD(SHURK_SELPH_DQSG1_TX_DLY_DQS1_GATED_P1, 14, 12)
DEFINE_BITFIELD(SHURK_SELPH_DQSG1_TX_DLY_DQS0_GATED_P1, 6, 4)
DEFINE_BITFIELD(SHURK_SELPH_DQSG1_TX_DLY_DQS0_GATED, 2, 0)
DEFINE_BITFIELD(SHU1RK0_DQSOSC_DQSOSC_BASE_RK0, 15, 0)
DEFINE_BITFIELD(SHU1RK0_DQSOSC_DQSOSC_BASE_RK0_B1, 31, 16)

DEFINE_BIT(B0_DQ5_RG_RX_ARDQ_VREF_EN_B0, 16)
DEFINE_BIT(B1_DQ5_RG_RX_ARDQ_VREF_EN_B1, 16)

DEFINE_BIT(MISC_CTRL1_R_DMSTBENCMP_RK, 25)
DEFINE_BIT(MISC_CTRL1_R_DMARPIDQ_SW, 7)
DEFINE_BIT(MISC_CTRL1_R_DMPHYRST, 1)

DEFINE_BITFIELD(MISC_STBERR_RK_R_STBERR_RK_R, 15, 0)
DEFINE_BITFIELD(MISC_STBERR_RK_F_STBERR_RK_F, 15, 0)

DEFINE_BITFIELD(SHU1_BX_DQ5_RG_RX_ARDQ_VREF_SEL_B0, 5, 0)

DEFINE_BIT(SHU1_B0_DQ7_R_DMDQMDBI_SHU_B0, 7)
DEFINE_BITFIELD(SHU1_B0_DQ7_R_DMRANKRXDVS, 3, 0)

DEFINE_BITFIELD(SHU1_CA_CMD0_RG_TX_ARCLK_DRVN_PRE, 14, 12)

DEFINE_BITFIELD(SHU1_CA_CMD3_RG_TX_ARCMD_PU_PRE, 11, 10)

DEFINE_BITFIELD(SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0, 30, 24)
DEFINE_BITFIELD(SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0, 22, 16)
DEFINE_BITFIELD(SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0, 13, 8)
DEFINE_BITFIELD(SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0, 5, 0)

DEFINE_BITFIELD(FINE_TUNE_PBYTE, 29, 24)
DEFINE_BITFIELD(FINE_TUNE_DQM, 21, 16)
DEFINE_BITFIELD(FINE_TUNE_DQ, 13, 8)

DEFINE_BITFIELD(SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK, 29, 24)
DEFINE_BITFIELD(SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD, 13, 8)
DEFINE_BITFIELD(SHU1_R1_CA_CMD9_RG_RK1_ARPI_CMD, 13, 8)
DEFINE_BITFIELD(SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK1) */
DEFINE_BIT(PRE_TDQSCK1_TDQSCK_HW_SW_UP_SEL, 22)
DEFINE_BIT(PRE_TDQSCK1_TDQSCK_REG_DVFS, 25)
DEFINE_BIT(PRE_TDQSCK1_TDQSCK_PRECAL_HW, 26)

/* DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK2) */
DEFINE_BITFIELD(PRE_TDQSCK2_TDDQSCK_JUMP_RATIO3, 7, 0)
DEFINE_BITFIELD(PRE_TDQSCK2_TDDQSCK_JUMP_RATIO2, 15, 8)
DEFINE_BITFIELD(PRE_TDQSCK2_TDDQSCK_JUMP_RATIO1, 23, 16)
DEFINE_BITFIELD(PRE_TDQSCK2_TDDQSCK_JUMP_RATIO0, 31, 24)

/* DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK3) */
DEFINE_BITFIELD(PRE_TDQSCK3_TDDQSCK_JUMP_RATIO7, 7, 0)
DEFINE_BITFIELD(PRE_TDQSCK3_TDDQSCK_JUMP_RATIO6, 15, 8)
DEFINE_BITFIELD(PRE_TDQSCK3_TDDQSCK_JUMP_RATIO5, 23, 16)
DEFINE_BITFIELD(PRE_TDQSCK3_TDDQSCK_JUMP_RATIO4, 31, 24)

/* DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK4) */
DEFINE_BITFIELD(PRE_TDQSCK4_TDDQSCK_JUMP_RATIO11, 7, 0)
DEFINE_BITFIELD(PRE_TDQSCK4_TDDQSCK_JUMP_RATIO10, 15, 8)
DEFINE_BITFIELD(PRE_TDQSCK4_TDDQSCK_JUMP_RATIO9, 23, 16)
DEFINE_BITFIELD(PRE_TDQSCK4_TDDQSCK_JUMP_RATIO8, 31, 24)

/* DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0) */
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1, 30, 28)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED, 26, 24)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1, 22, 20)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED, 18, 16)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1, 14, 12)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED, 10, 8)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1, 6, 4)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED, 2, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1) */
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1, 30, 28)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED, 26, 24)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1, 22, 20)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED, 18, 16)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1, 14, 12)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED, 10, 8)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1, 6, 4)
DEFINE_BITFIELD(SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED, 2, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R0, 25, 19)
DEFINE_BITFIELD(RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0, 18, 13)
DEFINE_BITFIELD(RK0_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN) */
DEFINE_BITFIELD(SHURK0_DQSIEN_R0DQS3IEN, 30, 24)
DEFINE_BITFIELD(SHURK0_DQSIEN_R0DQS2IEN, 22, 16)
DEFINE_BITFIELD(SHURK0_DQSIEN_R0DQS1IEN, 14, 8)
DEFINE_BITFIELD(SHURK0_DQSIEN_R0DQS0IEN, 6, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK3) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0, 17, 12)
DEFINE_BITFIELD(RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0, 11, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK2) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK4) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R0, 25, 19)
DEFINE_BITFIELD(RK0_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R0, 18, 13)
DEFINE_BITFIELD(RK0_PRE_TDQSCK4_TDQSCK_PIFREQ1_B1R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK6) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R0, 17, 12)
DEFINE_BITFIELD(RK0_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R0, 11, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK5) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK7) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK7_TDQSCK_PIFREQ2_B2R0, 25, 19)
DEFINE_BITFIELD(RK0_PRE_TDQSCK7_TDQSCK_UIFREQ2_B2R0, 18, 13)
DEFINE_BITFIELD(RK0_PRE_TDQSCK7_TDQSCK_PIFREQ1_B2R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK7_TDQSCK_UIFREQ1_B2R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK9) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK9_TDQSCK_UIFREQ3_P1_B2R0, 17, 12)
DEFINE_BITFIELD(RK0_PRE_TDQSCK9_TDQSCK_UIFREQ2_P1_B2R0, 11, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK9_TDQSCK_UIFREQ1_P1_B2R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK8) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK8_TDQSCK_PIFREQ3_B2R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK8_TDQSCK_UIFREQ3_B2R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK10) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK10_TDQSCK_PIFREQ2_B3R0, 25, 19)
DEFINE_BITFIELD(RK0_PRE_TDQSCK10_TDQSCK_UIFREQ2_B3R0, 18, 13)
DEFINE_BITFIELD(RK0_PRE_TDQSCK10_TDQSCK_PIFREQ1_B3R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK10_TDQSCK_UIFREQ1_B3R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK12) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK12_TDQSCK_UIFREQ3_P1_B3R0, 17, 12)
DEFINE_BITFIELD(RK0_PRE_TDQSCK12_TDQSCK_UIFREQ2_P1_B3R0, 11, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK12_TDQSCK_UIFREQ1_P1_B3R0, 5, 0)

/* DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK11) */
DEFINE_BITFIELD(RK0_PRE_TDQSCK11_TDQSCK_PIFREQ3_B3R0, 12, 6)
DEFINE_BITFIELD(RK0_PRE_TDQSCK11_TDQSCK_UIFREQ3_B3R0, 5, 0)

/* DRAMC_REG_MRR_BIT_MUX1 */
DEFINE_BITFIELD(MRR_BIT_MUX1_MRR_BIT3_SEL, 28, 24)
DEFINE_BITFIELD(MRR_BIT_MUX1_MRR_BIT2_SEL, 20, 16)
DEFINE_BITFIELD(MRR_BIT_MUX1_MRR_BIT1_SEL, 12, 8)
DEFINE_BITFIELD(MRR_BIT_MUX1_MRR_BIT0_SEL, 4, 0)

/* DRAMC_REG_SHU_SELPH_CA7 */
DEFINE_BITFIELD(SHU_SELPH_CA7_DLY_RA5, 22, 20)
DEFINE_BITFIELD(SHU_SELPH_CA7_DLY_RA4, 18, 16)
DEFINE_BITFIELD(SHU_SELPH_CA7_DLY_RA3, 14, 12)
DEFINE_BITFIELD(SHU_SELPH_CA7_DLY_RA2, 10, 8)
DEFINE_BITFIELD(SHU_SELPH_CA7_DLY_RA1, 6, 4)
DEFINE_BITFIELD(SHU_SELPH_CA7_DLY_RA0, 2, 0)

/* DRAMC_REG_MRR_BIT_MUX2 */
DEFINE_BITFIELD(MRR_BIT_MUX2_MRR_BIT5_SEL, 12, 8)
DEFINE_BITFIELD(MRR_BIT_MUX2_MRR_BIT4_SEL, 4, 0)

/* DDRPHY_SHU1_R0_CA_CMD0 */
DEFINE_BITFIELD(SHU1_R0_CA_CMD0_RK0_TX_ARCA5_DLY, 23, 20)
DEFINE_BITFIELD(SHU1_R0_CA_CMD0_RK0_TX_ARCA4_DLY, 19, 16)
DEFINE_BITFIELD(SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY, 15, 12)
DEFINE_BITFIELD(SHU1_R0_CA_CMD0_RK0_TX_ARCA2_DLY, 11, 8)
DEFINE_BITFIELD(SHU1_R0_CA_CMD0_RK0_TX_ARCA1_DLY, 7, 4)
DEFINE_BITFIELD(SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY, 3, 0)

/* DDRPHY_PLL2 */
DEFINE_BIT(PLL2_RG_RCLRPLL_EN, 31)

/* DDRPHY_PLL1 */
DEFINE_BIT(PLL1_RG_RPHYPLL_EN, 31)

/* DRAMC_REG_PADCTRL */
DEFINE_BITFIELD(PADCTRL_FIXDQIEN, 19, 16)

/* SPM_POWER_ON_VAL0 */
DEFINE_BIT(SPM_POWER_ON_VAL0_SC_DR_SHU_EN_PCM, 22)
DEFINE_BIT(SPM_POWER_ON_VAL0_SC_DPHY_RXDLY_TRACK_EN, 25)
DEFINE_BIT(SPM_POWER_ON_VAL0_SC_DDRPHY_FB_CK_EN_PCM, 16)
DEFINE_BIT(SPM_POWER_ON_VAL0_SC_TX_TRACKING_DIS, 11)
DEFINE_BITFIELD(SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL_PCM, 31, 30)
DEFINE_BIT(SPM_POWER_ON_VAL0_SC_PHYPLL2_SHU_EN_PCM, 27)
DEFINE_BIT(SPM_POWER_ON_VAL0_SC_PHYPLL1_SHU_EN_PCM, 26)

/* SPM_POWER_ON_VAL1 */
DEFINE_BIT(SPM_POWER_ON_VAL1_SC_DR_SHORT_QUEUE_PCM, 31)

/* SPM_DRAMC_DPY_CLK_SW_CON */
DEFINE_BITFIELD(DRAMC_DPY_CLK_SW_CON_SC_DMDRAMCSHU_ACK, 25, 24)

/* DRAMC_REG_DRAMC_PD_CTRL */
DEFINE_BIT(DRAMC_PD_CTRL_DCMEN, 0)
DEFINE_BIT(DRAMC_PD_CTRL_PHYCLKDYNGEN, 30)
DEFINE_BIT(DRAMC_PD_CTRL_MIOCKCTRLOFF, 26)

/* DRAMC_REG_WRITE_LEV */
DEFINE_BIT(WRITE_LEV_DQS_WLEV, 7)
DEFINE_BITFIELD(WRITE_LEV_DQSBX_G, 11, 8)
DEFINE_BITFIELD(WRITE_LEV_DQS_SEL, 19, 16)
DEFINE_BITFIELD(WRITE_LEV_DMVREFCA, 27, 20)
DEFINE_BIT(WRITE_LEV_WRITE_LEVEL_EN, 0)
DEFINE_BIT(WRITE_LEV_BYTEMODECBTEN, 3)

/* DRAMC_REG_STBCAL */
DEFINE_BIT(STBCAL_DQSIENCG_NORMAL_EN, 29)

/* DDRPHY_B0_DQ5 */
DEFINE_BIT(B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0, 17)

/* DDRPHY_B1_DQ5 */
DEFINE_BIT(B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1, 17)

/* DDRPHY_B0_DQ3 */
DEFINE_BIT(B0_DQ3_RG_RX_ARDQ_SMT_EN_B0, 1)

/* DDRPHY_B1_DQ3 */
DEFINE_BIT(B1_DQ3_RG_RX_ARDQ_SMT_EN_B1, 1)

/* DDRPHY_CA_CMD5  */
DEFINE_BIT(CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN, 17)

/* DDRPHY_CA_CMD3  */
DEFINE_BIT(CA_CMD3_RG_RX_ARCMD_SMT_EN, 1)

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
