/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2012 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CPU_SAMSUNG_EXYNOS5250_CLK_H
#define CPU_SAMSUNG_EXYNOS5250_CLK_H

#include <stdint.h>

enum periph_id;

#define APLL	0
#define MPLL	1
#define EPLL	2
#define HPLL	3
#define VPLL	4
#define BPLL	5

enum pll_src_bit {
	SRC_MPLL = 6,
	SRC_EPLL,
	SRC_VPLL,
};

/* *
 * This structure is to store the src bit, div bit and prediv bit
 * positions of the peripheral clocks of the src and div registers
 */
struct clk_bit_info {
	s8 src_bit;    /* offset in register to clock source field */
	s8 n_src_bits; /* number of bits in 'src_bit' field */
	s8 div_bit;
	s8 prediv_bit;
};

unsigned long get_pll_clk(int pllreg);
unsigned long get_arm_clk(void);
unsigned long get_pwm_clk(void);
unsigned long get_uart_clk(int dev_index);
void set_mmc_clk(int dev_index, unsigned int div);

/**
 * get the clk frequency of the required peripherial
 *
 * @param peripherial	Peripherial id
 *
 * @return frequency of the peripherial clk
 */
unsigned long clock_get_periph_rate(enum periph_id peripheral);

#include "pinmux.h"


#define MCT_ADDRESS 0x101c0000

#define MCT_HZ 24000000

/*
 * Set mshci controller instances clock divider
 *
 * @param enum periph_id instance of the mshci controller
 *
 * Return	0 if ok else -1
 */
int clock_set_mshci(enum periph_id peripheral);

/*
 * Sets the epll clockrate
 *
 * @param rate	Required clock rate to the presacaler in Hz
 *
 * Return	0 if ok else -1
 */
int clock_epll_set_rate(unsigned long rate);

/*
 * selects the clk source for I2S MCLK
 */
void clock_select_i2s_clk_source(void);

/*
 * Set prescaler division based on input and output frequency
 * for i2s audio clock
 *
 * @param src_frq	Source frequency in Hz
 * @param dst_frq	Required MCLK frequency in Hz
 *
 * Return	0 if ok else -1
 */
int clock_set_i2s_clk_prescaler(unsigned int src_frq, unsigned int dst_frq);

struct exynos5_clock {
	unsigned int	apll_lock;		/* base + 0 */
	unsigned char	res1[0xfc];
	unsigned int	apll_con0;
	unsigned int	apll_con1;
	unsigned char	res2[0xf8];
	unsigned int	src_cpu;
	unsigned char	res3[0x1fc];
	unsigned int	mux_stat_cpu;
	unsigned char	res4[0xfc];
	unsigned int	div_cpu0;
	unsigned int	div_cpu1;
	unsigned char	res5[0xf8];
	unsigned int	div_stat_cpu0;
	unsigned int	div_stat_cpu1;
	unsigned char	res6[0x1f8];
	unsigned int	gate_sclk_cpu;
	unsigned char	res7[0x1fc];
	unsigned int	clkout_cmu_cpu;
	unsigned int	clkout_cmu_cpu_div_stat;
	unsigned char	res8[0x5f8];

	unsigned int	armclk_stopctrl;	/* base + 0x1000 */
	unsigned int	atclk_stopctrl;
	unsigned char	res9[0x8];
	unsigned int	parityfail_status;
	unsigned int	parityfail_clear;
	unsigned char	res10[0x8];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctr2;
	unsigned char	res11[0xd8];
	unsigned int	apll_con0_l8;
	unsigned int	apll_con0_l7;
	unsigned int	apll_con0_l6;
	unsigned int	apll_con0_l5;
	unsigned int	apll_con0_l4;
	unsigned int	apll_con0_l3;
	unsigned int	apll_con0_l2;
	unsigned int	apll_con0_l1;
	unsigned int	iem_control;
	unsigned char	res12[0xdc];
	unsigned int	apll_con1_l8;
	unsigned int	apll_con1_l7;
	unsigned int	apll_con1_l6;
	unsigned int	apll_con1_l5;
	unsigned int	apll_con1_l4;
	unsigned int	apll_con1_l3;
	unsigned int	apll_con1_l2;
	unsigned int	apll_con1_l1;
	unsigned char	res13[0xe0];
	unsigned int	div_iem_l8;
	unsigned int	div_iem_l7;
	unsigned int	div_iem_l6;
	unsigned int	div_iem_l5;
	unsigned int	div_iem_l4;
	unsigned int	div_iem_l3;
	unsigned int	div_iem_l2;
	unsigned int	div_iem_l1;
	unsigned char	res14[0x2ce0];

	unsigned int	mpll_lock;		/* base + 0x4000 */
	unsigned char	res15[0xfc];
	unsigned int	mpll_con0;
	unsigned int	mpll_con1;
	unsigned char	res16[0xf8];
	unsigned int	src_core0;
	unsigned int	src_core1;
	unsigned char	res17[0xf8];
	unsigned int	src_mask_core;
	unsigned char	res18[0x100];
	unsigned int	mux_stat_core1;
	unsigned char	res19[0xf8];
	unsigned int	div_core0;
	unsigned int	div_core1;
	unsigned int	div_sysrgt;
	unsigned char	res20[0xf4];
	unsigned int	div_stat_core0;
	unsigned int	div_stat_core1;
	unsigned int	div_stat_sysrgt;
	unsigned char	res21[0x2f4];
	unsigned int	gate_ip_core;
	unsigned int	gate_ip_sysrgt;
	unsigned char	res22[0xf8];
	unsigned int	clkout_cmu_core;
	unsigned int	clkout_cmu_core_div_stat;
	unsigned char	res23[0x5f8];

	unsigned int	dcgidx_map0;		/* base + 0x5000 */
	unsigned int	dcgidx_map1;
	unsigned int	dcgidx_map2;
	unsigned char	res24[0x14];
	unsigned int	dcgperf_map0;
	unsigned int	dcgperf_map1;
	unsigned char	res25[0x18];
	unsigned int	dvcidx_map;
	unsigned char	res26[0x1c];
	unsigned int	freq_cpu;
	unsigned int	freq_dpm;
	unsigned char	res27[0x18];
	unsigned int	dvsemclk_en;
	unsigned int	maxperf;
	unsigned char	res28[0x3478];

	unsigned int	div_acp;		/* base + 0x8500 */
	unsigned char	res29[0xfc];
	unsigned int	div_stat_acp;
	unsigned char	res30[0x1fc];
	unsigned int	gate_ip_acp;
	unsigned char	res31a[0xfc];
	unsigned int	div_syslft;
	unsigned char	res31b[0xc];
	unsigned int	div_stat_syslft;
	unsigned char	res31c[0xc];
	unsigned int	gate_bus_syslft;
	unsigned char	res31d[0xdc];
	unsigned int	clkout_cmu_acp;
	unsigned int	clkout_cmu_acp_div_stat;
	unsigned char	res32[0x38f8];

	unsigned int	div_isp0;		/* base + 0xc300 */
	unsigned int	div_isp1;
	unsigned int	div_isp2;
	unsigned char	res33[0xf4];

	unsigned int	div_stat_isp0;		/* base + 0xc400 */
	unsigned int	div_stat_isp1;
	unsigned int	div_stat_isp2;
	unsigned char	res34[0x3f4];

	unsigned int	gate_ip_isp0;		/* base + 0xc800 */
	unsigned int	gate_ip_isp1;
	unsigned char	res35[0xf8];
	unsigned int	gate_sclk_isp;
	unsigned char	res36[0xc];
	unsigned int	mcuisp_pwr_ctrl;
	unsigned char	res37[0xec];
	unsigned int	clkout_cmu_isp;
	unsigned int	clkout_cmu_isp_div_stat;
	unsigned char	res38[0x3618];

	unsigned int	cpll_lock;		/* base + 0x10020 */
	unsigned char	res39[0xc];
	unsigned int	epll_lock;
	unsigned char	res40[0xc];
	unsigned int	vpll_lock;
	unsigned char	res41a[0xc];
	unsigned int	gpll_lock;
	unsigned char	res41b[0xcc];
	unsigned int	cpll_con0;
	unsigned int	cpll_con1;
	unsigned char	res42[0x8];
	unsigned int	epll_con0;
	unsigned int	epll_con1;
	unsigned int	epll_con2;
	unsigned char	res43[0x4];
	unsigned int	vpll_con0;
	unsigned int	vpll_con1;
	unsigned int	vpll_con2;
	unsigned char	res44a[0x4];
	unsigned int	gpll_con0;
	unsigned int	gpll_con1;
	unsigned char	res44b[0xb8];
	unsigned int	src_top0;
	unsigned int	src_top1;
	unsigned int	src_top2;
	unsigned int	src_top3;
	unsigned int	src_gscl;
	unsigned int	src_disp0_0;
	unsigned int	src_disp0_1;
	unsigned int	src_disp1_0;
	unsigned int	src_disp1_1;
	unsigned char	res46[0xc];
	unsigned int	src_mau;
	unsigned int	src_fsys;
	unsigned char	res47[0x8];
	unsigned int	src_peric0;
	unsigned int	src_peric1;
	unsigned char	res48[0x18];
	unsigned int	sclk_src_isp;
	unsigned char	res49[0x9c];
	unsigned int	src_mask_top;
	unsigned char	res50[0xc];
	unsigned int	src_mask_gscl;
	unsigned int	src_mask_disp0_0;
	unsigned int	src_mask_disp0_1;
	unsigned int	src_mask_disp1_0;
	unsigned int	src_mask_disp1_1;
	unsigned int	src_mask_maudio;
	unsigned char	res52[0x8];
	unsigned int	src_mask_fsys;
	unsigned char	res53[0xc];
	unsigned int	src_mask_peric0;
	unsigned int	src_mask_peric1;
	unsigned char	res54[0x18];
	unsigned int	src_mask_isp;
	unsigned char	res55[0x9c];
	unsigned int	mux_stat_top0;
	unsigned int	mux_stat_top1;
	unsigned int	mux_stat_top2;
	unsigned int	mux_stat_top3;
	unsigned char	res56[0xf0];
	unsigned int	div_top0;
	unsigned int	div_top1;
	unsigned char	res57[0x8];
	unsigned int	div_gscl;
	unsigned int	div_disp0_0;
	unsigned int	div_disp0_1;
	unsigned int	div_disp1_0;
	unsigned int	div_disp1_1;
	unsigned char	res59[0x8];
	unsigned int	div_gen;
	unsigned char	res60[0x4];
	unsigned int	div_mau;
	unsigned int	div_fsys0;
	unsigned int	div_fsys1;
	unsigned int	div_fsys2;
	unsigned int	div_fsys3;
	unsigned int	div_peric0;
	unsigned int	div_peric1;
	unsigned int	div_peric2;
	unsigned int	div_peric3;
	unsigned int	div_peric4;
	unsigned int	div_peric5;
	unsigned char	res61[0x10];
	unsigned int	sclk_div_isp;
	unsigned char	res62[0xc];
	unsigned int	div2_ratio0;
	unsigned int	div2_ratio1;
	unsigned char	res63[0x8];
	unsigned int	div4_ratio;
	unsigned char	res64[0x6c];
	unsigned int	div_stat_top0;
	unsigned int	div_stat_top1;
	unsigned char	res65[0x8];
	unsigned int	div_stat_gscl;
	unsigned int	div_stat_disp0_0;
	unsigned int	div_stat_disp0_1;
	unsigned int	div_stat_disp1_0;
	unsigned int	div_stat_disp1_1;
	unsigned char	res67[0x8];
	unsigned int	div_stat_gen;
	unsigned char	res68[0x4];
	unsigned int	div_stat_maudio;
	unsigned int	div_stat_fsys0;
	unsigned int	div_stat_fsys1;
	unsigned int	div_stat_fsys2;
	unsigned int	div_stat_fsys3;
	unsigned int	div_stat_peric0;
	unsigned int	div_stat_peric1;
	unsigned int	div_stat_peric2;
	unsigned int	div_stat_peric3;
	unsigned int	div_stat_peric4;
	unsigned int	div_stat_peric5;
	unsigned char	res69[0x10];
	unsigned int	sclk_div_stat_isp;
	unsigned char	res70[0xc];
	unsigned int	div2_stat0;
	unsigned int	div2_stat1;
	unsigned char	res71[0x8];
	unsigned int	div4_stat;
	unsigned char	res72[0x180];
	unsigned int	gate_top_sclk_disp0;
	unsigned int	gate_top_sclk_disp1;
	unsigned int	gate_top_sclk_gen;
	unsigned char	res74[0xc];
	unsigned int	gate_top_sclk_mau;
	unsigned int	gate_top_sclk_fsys;
	unsigned char	res75[0xc];
	unsigned int	gate_top_sclk_peric;
	unsigned char	res76[0x1c];
	unsigned int	gate_top_sclk_isp;
	unsigned char	res77[0xac];
	unsigned int	gate_ip_gscl;
	unsigned int	gate_ip_disp0;
	unsigned int	gate_ip_disp1;
	unsigned int	gate_ip_mfc;
	unsigned int	gate_ip_g3d;
	unsigned int	gate_ip_gen;
	unsigned char	res79[0xc];
	unsigned int	gate_ip_fsys;
	unsigned char	res80[0x4];
	unsigned int	gate_ip_gps;
	unsigned int	gate_ip_peric;
	unsigned char	res81[0xc];
	unsigned int	gate_ip_peris;
	unsigned char	res82[0x1c];
	unsigned int	gate_block;
	unsigned char	res83[0x7c];
	unsigned int	clkout_cmu_top;
	unsigned int	clkout_cmu_top_div_stat;
	unsigned char	res84[0x37f8];

	unsigned int	src_lex;		/* base + 0x14200 */
	unsigned char	res85[0x1fc];
	unsigned int	mux_stat_lex;
	unsigned char	res85b[0xfc];
	unsigned int	div_lex;
	unsigned char	res86[0xfc];
	unsigned int	div_stat_lex;
	unsigned char	res87[0x1fc];
	unsigned int	gate_ip_lex;
	unsigned char	res88[0x1fc];
	unsigned int	clkout_cmu_lex;
	unsigned int	clkout_cmu_lex_div_stat;
	unsigned char	res89[0x3af8];

	unsigned int	div_r0x;		/* base + 0x18500 */
	unsigned char	res90[0xfc];
	unsigned int	div_stat_r0x;
	unsigned char	res91[0x1fc];
	unsigned int	gate_ip_r0x;
	unsigned char	res92[0x1fc];
	unsigned int	clkout_cmu_r0x;
	unsigned int	clkout_cmu_r0x_div_stat;
	unsigned char	res94[0x3af8];

	unsigned int	div_r1x;		/* base + 0x1c500 */
	unsigned char	res95[0xfc];
	unsigned int	div_stat_r1x;
	unsigned char	res96[0x1fc];
	unsigned int	gate_ip_r1x;
	unsigned char	res97[0x1fc];
	unsigned int	clkout_cmu_r1x;
	unsigned int	clkout_cmu_r1x_div_stat;
	unsigned char	res98[0x3608];

	unsigned int	bpll_lock;		/* base + 0x2000c */
	unsigned char	res99[0xfc];
	unsigned int	bpll_con0;
	unsigned int	bpll_con1;
	unsigned char	res100[0xe8];
	unsigned int	src_cdrex;
	unsigned char	res101[0x1fc];
	unsigned int	mux_stat_cdrex;
	unsigned char	res102[0xfc];
	unsigned int	div_cdrex;
	unsigned int	div_cdrex2;
	unsigned char	res103[0xf8];
	unsigned int	div_stat_cdrex;
	unsigned char	res104[0x2fc];
	unsigned int	gate_ip_cdrex;
	unsigned char	res105[0xc];
	unsigned int	c2c_monitor;
	unsigned int	dmc_pwr_ctrl;
	unsigned char	res106[0x4];
	unsigned int	drex2_pause;
	unsigned char	res107[0xe0];
	unsigned int	clkout_cmu_cdrex;
	unsigned int	clkout_cmu_cdrex_div_stat;
	unsigned char	res108[0x8];
	unsigned int	lpddr3phy_ctrl;
	unsigned char	res109a[0xc];
	unsigned int	lpddr3phy_con3;
	unsigned int	pll_div2_sel;
	unsigned char	res109b[0xf5e4];
};

struct exynos5_mct_regs {
	uint32_t	mct_cfg;
	uint8_t		reserved0[0xfc];
	uint32_t	g_cnt_l;
	uint32_t	g_cnt_u;
	uint8_t		reserved1[0x8];
	uint32_t	g_cnt_wstat;
	uint8_t		reserved2[0xec];
	uint32_t	g_comp0_l;
	uint32_t	g_comp0_u;
	uint32_t	g_comp0_addr_incr;
	uint8_t		reserved3[0x4];
	uint32_t	g_comp1_l;
	uint32_t	g_comp1_u;
	uint32_t	g_comp1_addr_incr;
	uint8_t		reserved4[0x4];
	uint32_t	g_comp2_l;
	uint32_t	g_comp2_u;
	uint32_t	g_comp2_addr_incr;
	uint8_t		reserved5[0x4];
	uint32_t	g_comp3_l;
	uint32_t	g_comp3_u;
	uint32_t	g_comp3_addr_incr;
	uint8_t		reserved6[0x4];
	uint32_t	g_tcon;
	uint32_t	g_int_cstat;
	uint32_t	g_int_enb;
	uint32_t	g_wstat;
	uint8_t		reserved7[0xb0];
	uint32_t	l0_tcntb;
	uint32_t	l0_tcnto;
	uint32_t	l0_icntb;
	uint32_t	l0_icnto;
	uint32_t	l0_frcntb;
	uint32_t	l0_frcnto;
	uint8_t		reserved8[0x8];
	uint32_t	l0_tcon;
	uint8_t		reserved9[0xc];
	uint32_t	l0_int_cstat;
	uint32_t	l0_int_enb;
	uint8_t		reserved10[0x8];
	uint32_t	l0_wstat;
	uint8_t		reserved11[0xbc];
	uint32_t	l1_tcntb;
	uint32_t	l1_tcnto;
	uint32_t	l1_icntb;
	uint32_t	l1_icnto;
	uint32_t	l1_frcntb;
	uint32_t	l1_frcnto;
	uint8_t		reserved12[0x8];
	uint32_t	l1_tcon;
	uint8_t		reserved13[0xc];
	uint32_t	l1_int_cstat;
	uint32_t	l1_int_enb;
	uint8_t		reserved14[0x8];
	uint32_t	l1_wstat;
};

#define EXYNOS5_EPLLCON0_LOCKED_SHIFT	29  /* EPLL Locked bit position*/
#define EPLL_SRC_CLOCK			24000000  /*24 MHz Crystal Input */
#define TIMEOUT_EPLL_LOCK		1000

#define AUDIO_0_RATIO_MASK		0x0f
#define AUDIO_1_RATIO_MASK		0x0f

#define CLK_SRC_PERIC1			0x254
#define AUDIO1_SEL_MASK			0xf
#define CLK_SRC_AUDIOCDCLK1		0x0
#define CLK_SRC_XXTI			0x1
#define CLK_SRC_SCLK_EPLL		0x7

/* CON0 bit-fields */
#define EPLL_CON0_MDIV_MASK		0x1ff
#define EPLL_CON0_PDIV_MASK		0x3f
#define EPLL_CON0_SDIV_MASK		0x7
#define EPLL_CON0_LOCKED_SHIFT		29
#define EPLL_CON0_MDIV_SHIFT		16
#define EPLL_CON0_PDIV_SHIFT		8
#define EPLL_CON0_SDIV_SHIFT		0
#define EPLL_CON0_LOCK_DET_EN_SHIFT	28
#define EPLL_CON0_LOCK_DET_EN_MASK	1

/* structure for epll configuration used in audio clock configuration */
struct st_epll_con_val {
	unsigned int freq_out;		/* frequency out */
	unsigned int en_lock_det;	/* enable lock detect */
	unsigned int m_div;		/* m divider value */
	unsigned int p_div;		/* p divider value */
	unsigned int s_div;		/* s divider value */
	unsigned int k_dsm;		/* k value of delta signal modulator */
};

/**
 * Low-level function to set the clock pre-ratio for a peripheral
 *
 * @param periph_id	Peripheral ID of peripheral to change
 * @param divisor	New divisor for this peripheral's clock
 */
void clock_ll_set_pre_ratio(enum periph_id periph_id, unsigned divisor);

/**
 * Low-level function to set the clock ratio for a peripheral
 *
 * @param periph_id	Peripheral ID of peripheral to change
 * @param divisor	New divisor for this peripheral's clock
 */
void clock_ll_set_ratio(enum periph_id periph_id, unsigned divisor);

/**
 * Low-level function that selects the best clock scalars for a given rate and
 * sets up the given peripheral's clock accordingly.
 *
 * @param periph_id	Peripheral ID of peripheral to change
 * @param rate		Desired clock rate in Hz
 *
 * @return zero on success, negative on error
 */
int clock_set_rate(enum periph_id periph_id, unsigned int rate);

/* Clock gate unused IP */
void clock_gate(void);

void mct_start(void);
uint64_t mct_raw_value(void);

#include "dmc.h"

/* These are the ratio's for configuring ARM clock */
struct arm_clk_ratios {
	unsigned int arm_freq_mhz;	/* Frequency of ARM core in MHz */

	unsigned int apll_mdiv;
	unsigned int apll_pdiv;
	unsigned int apll_sdiv;

	unsigned int arm2_ratio;
	unsigned int apll_ratio;
	unsigned int pclk_dbg_ratio;
	unsigned int atb_ratio;
	unsigned int periph_ratio;
	unsigned int acp_ratio;
	unsigned int cpud_ratio;
	unsigned int arm_ratio;
};

/**
 * Get the clock ratios for CPU configuration
 *
 * @return pointer to the clock ratios that we should use
 */
struct arm_clk_ratios *get_arm_clk_ratios(void);

/*
 * Initialize clock for the device
 */
struct mem_timings;
void system_clock_init(struct mem_timings *mem,
		struct arm_clk_ratios *arm_clk_ratio);

#endif
