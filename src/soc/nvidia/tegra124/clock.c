/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <console/console.h>
#include <delay.h>
#include <arch/io.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include "clk_rst.h"
#include "cpug.h"
#include "flow.h"
#include "pmc.h"

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;
static struct flow_ctlr *flow = (void *)TEGRA_FLOW_BASE;
static struct tegra_pmc_regs *pmc = (void*)TEGRA_PMC_BASE;

struct pll_dividers {
	u32	n : 10;
	u32	m : 8;
	u32	p : 4;
	u32	cpcon: 4;
	u32	: 6;
};

/* Some PLLs have more restrictive divider bit lengths or are missing some
 * fields. Make sure to use the right struct in the osc_table definition to get
 * compile-time checking, but keep the bits aligned with struct pll_dividers so
 * they can be used interchangeably at run time. Add new formats as required. */
struct pllcx_dividers {
	u32	n : 8;
	u32	: 2;
	u32	m : 8;
	u32	p : 4;
	u32	: 10;
};
struct pllpad_dividers {
	u32	n : 10;
	u32	m : 5;
	u32	: 3;
	u32	p : 3;
	u32	: 1;
	u32	cpcon : 4;
	u32	: 6;
};
struct pllu_dividers {
	u32	n : 10;
	u32	m : 5;
	u32	: 3;
	u32	p : 1;
	u32	: 3;
	u32	cpcon : 4;
	u32	: 6;
};

union __attribute__((transparent_union)) pll_fields {
	u32 raw;
	struct pll_dividers div;
	struct pllcx_dividers cx;
	struct pllpad_dividers pad;
	struct pllu_dividers u;
};

/* This table defines the frequency dividers for every PLL to turn the external
 * OSC clock into the frequencies defined by TEGRA_PLL*_KHZ in soc/clock.h.
 * All PLLs have three dividers (N, M and P), with the governing formula for
 * the output frequency being OUT = (IN / m) * N / (2^P). */
struct {
	int khz;
	struct pllcx_dividers	pllx;	/* target: 1900 MHz */
	struct pllpad_dividers	pllp;	/* target:  408 MHz */
	struct pllcx_dividers	pllc;	/* target:  600 MHz */
	struct pllpad_dividers	plld;	/* target:  925 MHz */
	struct pllu_dividers	pllu;	/* target;  960 MHz */
} static const osc_table[16] = {
	[OSC_FREQ_OSC12]{
		.khz = 12000,
		.pllx = {.n = 158, .m =  1, .p = 0},		  /* 1896 MHz */
		.pllp = {.n =  34, .m =  1, .p = 0, .cpcon = 2},
		.pllc = {.n =  50, .m =  1, .p = 0},
		.plld = {.n = 925, .m = 12, .p = 0, .cpcon = 12},
		.pllu = {.n =  80, .m =  1, .p = 0, .cpcon = 3},
	},
	[OSC_FREQ_OSC13]{
		.khz = 13000,
		.pllx = {.n = 146, .m =  1, .p = 0},		  /* 1898 MHz */
		.pllp = {.n = 408, .m = 13, .p = 0, .cpcon = 8},
		.pllc = {.n = 231, .m =  5, .p = 0},		 /* 600.6 MHz */
		.plld = {.n = 925, .m = 13, .p = 0, .cpcon = 12},
		.pllu = {.n = 960, .m = 13, .p = 0, .cpcon = 12},
	},
	[OSC_FREQ_OSC16P8]{
		.khz = 16800,
		.pllx = {.n = 113, .m =  1, .p = 0},		/* 1898.4 MHz */
		.pllp = {.n = 170, .m =  7, .p = 0, .cpcon = 4},
		.pllc = {.n = 250, .m =  7, .p = 0},
		.plld = {.n = 936, .m = 17, .p = 0, .cpcon = 12},/* 924.9 MHz */
		.pllu = {.n = 400, .m =  7, .p = 0, .cpcon = 8},
	},
	[OSC_FREQ_OSC19P2]{
		.khz = 19200,
		.pllx = {.n =  98, .m =  1, .p = 0},		/* 1881.6 MHz */
		.pllp = {.n =  85, .m =  4, .p = 0, .cpcon = 3},
		.pllc = {.n = 125, .m =  4, .p = 0},
		.plld = {.n = 819, .m = 17, .p = 0, .cpcon = 12},/* 924.9 MHz */
		.pllu = {.n =  50, .m =  1, .p = 0, .cpcon = 2},
	},
	[OSC_FREQ_OSC26]{
		.khz = 26000,
		.pllx = {.n =  73, .m =  1, .p = 0},		  /* 1898 MHz */
		.pllp = {.n = 204, .m = 13, .p = 0, .cpcon = 5},
		.pllc = {.n =  23, .m =  1, .p = 0},		   /* 598 MHz */
		.plld = {.n = 925, .m = 26, .p = 0, .cpcon = 12},
		.pllu = {.n = 480, .m = 13, .p = 0, .cpcon = 8},
	},
	[OSC_FREQ_OSC38P4]{
		.khz = 38400,
		.pllx = {.n =  98, .m =  1, .p = 0},		/* 1881.6 MHz */
		.pllp = {.n =  85, .m =  4, .p = 0, .cpcon = 3},
		.pllc = {.n = 125, .m =  4, .p = 0},
		.plld = {.n = 819, .m = 17, .p = 0, .cpcon = 12},/* 924.9 MHz */
		.pllu = {.n =  50, .m =  1, .p = 0, .cpcon = 2},
	},
	[OSC_FREQ_OSC48]{
		.khz = 48000,
		.pllx = {.n = 158, .m =  1, .p = 0},		  /* 1896 MHz */
		.pllp = {.n =  24, .m =  1, .p = 0, .cpcon = 2},
		.pllc = {.n =  50, .m =  1, .p = 0},
		.plld = {.n = 925, .m = 12, .p = 0, .cpcon = 12},
		.pllu = {.n =  80, .m =  1, .p = 0, .cpcon = 3},
	},
};

void clock_ll_set_source_divisor(u32 *reg, u32 source, u32 divisor)
{
        u32 value;

        value = readl(reg);

        value &= ~CLK_SOURCE_MASK;
        value |= source << CLK_SOURCE_SHIFT;

        value &= ~CLK_DIVISOR_MASK;
        value |= divisor << CLK_DIVISOR_SHIFT;

        writel(value, reg);
}

/* Get the oscillator frequency, from the corresponding hardware
 * configuration field. This is actually a per-soc thing. Avoid the
 * temptation to make it common.
 */
static u32 clock_get_osc_bits(void)
{
	return readl(&clk_rst->osc_ctrl) >> OSC_CTRL_OSC_FREQ_SHIFT;
}

int clock_get_osc_khz(void)
{
	return osc_table[clock_get_osc_bits()].khz;
}

static void adjust_pllp_out_freqs(void)
{
	u32 reg;
	/* Set T30 PLLP_OUT1, 2, 3 & 4 freqs to 9.6, 48, 102 & 204MHz */
	reg = readl(&clk_rst->pllp_outa); // OUTA contains OUT2 / OUT1
	reg |= (IN_408_OUT_48_DIVISOR << PLLP_OUT2_RATIO) | PLLP_OUT2_OVR
		| (IN_408_OUT_9_6_DIVISOR << PLLP_OUT1_RATIO) | PLLP_OUT1_OVR;
	writel(reg, &clk_rst->pllp_outa);

	reg = readl(&clk_rst->pllp_outb);   // OUTB, contains OUT4 / OUT3
	reg |= (IN_408_OUT_204_DIVISOR << PLLP_OUT4_RATIO) | PLLP_OUT4_OVR
		| (IN_408_OUT_102_DIVISOR << PLLP_OUT3_RATIO) | PLLP_OUT3_OVR;
	writel(reg, &clk_rst->pllp_outb);
}

static void init_pll(u32 *base, u32 *misc, const union pll_fields pll)
{
	u32 dividers =  pll.div.n << PLL_BASE_DIVN_SHIFT |
			pll.div.m << PLL_BASE_DIVM_SHIFT |
			pll.div.p << PLL_BASE_DIVP_SHIFT;

	/* Write dividers but BYPASS the PLL while we're messing with it. */
	writel(dividers | PLL_BASE_BYPASS, base);

	/* Set CPCON field (defaults to 0 if it doesn't exist for this PLL) */
	writel(pll.div.cpcon << PLL_MISC_CPCON_SHIFT, misc);

	/* enable PLL and take it back out of BYPASS (we don't wait for lock
	 * because we assume that to be done by the time we start using it). */
	writel(dividers | PLL_BASE_ENABLE, base);
}

/* Initialize the UART and put it on CLK_M so we can use it during clock_init().
 * Will later move it to PLLP in clock_config(). The divisor must be very small
 * to accomodate 12KHz OSCs, so we override the 16.0 UART divider with the 15.1
 * CLK_SOURCE divider to get more precision. (This might still not be enough for
 * some OSCs... if you use 13KHz, be prepared to have a bad time.) The 1800 has
 * been determined through trial and error (must lead to div 13 at 24MHz). */
void clock_early_uart(void)
{
	clock_ll_set_source_divisor(&clk_rst->clk_src_uarta, 3,
		CLK_UART_DIV_OVERRIDE | CLK_DIVIDER(clock_get_osc_khz(), 1800));
	setbits_le32(&clk_rst->clk_out_enb_l, CLK_L_UARTA);
	udelay(2);
	clrbits_le32(&clk_rst->rst_dev_l, CLK_L_UARTA);
}

void clock_cpu0_config_and_reset(void *entry)
{
	void * const evp_cpu_reset = (uint8_t *)TEGRA_EVP_BASE + 0x100;

	write32(CONFIG_STACK_TOP, &cpug_stack_pointer);
	write32((uintptr_t)entry, &cpug_entry_point);
	write32((uintptr_t)&cpug_setup, evp_cpu_reset);

	// Set up cclk_brst and divider.
	write32((CRC_CCLK_BRST_POL_PLLX_OUT0 << 0) |
		(CRC_CCLK_BRST_POL_PLLX_OUT0 << 4) |
		(CRC_CCLK_BRST_POL_PLLX_OUT0 << 8) |
		(CRC_CCLK_BRST_POL_PLLX_OUT0 << 12) |
		(CRC_CCLK_BRST_POL_CPU_STATE_RUN << 28),
		&clk_rst->cclk_brst_pol);
	write32(CRC_SUPER_CCLK_DIVIDER_SUPER_CDIV_ENB,
		&clk_rst->super_cclk_div);

	// Enable the clocks for CPUs 0-3.
	uint32_t cpu_cmplx_clr = read32(&clk_rst->clk_cpu_cmplx_clr);
	cpu_cmplx_clr |= CRC_CLK_CLR_CPU0_STP | CRC_CLK_CLR_CPU1_STP |
			 CRC_CLK_CLR_CPU2_STP | CRC_CLK_CLR_CPU3_STP;
	write32(cpu_cmplx_clr, &clk_rst->clk_cpu_cmplx_clr);

	// Enable other CPU related clocks.
	setbits_le32(&clk_rst->clk_out_enb_l, CLK_L_CPU);
	setbits_le32(&clk_rst->clk_out_enb_v, CLK_V_CPUG);

	// Disable the reset on the non-CPU parts of the fast cluster.
	write32(CRC_RST_CPUG_CLR_NONCPU,
		&clk_rst->rst_cpug_cmplx_clr);
	// Disable the various resets on the CPUs.
	write32(CRC_RST_CPUG_CLR_CPU0 | CRC_RST_CPUG_CLR_CPU1 |
		CRC_RST_CPUG_CLR_CPU2 | CRC_RST_CPUG_CLR_CPU3 |
		CRC_RST_CPUG_CLR_DBG0 | CRC_RST_CPUG_CLR_DBG1 |
		CRC_RST_CPUG_CLR_DBG2 | CRC_RST_CPUG_CLR_DBG3 |
		CRC_RST_CPUG_CLR_CORE0 | CRC_RST_CPUG_CLR_CORE1 |
		CRC_RST_CPUG_CLR_CORE2 | CRC_RST_CPUG_CLR_CORE3 |
		CRC_RST_CPUG_CLR_CX0 | CRC_RST_CPUG_CLR_CX1 |
		CRC_RST_CPUG_CLR_CX2 | CRC_RST_CPUG_CLR_CX3 |
		CRC_RST_CPUG_CLR_L2 | CRC_RST_CPUG_CLR_PDBG,
		&clk_rst->rst_cpug_cmplx_clr);
}

/**
 * The T124 requires some special clock initialization, including setting up
 * the DVC I2C, turning on MSELECT and selecting the G CPU cluster
 */
void clock_init(void)
{
	u32 val;
	u32 osc = clock_get_osc_bits();

	/*
	 * On poweron, AVP clock source (also called system clock) is set to
	 * PLLP_out0 with frequency set at 1MHz. Before initializing PLLP, we
	 * need to move the system clock's source to CLK_M temporarily. And
	 * then switch it to PLLP_out4 (204MHz) at a later time.
	 */
	val = (SCLK_SOURCE_CLKM << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT) |
		(SCLK_SOURCE_CLKM << SCLK_SWAKEUP_IRQ_SOURCE_SHIFT) |
		(SCLK_SOURCE_CLKM << SCLK_SWAKEUP_RUN_SOURCE_SHIFT) |
		(SCLK_SOURCE_CLKM << SCLK_SWAKEUP_IDLE_SOURCE_SHIFT) |
		(SCLK_SYS_STATE_RUN << SCLK_SYS_STATE_SHIFT);
	writel(val, &clk_rst->sclk_brst_pol);
	udelay(2);

	/* Set active CPU cluster to G */
	clrbits_le32(&flow->cluster_control, 1);

	/* Change the oscillator drive strength */
	val = readl(&clk_rst->osc_ctrl);
	val &= ~OSC_XOFS_MASK;
	val |= (OSC_DRIVE_STRENGTH << OSC_XOFS_SHIFT);
	writel(val, &clk_rst->osc_ctrl);

	/* Ambiguous quote from u-boot. TODO: what's this mean?
	 * "should update same value in PMC_OSC_EDPD_OVER XOFS
	   field for warmboot "*/
	val = readl(&pmc->osc_edpd_over);
	val &= ~PMC_OSC_EDPD_OVER_XOFS_MASK;
	val |= (OSC_DRIVE_STRENGTH << PMC_OSC_EDPD_OVER_XOFS_SHIFT);
	writel(val, &pmc->osc_edpd_over);

	/* Disable IDDQ for PLLX before we set it up (from U-Boot -- why?) */
	val = readl(&clk_rst->pllx_misc3);
	val &= ~PLLX_IDDQ_MASK;
	writel(val, &clk_rst->pllx_misc3);
	udelay(2);

	/* Set PLLC dynramp_step A to 0x2b and B to 0xb (from U-Boot -- why? */
	writel(0x2b << 17 | 0xb << 9, &clk_rst->pllc_misc2);

	adjust_pllp_out_freqs();

	init_pll(&clk_rst->pllx_base, &clk_rst->pllx_misc, osc_table[osc].pllx);
	init_pll(&clk_rst->pllp_base, &clk_rst->pllp_misc, osc_table[osc].pllp);
	init_pll(&clk_rst->pllc_base, &clk_rst->pllc_misc, osc_table[osc].pllc);
	init_pll(&clk_rst->plld_base, &clk_rst->plld_misc, osc_table[osc].plld);
	init_pll(&clk_rst->pllu_base, &clk_rst->pllu_misc, osc_table[osc].pllu);

	val = (1 << CLK_SYS_RATE_AHB_RATE_SHIFT);
	writel(val, &clk_rst->clk_sys_rate);
}

void clock_config(void)
{
	/* Enable clocks for the required peripherals. */
	/* TODO: can (should?) we use the _SET and _CLR registers here? */
	setbits_le32(&clk_rst->clk_out_enb_l,
		     CLK_L_CACHE2 | CLK_L_GPIO | CLK_L_TMR | CLK_L_I2C1 |
		     CLK_L_SDMMC4);
	setbits_le32(&clk_rst->clk_out_enb_h,
		     CLK_H_EMC | CLK_H_I2C2 | CLK_H_I2C5 | CLK_H_SBC1 |
		     CLK_H_PMC | CLK_H_APBDMA | CLK_H_MEM);
	setbits_le32(&clk_rst->clk_out_enb_u,
		     CLK_U_I2C3 | CLK_U_CSITE | CLK_U_SDMMC3);
	setbits_le32(&clk_rst->clk_out_enb_v, CLK_V_MSELECT);
	setbits_le32(&clk_rst->clk_out_enb_w, CLK_W_DVFS);

	/*
	 * Set MSELECT clock source as PLLP (00)_REG, and ask for a clock
	 * divider that would set the MSELECT clock at 102MHz for a
	 * PLLP base of 408MHz.
	 */
	clock_ll_set_source_divisor(&clk_rst->clk_src_mselect, 0,
		CLK_DIVIDER(TEGRA_PLLP_KHZ, 102000));

	/* Give clock time to stabilize */
	udelay(IO_STABILIZATION_DELAY);

	/* I2C1 gets CLK_M and a divisor of 17 */
	clock_ll_set_source_divisor(&clk_rst->clk_src_i2c1, 3, 16);
	/* I2C2 gets CLK_M and a divisor of 17 */
	clock_ll_set_source_divisor(&clk_rst->clk_src_i2c2, 3, 16);
	/* I2C3 (cam) gets CLK_M and a divisor of 17 */
	clock_ll_set_source_divisor(&clk_rst->clk_src_i2c3, 3, 16);
	/* I2C5 (PMU) gets CLK_M and a divisor of 17 */
	clock_ll_set_source_divisor(&clk_rst->clk_src_i2c5, 3, 16);

	/* UARTA gets PLLP, deactivate CLK_UART_DIV_OVERRIDE */
	writel(0 << CLK_SOURCE_SHIFT, &clk_rst->clk_src_uarta);

	/* Give clock time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);

	/* Take required peripherals out of reset. */

	clrbits_le32(&clk_rst->rst_dev_l,
		     CLK_L_CACHE2 | CLK_L_GPIO | CLK_L_TMR | CLK_L_I2C1 |
		     CLK_L_SDMMC4);
	clrbits_le32(&clk_rst->rst_dev_h,
		     CLK_H_EMC | CLK_H_I2C2 | CLK_H_I2C5 | CLK_H_SBC1 |
		     CLK_H_PMC | CLK_H_APBDMA | CLK_H_MEM);
	clrbits_le32(&clk_rst->rst_dev_u,
		     CLK_U_I2C3 | CLK_U_CSITE | CLK_U_SDMMC3);
	clrbits_le32(&clk_rst->rst_dev_v, CLK_V_MSELECT);
	clrbits_le32(&clk_rst->rst_dev_w, CLK_W_DVFS);
}
