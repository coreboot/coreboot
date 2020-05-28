/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRAMC_REGISTER_H_
#define _DRAMC_REGISTER_H_

#include <types.h>

#define DRIVING_DS2_0	    7	/* DS[2:0] 7->6 */
#define DEFAULT_DRIVING  0x99009900

enum {
	/* CONF2 = 0x008 */
	CONF2_TEST1_EN  = BIT(29),
	CONF2_TEST2R_EN = BIT(30),
	CONF2_TEST2W_EN = BIT(31),
	/* PADCTL1 = 0x00c */
	PADCTL1_CLK_SHIFT = 24,
	PADCTL1_CS1_SHIFT = 28,
	/* PADCTL2 = 0x010 */
	MASK_PADCTL2_16BIT = 0x000000ff,
	MASK_PADCTL2_32BIT = 0x0000ffff,
	MASK_PADCTL2	   = 0xffff0000,
	PADCTL2_SHIFT = 0,
	/* TEST2_3 = 0x044 */
	TEST2_3_TESTCNT_SHIFT = 0,
	TEST2_3_TESTCNT_MASK  = (0xful << TEST2_3_TESTCNT_SHIFT),
	TEST2_3_TESTAUDPAT_EN = BIT(7),
	TEST2_3_ADVREFEN_EN   = BIT(30),
	/* TEST2_4 = 0x048 */
	TEST2_4_TESTAUDINC_SHIFT  = 0,
	TEST2_4_TESTAUDINC_MASK   = (0x1ful << TEST2_4_TESTAUDINC_SHIFT),
	TEST2_4_TESTAUDINIT_SHIFT = 8,
	TEST2_4_TESTAUDINIT_MASK  = (0x1ful << TEST2_4_TESTAUDINIT_SHIFT),
	TEST2_4_TESTAUDBITINV_EN  = BIT(14),
	TEST2_4_TESTAUDMODE_EN    = BIT(15),
	TEST2_4_TESTXTALKPAT_EN   = BIT(16),
	/* DDR2CTL = 0x07c */
	DDR2CTL_WOEN_SHIFT = 3,
	DDR2CTL_DATLAT_SHIFT = 4,
	/* MISC = 0x80 */
	MISC_LATNORMP_SHIFT = 0,
	MISC_DATLAT_DSEL_SHIFT = 8,
	/* MRS = 0x088 */
	MASK_MR2_OP = 0x00800000,
	/* R0 R1 DQSIEN = 0x094 */
	DQSIEN_DQS0IEN_SHIFT = 0,
	DQSIEN_DQS1IEN_SHIFT = 8,
	DQSIEN_DQS2IEN_SHIFT = 16,
	DQSIEN_DQS3IEN_SHIFT = 24,
	/* MCKDLY = 0x0d8 */
	MCKDLY_DQIENLAT_SHIFT = 4,
	MCKDLY_DQIENQKEND_SHIFT = 10,
	MCKDLY_FIXDQIEN_SHIFT = 12,
	MCKDLY_FIXODT_SHIFT = 23,
	/* DQSCTL1 = 0x0e0 */
	DQSCTL1_DQSINCTL_SHIFT = 24,
	DQSCTL1_DQSIENMODE_SHIFT = 28,
	/* PADCTL4 = 0x0e4 */
	PADCTL4_CKEFIXON_SHIFT = 2,
	PADCTL4_DATLAT3_SHIFT = 4,
	/* PHYCTL1 = 0x0f0 */
	PHYCTL1_DATLAT4_SHIFT = 25,
	PHYCTL1_PHYRST_SHIFT = 28,
	/* GDDR3CTL1 = 0x0f4 */
	GDDR3CTL1_BKSWAP_SHIFT = 20,
	GDDR3CTL1_RDATRST_SHIFT = 25,
	GDDR3CTL1_DQMSWAP_SHIFT = 31,
	/* RKCFG = 0x110 */
	MASK_RKCFG_RKSWAP_EN = 0x08,
	RKCFG_PBREF_DISBYRATE_SHIFT = 6,
	RKCFG_WDATKEY64_SHIFT = 29,
	/* DQSCTL2 = 0x118 */
	DQSCTL2_DQSINCTL_SHIFT = 0,
	/* DQSGCTL = 0x124 */
	DQSGCTL_DQSGDUALP_SHIFT = 30,
	/* PHYCLKDUTY = 0x148 */
	PHYCLKDUTY_CMDCLKP0DUTYN_SHIFT = 16,
	PHYCLKDUTY_CMDCLKP0DUTYP_SHIFT = 18,
	PHYCLKDUTY_CMDCLKP0DUTYSEL_SHIFT = 28,
	/* CMDDLY0 = 0x1a8 */
	CMDDLY0_RA0_SHIFT = 0,
	CMDDLY0_RA1_SHIFT = 8,
	CMDDLY0_RA2_SHIFT = 16,
	CMDDLY0_RA3_SHIFT = 24,
	/* CMDDLY1 = 0x1ac */
	CMDDLY1_RA7_SHIFT = 24,
	/* CMDDLY3 = 0x1b4 */
	CMDDLY3_BA0_SHIFT = 8,
	CMDDLY3_BA1_SHIFT = 16,
	CMDDLY3_BA2_SHIFT = 24,
	/* CMDDLY4 = 0x1b8 */
	CMDDLY4_CS_SHIFT = 0,
	CMDDLY4_CKE_SHIFT = 8,
	CMDDLY4_RAS_SHIFT = 16,
	CMDDLY4_CAS_SHIFT = 24,
	/* CMDDLY5 = 0x1bc */
	CMDDLY5_WE_SHIFT = 8,
	CMDDLY5_RA13_SHIFT = 16,
	/* DQSCAL0 = 0x1c0 */
	DQSCAL0_RA14_SHIFT = 24,
	DQSCAL0_STBCALEN_SHIFT = 31,
	/* DQSCAL1 = 0x1c4 */
	DQSCAL1_CKE1_SHIFT = 24,
	/* IMPCAL = 0x1c8 */
	IMP_CALI_EN_SHIFT = 0,
	IMP_CALI_HW_SHIFT = 1,
	IMP_CALI_ENN_SHIFT = 4,
	IMP_CALI_ENP_SHIFT = 5,
	IMP_CALI_PDN_SHIFT = 6,
	IMP_CALI_PDP_SHIFT = 7,
	IMP_CALI_DRVP_SHIFT = 8,
	IMP_CALI_DRVN_SHIFT = 12,
	/* JMETER for PLL2, PLL3, PLL4 */
	JMETER_EN_BIT= BIT(0),
	JMETER_COUNTER_SHIFT = 16,
	JMETER_COUNTER_MASK = (0xffff << JMETER_COUNTER_SHIFT),
	/* SPCMD = 0x1e4 */
	SPCMD_MRWEN_SHIFT = 0,
	SPCMD_DQSGCNTEN_SHIFT = 8,
	SPCMD_DQSGCNTRST_SHIFT = 9,
	/* JMETER for PLL2/3/4 ST */
	JMETER_PLL_ZERO_SHIFT = 0,
	JMETER_PLL_ONE_SHIFT = 16,
	/* TESTRPT = 0x3fc */
	TESTRPT_DM_CMP_CPT_SHIFT = 10,
	TESTRPT_DM_CMP_ERR_SHIFT = 14,
	/* SELPH2 = 0x404 */
	SELPH2_TXDLY_DQSGATE_SHIFT = 12,
	SELPH2_TXDLY_DQSGATE_P1_SHIFT = 20,
	/* SELPH5 = 0x410 */
	SELPH5_DLY_DQSGATE_SHIFT = 22,
	SELPH5_DLY_DQSGATE_P1_SHIFT = 24,
	/* SELPH6_1 = 0x418 */
	SELPH6_1_DLY_R1DQSGATE_SHIFT = 0,
	SELPH6_1_DLY_R1DQSGATE_P1_SHIFT = 2,
	SELPH6_1_TXDLY_R1DQSGATE_SHIFT = 4,
	SELPH6_1_TXDLY_R1DQSGATE_P1_SHIFT = 8,
	/* MEMPLL_S14 = 0x638 */
	MASK_MEMPLL_DL = 0xc0ffffff,
	MEMPLL_FB_DL_SHIFT = 0,
	MEMPLL_REF_DL_SHIFT = 8,
	MEMPLL_DL_SHIFT = 24,
	MEMPLL_MODE_SHIFT = 29,
	/* MEMPLL_DIVIDER = 0x640 */
	MEMCLKENB_SHIFT = 5
};

struct dramc_ao_regs {
	uint32_t actim0;		/* 0x0 */
	uint32_t conf1;			/* 0x4 */
	uint32_t conf2;			/* 0x8 */
	uint32_t rsvd_ao1[3];		/* 0xc */
	uint32_t r0deldly;		/* 0x18 */
	uint32_t r1deldly;		/* 0x1c */
	uint32_t r0difdly;		/* 0x20 */
	uint32_t r1difdly;		/* 0x24 */
	uint32_t dllconf;		/* 0x28 */
	uint32_t rsvd_ao2[6];		/* 0x2c */
	uint32_t test2_3;		/* 0x44 */
	uint32_t test2_4;		/* 0x48 */
	uint32_t catraining;		/* 0x4c */
	uint32_t catraining2;		/* 0x50 */
	uint32_t wodt;			/* 0x54 */
	uint32_t rsvd_ao3[9];		/* 0x58 */
	uint32_t ddr2ctl;		/* 0x7c */
	uint32_t misc;			/* 0x80 */
	uint32_t zqcs;			/* 0x84 */
	uint32_t mrs;			/* 0x88 */
	uint32_t clk1delay;		/* 0x8c */
	uint32_t rsvd_ao4[1];		/* 0x90 */
	uint32_t dqsien[2];		/* 0x94 */
	uint32_t rsvd_ao5[2];		/* 0x9c */
	uint32_t iodrv1;		/* 0xa4 */
	uint32_t iodrv2;		/* 0xa8 */
	uint32_t iodrv3;		/* 0xac */
	uint32_t iodrv4;		/* 0xb0 */
	uint32_t iodrv5;		/* 0xb4 */
	uint32_t iodrv6;		/* 0xb8 */
	uint32_t drvctl1;		/* 0xbc */
	uint32_t dllsel;		/* 0xc0 */
	uint32_t rsvd_ao7[5];		/* 0xc4 */
	uint32_t mckdly;		/* 0xd8 */
	uint32_t rsvd_ao8[1];		/* 0xdc */
	uint32_t dqsctl1;		/* 0xe0 */
	uint32_t padctl4;		/* 0xe4 */
	uint32_t rsvd_ao9[2];		/* 0xe8 */
	uint32_t phyctl1;		/* 0xf0 */
	uint32_t gddr3ctl1;		/* 0xf4 */
	uint32_t padctl7;		/* 0xf8 */
	uint32_t misctl0;		/* 0xfc */
	uint32_t ocdk;			/* 0x100 */
	uint32_t rsvd_ao10[3];		/* 0x104 */
	uint32_t rkcfg;			/* 0x110 */
	uint32_t ckphdet;		/* 0x114 */
	uint32_t dqsctl2;		/* 0x118 */
	uint32_t rsvd_ao11[5];		/* 0x11c */
	uint32_t clkctl;		/* 0x130 */
	uint32_t rsvd_ao12[1];		/* 0x134 */
	uint32_t dummy;			/* 0x138 */
	uint32_t write_leveling;	/* 0x13c */
	uint32_t rsvd_ao13[10];		/* 0x140 */
	uint32_t arbctl0;		/* 0x168 */
	uint32_t rsvd_ao14[21];		/* 0x16c */
	uint32_t dqscal0;		/* 0x1c0 */
	uint32_t dqscal1;		/* 0x1c4 */
	uint32_t impcal;		/* 0x1c8 */
	uint32_t rsvd_ao15[4];		/* 0x1cc */
	uint32_t dramc_pd_ctrl;		/* 0x1dc */
	uint32_t lpddr2_3;		/* 0x1e0 */
	uint32_t spcmd;			/* 0x1e4 */
	uint32_t actim1;		/* 0x1e8 */
	uint32_t perfctl0;		/* 0x1ec */
	uint32_t ac_derating;		/* 0x1f0 */
	uint32_t rrrate_ctl;		/* 0x1f4 */
	uint32_t ac_time_05t;		/* 0x1f8 */
	uint32_t mrr_ctl;		/* 0x1fc */
	uint32_t rsvd_ao16[4];		/* 0x200 */
	uint32_t dqidly[9];		/* 0x210 */
	uint32_t rsvd_ao17[115];	/* 0x234 */
	uint32_t selph1;		/* 0x400 */
	uint32_t selph2;		/* 0x404 */
	uint32_t selph3;		/* 0x408 */
	uint32_t selph4;		/* 0x40c */
	uint32_t selph5;		/* 0x410 */
	uint32_t selph6;		/* 0x414 */
	uint32_t selph6_1;		/* 0x418 */
	uint32_t selph7;		/* 0x41c */
	uint32_t selph8;		/* 0x420 */
	uint32_t selph9;		/* 0x424 */
	uint32_t selph10;		/* 0x428 */
	uint32_t selph11;		/* 0x42c */
};

check_member(dramc_ao_regs, selph11, 0x42c);

struct dramc_nao_regs {
	uint32_t rsvd_nao1[11];			/* 0x0 */
	uint32_t test_mode;			/* 0x2c */
	uint32_t rsvd_nao2[3];			/* 0x30 */
	uint32_t test2_1;			/* 0x3c */
	uint32_t test2_2;			/* 0x40 */
	uint32_t rsvd_nao3[48];			/* 0x44 */
	uint32_t lbwdat0;			/* 0x104 */
	uint32_t lbwdat1;			/* 0x108 */
	uint32_t lbwdat2;			/* 0x10c */
	uint32_t rsvd_nao4[1];			/* 0x110 */
	uint32_t ckphdet;			/* 0x114 */
	uint32_t rsvd_nao5[48];			/* 0x118 */
	uint32_t dmmonitor;			/* 0x1d8 */
	uint32_t rsvd_nao6[41];			/* 0x1dc */
	uint32_t r2r_page_hit_counter;		/* 0x280 */
	uint32_t r2r_page_miss_counter;		/* 0x284 */
	uint32_t r2r_interbank_counter;		/* 0x288 */
	uint32_t r2w_page_hit_counter;		/* 0x28c */
	uint32_t r2w_page_miss_counter;		/* 0x290 */
	uint32_t r2w_interbank_counter;		/* 0x294 */
	uint32_t w2r_page_hit_counter;		/* 0x298 */
	uint32_t w2r_page_miss_counter;		/* 0x29c */
	uint32_t w2r_page_interbank_counter;	/* 0x2a0 */
	uint32_t w2w_page_hit_counter;		/* 0x2a4 */
	uint32_t w2w_page_miss_counter;		/* 0x2a8 */
	uint32_t w2w_page_interbank_counter;	/* 0x2ac */
	uint32_t dramc_idle_counter;		/* 0x2b0 */
	uint32_t freerun_26m_counter;		/* 0x2b4 */
	uint32_t refresh_pop_counter;		/* 0x2b8 */
	uint32_t jmeter_st;			/* 0x2bc */
	uint32_t dq_cal_max[8];			/* 0x2c0 */
	uint32_t dqs_cal_min[8];		/* 0x2e0 */
	uint32_t dqs_cal_max[8];		/* 0x300 */
	uint32_t rsvd_nao7[4];			/* 0x320 */
	uint32_t read_bytes_counter;		/* 0x330 */
	uint32_t write_bytes_counter;		/* 0x334 */
	uint32_t rsvd_nao8[6];			/* 0x338 */
	uint32_t dqical[4];			/* 0x350 */
	uint32_t rsvd_nao9[4];			/* 0x360 */
	uint32_t cmp_err;			/* 0x370 */
	uint32_t r0dqsiendly;			/* 0x374 */
	uint32_t r1dqsiendly;			/* 0x378 */
	uint32_t rsvd_nao10[9];			/* 0x37c */
	uint32_t dqsdly0;			/* 0x3a0 */
	uint32_t rsvd_nao11[4];			/* 0x3a4 */
	uint32_t mrrdata;			/* 0x3b4 */
	uint32_t spcmdresp;			/* 0x3b8 */
	uint32_t iorgcnt;			/* 0x3bc */
	uint32_t dqsgnwcnt[6];			/* 0x3c0 */
	uint32_t rsvd_nao12[4];			/* 0x3d8 */
	uint32_t ckphcnt;			/* 0x3e8 */
	uint32_t rsvd_nao13[4];			/* 0x3ec */
	uint32_t testrpt;			/* 0x3fc */
};

check_member(dramc_nao_regs, testrpt, 0x3fc);

struct dramc_ddrphy_regs {
	uint32_t rsvd_phy1[3];		/* 0x0 */
	uint32_t padctl1;		/* 0xc */
	uint32_t padctl2;		/* 0x10 */
	uint32_t padctl3;		/* 0x14 */
	uint32_t rsvd_phy2[25];		/* 0x18 */
	uint32_t ddr2ctl;		/* 0x7c */
	uint32_t rsvd_phy3[3];		/* 0x80 */
	uint32_t clk1delay;		/* 0x8c */
	uint32_t ioctl;			/* 0x90 */
	uint32_t rsvd_phy4[7];		/* 0x94 */
	uint32_t iodrv4;		/* 0xb0 */
	uint32_t iodrv5;		/* 0xb4 */
	uint32_t iodrv6;		/* 0xb8 */
	uint32_t drvctl1;		/* 0xbc */
	uint32_t dllsel;		/* 0xc0 */
	uint32_t rsvd_phy5[2];		/* 0xc4 */
	uint32_t tdsel[3];		/* 0xcc */
	uint32_t mckdly;		/* 0xd8 */
	uint32_t dqsctl0;		/* 0xdc */
	uint32_t dqsctl1;		/* 0xe0 */
	uint32_t dqsctl4;		/* 0xe4 */
	uint32_t dqsctl5;		/* 0xe8 */
	uint32_t dqsctl6;		/* 0xec */
	uint32_t phyctl1;		/* 0xf0 */
	uint32_t gddr3ctl1;		/* 0xf4 */
	uint32_t rsvd_phy6[1];		/* 0xf8 */
	uint32_t misctl0;		/* 0xfc */
	uint32_t ocdk;			/* 0x100 */
	uint32_t rsvd_phy7[8];		/* 0x104 */
	uint32_t dqsgctl;		/* 0x124 */
	uint32_t rsvd_phy8[6];		/* 0x128 */
	uint32_t ddrphydqsgctl;		/* 0x140 */
	uint32_t dqsgct2;		/* 0x144 */
	uint32_t phyclkduty;		/* 0x148 */
	uint32_t rsvd_phy9[3];		/* 0x14c */
	uint32_t dqsisel;		/* 0x158 */
	uint32_t dqmdqs_sel;		/* 0x15c */
	uint32_t rsvd_phy10[10];	/* 0x160 */
	uint32_t jmeterpop1;		/* 0x188 */
	uint32_t jmeterpop2;		/* 0x18c */
	uint32_t jmeterpop3;		/* 0x190 */
	uint32_t jmeterpop4;		/* 0x194 */
	uint32_t rsvd_phy11[4];		/* 0x198 */
	uint32_t cmddly[6];		/* 0x1a8 */
	uint32_t dqscal0;		/* 0x1c0 */
	uint32_t rsvd_phy12[2];		/* 0x1c4 */
	uint32_t jmeter[3];		/* 0x1cc */
	uint32_t rsvd_phy13[2];		/* 0x1d8 */
	uint32_t lpddr2_3;		/* 0x1e0 */
	uint32_t spcmd;			/* 0x1e4 */
	uint32_t rsvd_phy14[6];		/* 0x1e8 */
	uint32_t dqodly[4];		/* 0x200 */
	uint32_t rsvd_phy15[11];	/* 0x210 */
	uint32_t lpddr2_4;		/* 0x23c */
	uint32_t rsvd_phy16[56];	/* 0x240 */
	uint32_t jmeter_pll_st[3];	/* 0x320 */
	uint32_t jmeter_done_st;	/* 0x32c */
	uint32_t rsvd_phy17[2];		/* 0x330 */
	uint32_t jmeter_pll1_st;	/* 0x338 */
	uint32_t jmeter_pop_pll2_st;	/* 0x33c */
	uint32_t jmeter_pop_pll3_st;	/* 0x340 */
	uint32_t jmeter_pop_pll4_st;	/* 0x344 */
	uint32_t jmeter_pop_pll1_st;	/* 0x348 */
	uint32_t rsvd_phy18[13];	/* 0x34c */
	uint32_t dq_o1;			/* 0x380 */
	uint32_t rsvd_phy19[2];		/* 0x384 */
	uint32_t stben[4];		/* 0x38c */
	uint32_t rsvd_phy20[16];	/* 0x39c */
	uint32_t dllcnt0;		/* 0x3dc */
	uint32_t pllautok;		/* 0x3e0 */
	uint32_t poppllautok;		/* 0x3e4 */
	uint32_t rsvd_phy21[18];	/* 0x3e8 */
	uint32_t selph12;		/* 0x430 */
	uint32_t selph13;		/* 0x434 */
	uint32_t selph14;		/* 0x438 */
	uint32_t selph15;		/* 0x43c */
	uint32_t selph16;		/* 0x440 */
	uint32_t selph17;		/* 0x444 */
	uint32_t selph18;		/* 0x448 */
	uint32_t selph19;		/* 0x44c */
	uint32_t selph20;		/* 0x450 */
	uint32_t rsvd_phy22[91];	/* 0x454 */
	uint32_t peri[4];		/* 0x5c0 */
	uint32_t rsvd_phy23[12];	/* 0x5d0 */
	uint32_t mempll[15];		/* 0x600 */
	uint32_t ddrphy_cg_ctrl;	/* 0x63c */
	uint32_t mempll_divider;	/* 0x640 */
	uint32_t vrefctl0;		/* 0x644 */
	uint32_t rsvd_phy24[18];	/* 0x648 */
	uint32_t mempll05_divider;	/* 0x690 */
};

check_member(dramc_ddrphy_regs, mempll05_divider, 0x690);

struct emi_regs {
	uint32_t emi_cona;		/* 0x0 */
	uint32_t rsvd_emi1;		/* 0x4 */
	uint32_t emi_conb;		/* 0x08 */
	uint32_t rsvd_emi2;		/* 0x0c */
	uint32_t emi_conc;		/* 0x10 */
	uint32_t rsvd_emi3;		/* 0x14 */
	uint32_t emi_cond;		/* 0x18 */
	uint32_t rsvd_emi4;		/* 0x1c */
	uint32_t emi_cone;		/* 0x20 */
	uint32_t rsvd_emi5;		/* 0x24 */
	uint32_t emi_conf;		/* 0x28 */
	uint32_t rsvd_emi6;		/* 0x2c */
	uint32_t emi_cong;		/* 0x30 */
	uint32_t rsvd_emi7;		/* 0x34 */
	uint32_t emi_conh;		/* 0x38 */
	uint32_t rsvd_emi8[9];		/* 0x3c */
	uint32_t emi_conm;		/* 0x60 */
	uint32_t rsvd_emi9[5];		/* 0x64 */
	uint32_t emi_mdct;		/* 0x78 */
	uint32_t rsvd_emi10[21];	/* 0x7c */
	uint32_t emi_test0;		/* 0xd0 */
	uint32_t rsvd_emi11;		/* 0xd4 */
	uint32_t emi_test1;		/* 0xd8 */
	uint32_t rsvd_emi12;		/* 0xdc */
	uint32_t emi_testa;		/* 0xe0 */
	uint32_t rsvd_emi13;		/* 0xe4 */
	uint32_t emi_testb;		/* 0xe8 */
	uint32_t rsvd_emi14;		/* 0xec */
	uint32_t emi_testc;		/* 0xf0 */
	uint32_t rsvd_emi15;		/* 0xf4 */
	uint32_t emi_testd;		/* 0xf8 */
	uint32_t rsvd_emi16;		/* 0xfc */
	uint32_t emi_arba;		/* 0x100 */
	uint32_t rsvd_emi17[3];		/* 0x104 */
	uint32_t emi_arbc;		/* 0x110 */
	uint32_t rsvd_emi18;		/* 0x114 */
	uint32_t emi_arbd;		/* 0x118 */
	uint32_t rsvd_emi19;		/* 0x11c */
	uint32_t emi_arbe;		/* 0x120 */
	uint32_t rsvd_emi20;		/* 0x124 */
	uint32_t emi_arbf;		/* 0x128 */
	uint32_t rsvd_emi21;		/* 0x12c */
	uint32_t emi_arbg;		/* 0x130 */
	uint32_t rsvd_emi22;		/* 0x134 */
	uint32_t emi_arbh;		/* 0x138 */
	uint32_t rsvd_emi23;		/* 0x13c */
	uint32_t emi_arbi;		/* 0x140 */
	uint32_t emi_arbi_2nd;		/* 0x144 */
	uint32_t emi_arbj;		/* 0x148 */
	uint32_t emi_arbj_2nd;		/* 0x14c */
	uint32_t emi_arbk;		/* 0x150 */
	uint32_t emi_arbk_2nd;		/* 0x154 */
	uint32_t emi_slct;		/* 0x158 */
	uint32_t rsvd_emi24;		/* 0x15C */
	uint32_t emi_mpua;		/* 0x160 */
	uint32_t rsvd_emi25;		/* 0x164 */
	uint32_t emi_mpub;		/* 0x168 */
	uint32_t rsvd_emi26;		/* 0x16c */
	uint32_t emi_mpuc;		/* 0x170 */
	uint32_t rsvd_emi27;		/* 0x174 */
	uint32_t emi_mpud;		/* 0x178 */
	uint32_t rsvd_emi28;		/* 0x17C */
	uint32_t emi_mpue;		/* 0x180 */
	uint32_t rsvd_emi29;		/* 0x184 */
	uint32_t emi_mpuf;		/* 0x188 */
	uint32_t rsvd_emi30;		/* 0x18C */
	uint32_t emi_mpug;		/* 0x190 */
	uint32_t rsvd_emi31;		/* 0x194 */
	uint32_t emi_mpuh;		/* 0x198 */
	uint32_t rsvd_emi32;		/* 0x19C */
	uint32_t emi_mpui;		/* 0x1A0 */
	uint32_t rsvd_emi33;		/* 0x1A4 */
	uint32_t emi_mpuj;		/* 0x1A8 */
	uint32_t rsvd_emi34;		/* 0x1AC */
	uint32_t emi_mpuk;		/* 0x1B0 */
	uint32_t rsvd_emi35;		/* 0x1B4 */
	uint32_t emi_mpul;		/* 0x1B8 */
	uint32_t rsvd_emi36;		/* 0x1BC */
	uint32_t emi_mpum;		/* 0x1C0 */
	uint32_t rsvd_emi37;		/* 0x1C4 */
	uint32_t emi_mpun;		/* 0x1C8 */
	uint32_t rsvd_emi38;		/* 0x1CC */
	uint32_t emi_mpuo;		/* 0x1D0 */
	uint32_t rsvd_emi39;		/* 0x1D4 */
	uint32_t emi_mpup;		/* 0x1D8 */
	uint32_t rsvd_emi40;		/* 0x1DC */
	uint32_t emi_mpuq;		/* 0x1E0 */
	uint32_t rsvd_emi41;		/* 0x1E4 */
	uint32_t emi_mpur;		/* 0x1E8 */
	uint32_t rsvd_emi42;		/* 0x1EC */
	uint32_t emi_mpus;		/* 0x1F0 */
	uint32_t rsvd_emi43;		/* 0x1F4 */
	uint32_t emi_mput;		/* 0x1F8 */
	uint32_t rsvd_emi44;		/* 0x1FC */
	uint32_t emi_mpuu;		/* 0x200 */
	uint32_t rsvd_emi45[7];		/* 0x204 */
	uint32_t emi_mpuy;		/* 0x220 */
	uint32_t rsvd_emi46[119];	/* 0x224 */
	uint32_t emi_bmen;		/* 0x400 */
};

check_member(emi_regs, emi_bmen, 0x400);

extern struct dramc_ao_regs *ao_regs;
extern struct dramc_nao_regs *nao_regs;
extern struct dramc_ddrphy_regs *ddrphy_regs;

struct dramc_channel {
	struct dramc_ao_regs *ao_regs;
	struct dramc_nao_regs *nao_regs;
	struct dramc_ddrphy_regs *ddrphy_regs;
};

static struct dramc_channel const ch[2] = {
	{(void *)CHA_DRAMCAO_BASE, (void *)CHA_DRAMCNAO_BASE, (void *)CHA_DDRPHY_BASE},
	{(void *)CHB_DRAMCAO_BASE, (void *)CHB_DRAMCNAO_BASE, (void *)CHB_DDRPHY_BASE}
};

#endif /* _DRAMC_REGISTER_H_ */
