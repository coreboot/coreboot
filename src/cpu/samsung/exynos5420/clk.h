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

#ifndef CPU_SAMSUNG_EXYNOS5420_CLK_H
#define CPU_SAMSUNG_EXYNOS5420_CLK_H

#include <stdint.h>

enum periph_id;

#define APLL	0
#define MPLL	1
#define EPLL	2
#define HPLL	3
#define VPLL	4
#define BPLL	5
#define RPLL	6
#define SPLL	7

enum pll_src_bit {
	EXYNOS_SRC_CPLL = 1,
	EXYNOS_SRC_DPLL = 2,
	EXYNOS_SRC_MPLL = 3,
	EXYNOS_SRC_SPLL = 4,
	EXYNOS_SRC_IPLL = 5,
	EXYNOS_SRC_EPLL = 6,
	EXYNOS_SRC_RPLL = 7,
};

/* *
 * This structure is to store the src bit, div bit and prediv bit
 * positions of the peripheral clocks of the src and div registers
 */
struct clk_bit_info {
	s8 src_bit;    /* offset in register to clock source field */
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
 * Set mshci controller instances clock drivder
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

struct exynos5420_clock {
	uint32_t	apll_lock;		/* 0x10010000 */
	uint8_t		res1[0xfc];
	uint32_t	apll_con0;
	uint32_t	apll_con1;
	uint8_t		res2[0xf8];
	uint32_t	clk_src_cpu;
	uint8_t		res3[0x1fc];
	uint32_t	clk_mux_stat_cpu;
	uint8_t		res4[0xfc];
	uint32_t	clk_div_cpu0;		/* 0x10010500 */
	uint32_t	clk_div_cpu1;
	uint8_t		res5[0xf8];
	uint32_t	clk_div_stat_cpu0;
	uint32_t	clk_div_stat_cpu1;
	uint8_t		res6[0xf8];
	uint32_t	clk_gate_bus_cpu;
	uint8_t		res7[0xfc];
	uint32_t	clk_gate_sclk_cpu;
	uint8_t		res8[0x1fc];
	uint32_t	clkout_cmu_cpu;		/* 0x10010a00 */
	uint32_t	clkout_cmu_cpu_div_stat;
	uint8_t		res9[0x5f8];
	uint32_t	armclk_stopctrl;
	uint8_t		res10[0x4];
	uint32_t	arm_ema_ctrl;
	uint32_t	arm_ema_status;
	uint8_t		res11[0x10];
	uint32_t	pwr_ctrl;
	uint32_t	pwr_ctrl2;
	uint8_t		res12[0xd8];
	uint32_t	apll_con0_l8;		/* 0x1001100 */
	uint32_t	apll_con0_l7;
	uint32_t	apll_con0_l6;
	uint32_t	apll_con0_l5;
	uint32_t	apll_con0_l4;
	uint32_t	apll_con0_l3;
	uint32_t	apll_con0_l2;
	uint32_t	apll_con0_l1;
	uint32_t	iem_control;
	uint8_t		res13[0xdc];
	uint32_t	apll_con1_l8;		/* 0x10011200 */
	uint32_t	apll_con1_l7;
	uint32_t	apll_con1_l6;
	uint32_t	apll_con1_l5;
	uint32_t	apll_con1_l4;
	uint32_t	apll_con1_l3;
	uint32_t	apll_con1_l2;
	uint32_t	apll_con1_l1;
	uint8_t		res14[0xe0];
	uint32_t	clkdiv_iem_l8;
	uint32_t	clkdiv_iem_l7;		/* 0x10011304 */
	uint32_t	clkdiv_iem_l6;
	uint32_t	clkdiv_iem_l5;
	uint32_t	clkdiv_iem_l4;
	uint32_t	clkdiv_iem_l3;
	uint32_t	clkdiv_iem_l2;
	uint32_t	clkdiv_iem_l1;
	uint8_t		res15[0xe0];
	uint32_t	l2_status;
	uint8_t		res16[0x0c];
	uint32_t	cpu_status;		/* 0x10011410 */
	uint8_t		res17[0x0c];
	uint32_t	ptm_status;
	uint8_t		res18[0xbdc];
	uint32_t	cmu_cpu_spare0;
	uint32_t	cmu_cpu_spare1;
	uint32_t	cmu_cpu_spare2;
	uint32_t	cmu_cpu_spare3;
	uint32_t	cmu_cpu_spare4;
	uint8_t		res19[0x1fdc];
	uint32_t	cmu_cpu_version;
	uint8_t		res20[0x20c];
	uint32_t	clk_src_cperi0;		/* 0x10014200 */
	uint32_t	clk_src_cperi1;
	uint8_t		res21[0xf8];
	uint32_t	clk_src_mask_cperi;
	uint8_t		res22[0x100];
	uint32_t	clk_mux_stat_cperi1;
	uint8_t		res23[0xfc];
	uint32_t	clk_div_cperi1;
	uint8_t		res24[0xfc];
	uint32_t	clk_div_stat_cperi1;
	uint8_t		res25[0xf8];
	uint32_t	clk_gate_bus_cperi0;	/* 0x10014700 */
	uint32_t	clk_gate_bus_cperi1;
	uint8_t		res26[0xf8];
	uint32_t	clk_gate_sclk_cperi;
	uint8_t		res27[0xfc];
	uint32_t	clk_gate_ip_cperi;
	uint8_t		res28[0xfc];
	uint32_t	clkout_cmu_cperi;
	uint32_t	clkout_cmu_cperi_div_stat;
	uint8_t		res29[0x5f8];
	uint32_t	dcgidx_map0;		/* 0x10015000 */
	uint32_t	dcgidx_map1;
	uint32_t	dcgidx_map2;
	uint8_t		res30[0x14];
	uint32_t	dcgperf_map0;
	uint32_t	dcgperf_map1;
	uint8_t		res31[0x18];
	uint32_t	dvcidx_map;
	uint8_t		res32[0x1c];
	uint32_t	freq_cpu;
	uint32_t	freq_dpm;
	uint8_t		res33[0x18];
	uint32_t	dvsemclk_en;		/* 0x10015080 */
	uint32_t	maxperf;
	uint8_t		res34[0x2e78];
	uint32_t	cmu_cperi_spare0;
	uint32_t	cmu_cperi_spare1;
	uint32_t	cmu_cperi_spare2;
	uint32_t	cmu_cperi_spare3;
	uint32_t	cmu_cperi_spare4;
	uint32_t	cmu_cperi_spare5;
	uint32_t	cmu_cperi_spare6;
	uint32_t	cmu_cperi_spare7;
	uint32_t	cmu_cperi_spare8;
	uint8_t		res35[0xcc];
	uint32_t	cmu_cperi_version;	/* 0x10017ff0 */
	uint8_t		res36[0x50c];
	uint32_t	clk_div_g2d;
	uint8_t		res37[0xfc];
	uint32_t	clk_div_stat_g2d;
	uint8_t		res38[0xfc];
	uint32_t	clk_gate_bus_g2d;
	uint8_t		res39[0xfc];
	uint32_t	clk_gate_ip_g2d;
	uint8_t		res40[0x1fc];
	uint32_t	clkout_cmu_g2d;
	uint32_t	clkout_cmu_g2d_div_stat;/* 0x10018a04 */
	uint8_t		res41[0xf8];
	uint32_t	cmu_g2d_spare0;
	uint32_t	cmu_g2d_spare1;
	uint32_t	cmu_g2d_spare2;
	uint32_t	cmu_g2d_spare3;
	uint32_t	cmu_g2d_spare4;
	uint8_t		res42[0x34dc];
	uint32_t	cmu_g2d_version;
	uint8_t		res43[0x30c];
	uint32_t	clk_div_cmu_isp0;
	uint32_t	clk_div_cmu_isp1;
	uint32_t	clk_div_isp2;		/* 0x1001c308 */
	uint8_t		res44[0xf4];
	uint32_t	clk_div_stat_cmu_isp0;
	uint32_t	clk_div_stat_cmu_isp1;
	uint32_t	clk_div_stat_isp2;
	uint8_t		res45[0x2f4];
	uint32_t	clk_gate_bus_isp0;
	uint32_t	clk_gate_bus_isp1;
	uint32_t	clk_gate_bus_isp2;
	uint32_t	clk_gate_bus_isp3;
	uint8_t		res46[0xf0];
	uint32_t	clk_gate_ip_isp0;
	uint32_t	clk_gate_ip_isp1;
	uint8_t		res47[0xf8];
	uint32_t	clk_gate_sclk_isp;
	uint8_t		res48[0x0c];
	uint32_t	mcuisp_pwr_ctrl;	/* 0x1001c910 */
	uint8_t		res49[0x0ec];
	uint32_t	clkout_cmu_isp;
	uint32_t	clkout_cmu_isp_div_stat;
	uint8_t		res50[0xf8];
	uint32_t	cmu_isp_spare0;
	uint32_t	cmu_isp_spare1;
	uint32_t	cmu_isp_spare2;
	uint32_t	cmu_isp_spare3;
	uint8_t		res51[0x34e0];
	uint32_t	cmu_isp_version;
	uint8_t		res52[0x2c];
	uint32_t	cpll_lock;		/* 10020020 */
	uint8_t		res53[0xc];
	uint32_t	dpll_lock;
	uint8_t		res54[0xc];
	uint32_t	epll_lock;
	uint8_t		res55[0xc];
	uint32_t	rpll_lock;
	uint8_t		res56[0xc];
	uint32_t	ipll_lock;
	uint8_t		res57[0xc];
	uint32_t	spll_lock;
	uint8_t		res58[0xc];
	uint32_t	vpll_lock;
	uint8_t		res59[0xc];
	uint32_t	mpll_lock;
	uint8_t		res60[0x8c];
	uint32_t	cpll_con0;		/* 10020120 */
	uint32_t	cpll_con1;
	uint32_t	dpll_con0;
	uint32_t	dpll_con1;
	uint32_t	epll_con0;
	uint32_t	epll_con1;
	uint32_t	epll_con2;
	uint8_t		res601[0x4];
	uint32_t	rpll_con0;
	uint32_t	rpll_con1;
	uint32_t	rpll_con2;
	uint8_t		res602[0x4];
	uint32_t	ipll_con0;
	uint32_t	ipll_con1;
	uint8_t		res61[0x8];
	uint32_t	spll_con0;
	uint32_t	spll_con1;
	uint8_t		res62[0x8];
	uint32_t	vpll_con0;
	uint32_t	vpll_con1;
	uint8_t		res63[0x8];
	uint32_t	mpll_con0;
	uint32_t	mpll_con1;
	uint8_t		res64[0x78];
	uint32_t	clk_src_top0;		/* 0x10020200 */
	uint32_t	clk_src_top1;
	uint32_t	clk_src_top2;
	uint32_t	clk_src_top3;
	uint32_t	clk_src_top4;
	uint32_t	clk_src_top5;
	uint32_t	clk_src_top6;
	uint32_t	clk_src_top7;
	uint8_t		res65[0xc];
	uint32_t	clk_src_disp10;		/* 0x1002022c */
	uint8_t		res66[0x10];
	uint32_t	clk_src_mau;
	uint32_t	clk_src_fsys;
	uint8_t		res67[0x8];
	uint32_t	clk_src_peric0;
	uint32_t	clk_src_peric1;
	uint8_t		res68[0x18];
	uint32_t	clk_src_isp;
	uint8_t		res69[0x0c];
	uint32_t	clk_src_top10;
	uint32_t	clk_src_top11;
	uint32_t	clk_src_top12;
	uint8_t		res70[0x74];
	uint32_t	clk_src_mask_top0;
	uint32_t	clk_src_mask_top1;
	uint32_t	clk_src_mask_top2;
	uint8_t		res71[0x10];
	uint32_t	clk_src_mask_top7;
	uint8_t		res72[0xc];
	uint32_t	clk_src_mask_disp10;	/* 0x1002032c */
	uint8_t		res73[0x4];
	uint32_t	clk_src_mask_mau;
	uint8_t		res74[0x8];
	uint32_t	clk_src_mask_fsys;
	uint8_t		res75[0xc];
	uint32_t	clk_src_mask_peric0;
	uint32_t	clk_src_mask_peric1;
	uint8_t		res76[0x18];
	uint32_t	clk_src_mask_isp;
	uint8_t		res77[0x8c];
	uint32_t	clk_mux_stat_top0;	/* 0x10020400 */
	uint32_t	clk_mux_stat_top1;
	uint32_t	clk_mux_stat_top2;
	uint32_t	clk_mux_stat_top3;
	uint32_t	clk_mux_stat_top4;
	uint32_t	clk_mux_stat_top5;
	uint32_t	clk_mux_stat_top6;
	uint32_t	clk_mux_stat_top7;
	uint8_t		res78[0x60];
	uint32_t	clk_mux_stat_top10;
	uint32_t	clk_mux_stat_top11;
	uint32_t	clk_mux_stat_top12;
	uint8_t		res79[0x74];
	uint32_t	clk_div_top0;		/* 0x10020500 */
	uint32_t	clk_div_top1;
	uint32_t	clk_div_top2;
	uint8_t		res80[0x20];
	uint32_t	clk_div_disp10;
	uint8_t		res81[0x14];
	uint32_t	clk_div_mau;
	uint32_t	clk_div_fsys0;
	uint32_t	clk_div_fsys1;
	uint32_t	clk_div_fsys2;
	uint8_t		res82[0x4];
	uint32_t	clk_div_peric0;
	uint32_t	clk_div_peric1;
	uint32_t	clk_div_peric2;
	uint32_t	clk_div_peric3;
	uint32_t	clk_div_peric4;		/* 0x10020568 */
	uint8_t		res83[0x14];
	uint32_t	clk_div_isp0;
	uint32_t	clk_div_isp1;
	uint8_t		res84[0x8];
	uint32_t	clkdiv2_ratio;
	uint8_t		res850[0xc];
	uint32_t	clkdiv4_ratio;
	uint8_t		res85[0x5c];
	uint32_t	clk_div_stat_top0;
	uint32_t	clk_div_stat_top1;
	uint32_t	clk_div_stat_top2;
	uint8_t		res86[0x20];
	uint32_t	clk_div_stat_disp10;
	uint8_t		res87[0x14];
	uint32_t	clk_div_stat_mau;	/* 0x10020644 */
	uint32_t	clk_div_stat_fsys0;
	uint32_t	clk_div_stat_fsys1;
	uint32_t	clk_div_stat_fsys2;
	uint8_t		res88[0x4];
	uint32_t	clk_div_stat_peric0;
	uint32_t	clk_div_stat_peric1;
	uint32_t	clk_div_stat_peric2;
	uint32_t	clk_div_stat_peric3;
	uint32_t	clk_div_stat_peric4;
	uint8_t		res89[0x14];
	uint32_t	clk_div_stat_isp0;
	uint32_t	clk_div_stat_isp1;
	uint8_t		res90[0x8];
	uint32_t	clkdiv2_stat0;
	uint8_t		res91[0xc];
	uint32_t	clkdiv4_stat;
	uint8_t		res92[0x5c];
	uint32_t	clk_gate_bus_top;	/* 0x10020700 */
	uint8_t		res93[0xc];
	uint32_t	clk_gate_bus_gscl0;
	uint8_t		res94[0xc];
	uint32_t	clk_gate_bus_gscl1;
	uint8_t		res95[0x4];
	uint32_t	clk_gate_bus_disp1;
	uint8_t		res96[0x4];
	uint32_t	clk_gate_bus_wcore;
	uint32_t	clk_gate_bus_mfc;
	uint32_t	clk_gate_bus_g3d;
	uint32_t	clk_gate_bus_gen;
	uint32_t	clk_gate_bus_fsys0;
	uint32_t	clk_gate_bus_fsys1;
	uint32_t	clk_gate_bus_fsys2;
	uint32_t	clk_gate_bus_mscl;
	uint32_t	clk_gate_bus_peric;
	uint32_t	clk_gate_bus_peric1;
	uint8_t		res97[0x8];
	uint32_t	clk_gate_bus_peris0;
	uint32_t	clk_gate_bus_peris1;	/* 0x10020764 */
	uint8_t		res98[0x8];
	uint32_t	clk_gate_bus_noc;
	uint8_t		res99[0xac];
	uint32_t	clk_gate_top_sclk_gscl;
	uint8_t		res1000[0x4];
	uint32_t	clk_gate_top_sclk_disp1;
	uint8_t		res100[0x10];
	uint32_t	clk_gate_top_sclk_mau;
	uint32_t	clk_gate_top_sclk_fsys;
	uint8_t		res101[0xc];
	uint32_t	clk_gate_top_sclk_peric;
	uint8_t		res102[0xc];
	uint32_t	clk_gate_top_sclk_cperi;
	uint8_t		res103[0xc];
	uint32_t	clk_gate_top_sclk_isp;
	uint8_t		res104[0x9c];
	uint32_t	clk_gate_ip_gscl0;
	uint8_t		res105[0xc];
	uint32_t	clk_gate_ip_gscl1;
	uint8_t		res106[0x4];
	uint32_t	clk_gate_ip_disp1;
	uint32_t	clk_gate_ip_mfc;
	uint32_t	clk_gate_ip_g3d;
	uint32_t	clk_gate_ip_gen;	/* 0x10020934 */
	uint8_t		res107[0xc];
	uint32_t	clk_gate_ip_fsys;
	uint8_t		res108[0x8];
	uint32_t	clk_gate_ip_peric;
	uint8_t		res109[0xc];
	uint32_t	clk_gate_ip_peris;
	uint8_t		res110[0xc];
	uint32_t	clk_gate_ip_mscl;
	uint8_t		res111[0xc];
	uint32_t	clk_gate_ip_block;
	uint8_t		res112[0xc];
	uint32_t	bypass;
	uint8_t		res113[0x6c];
	uint32_t	clkout_cmu_top;
	uint32_t	clkout_cmu_top_div_stat;
	uint8_t		res114[0xf8];
	uint32_t	clkout_top_spare0;
	uint32_t	clkout_top_spare1;
	uint32_t	clkout_top_spare2;
	uint32_t	clkout_top_spare3;
	uint8_t		res115[0x34e0];
	uint32_t	clkout_top_version;
	uint8_t		res116[0xc01c];
	uint32_t	bpll_lock;		/* 0x10030010 */
	uint8_t		res117[0xfc];
	uint32_t	bpll_con0;
	uint32_t	bpll_con1;
	uint8_t		res118[0xe8];
	uint32_t	clk_src_cdrex;
	uint8_t		res119[0x1fc];
	uint32_t	clk_mux_stat_cdrex;
	uint8_t		res120[0xfc];
	uint32_t	clk_div_cdrex0;
	uint32_t	clk_div_cdrex1;
	uint8_t		res121[0xf8];
	uint32_t	clk_div_stat_cdrex;
	uint8_t		res1211[0xfc];
	uint32_t	clk_gate_bus_cdrex;
	uint32_t	clk_gate_bus_cdrex1;
	uint8_t		res122[0x1f8];
	uint32_t	clk_gate_ip_cdrex;
	uint8_t		res123[0x10];
	uint32_t	dmc_freq_ctrl;		/* 0x10030914 */
	uint8_t		res124[0x4];
	uint32_t	pause;
	uint32_t	ddrphy_lock_ctrl;
	uint8_t		res125[0xdc];
	uint32_t	clkout_cmu_cdrex;
	uint32_t	clkout_cmu_cdrex_div_stat;
	uint8_t		res126[0x8];
	uint32_t	lpddr3phy_ctrl;
	uint32_t	lpddr3phy_con0;
	uint32_t	lpddr3phy_con1;
	uint32_t	lpddr3phy_con2;
	uint32_t	lpddr3phy_con3;
	uint32_t	lpddr3phy_con4;
	uint32_t	lpddr3phy_con5;		/* 0x10030a28 */
	uint32_t	pll_div2_sel;
	uint8_t		res127[0xd0];
	uint32_t	cmu_cdrex_spare0;
	uint32_t	cmu_cdrex_spare1;
	uint32_t	cmu_cdrex_spare2;
	uint32_t	cmu_cdrex_spare3;
	uint32_t	cmu_cdrex_spare4;
	uint8_t		res128[0x34dc];
	uint32_t	cmu_cdrex_version;	/* 0x10033ff0 */
	uint8_t		res129[0x400c];
	uint32_t	kpll_lock;
	uint8_t		res130[0xfc];
	uint32_t	kpll_con0;
	uint32_t	kpll_con1;
	uint8_t		res131[0xf8];
	uint32_t	clk_src_kfc;
	uint8_t		res132[0x1fc];
	uint32_t	clk_mux_stat_kfc;	/* 0x10038400 */
	uint8_t		res133[0xfc];
	uint32_t	clk_div_kfc0;
	uint8_t		res134[0xfc];
	uint32_t	clk_div_stat_kfc0;
	uint8_t		res135[0xfc];
	uint32_t	clk_gate_bus_cpu_kfc;
	uint8_t		res136[0xfc];
	uint32_t	clk_gate_sclk_cpu_kfc;
	uint8_t		res137[0x1fc];
	uint32_t	clkout_cmu_kfc;
	uint32_t	clkout_cmu_kfc_div_stat;/* 0x10038a04 */
	uint8_t		res138[0x5f8];
	uint32_t	armclk_stopctrl_kfc;
	uint8_t		res139[0x4];
	uint32_t	armclk_ema_ctrl_kfc;
	uint32_t	armclk_ema_status_kfc;
	uint8_t		res140[0x10];
	uint32_t	pwr_ctrl_kfc;
	uint32_t	pwr_ctrl2_kfc;
	uint8_t		res141[0xd8];
	uint32_t	kpll_con0_l8;
	uint32_t	kpll_con0_l7;
	uint32_t	kpll_con0_l6;
	uint32_t	kpll_con0_l5;
	uint32_t	kpll_con0_l4;
	uint32_t	kpll_con0_l3;
	uint32_t	kpll_con0_l2;
	uint32_t	kpll_con0_l1;
	uint32_t	iem_control_kfc;	/* 0x10039120 */
	uint8_t		res142[0xdc];
	uint32_t	kpll_con1_l8;
	uint32_t	kpll_con1_l7;
	uint32_t	kpll_con1_l6;
	uint32_t	kpll_con1_l5;
	uint32_t	kpll_con1_l4;
	uint32_t	kpll_con1_l3;
	uint32_t	kpll_con1_l2;
	uint32_t	kpll_con1_l1;
	uint8_t		res143[0xe0];
	uint32_t	clkdiv_iem_l8_kfc;	/* 0x10039300 */
	uint32_t	clkdiv_iem_l7_kfc;
	uint32_t	clkdiv_iem_l6_kfc;
	uint32_t	clkdiv_iem_l5_kfc;
	uint32_t	clkdiv_iem_l4_kfc;
	uint32_t	clkdiv_iem_l3_kfc;
	uint32_t	clkdiv_iem_l2_kfc;
	uint32_t	clkdiv_iem_l1_kfc;
	uint8_t		res144[0xe0];
	uint32_t	l2_status_kfc;
	uint8_t		res145[0xc];
	uint32_t	cpu_status_kfc;		/* 0x10039410 */
	uint8_t		res146[0xc];
	uint32_t	ptm_status_kfc;
	uint8_t		res147[0xbdc];
	uint32_t	cmu_kfc_spare0;
	uint32_t	cmu_kfc_spare1;
	uint32_t	cmu_kfc_spare2;
	uint32_t	cmu_kfc_spare3;
	uint32_t	cmu_kfc_spare4;
	uint8_t		res148[0x1fdc];
	uint32_t	cmu_kfc_version;	/* 0x1003bff0 */
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
#define EPLL_SRC_CLOCK			24000000  /*24 MHz Cristal Input */
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
void system_clock_init(void);

#endif
