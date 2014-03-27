/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 * Copyright 2014 Google Inc.
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
#include <stdlib.h>
#include "clk_rst.h"
#include "flow.h"
#include "maincpu.h"
#include "pmc.h"
#include "sysctr.h"

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;
static struct flow_ctlr *flow = (void *)TEGRA_FLOW_BASE;
static struct tegra_pmc_regs *pmc = (void *)TEGRA_PMC_BASE;
static struct sysctr_regs *sysctr = (void *)TEGRA_SYSCTR0_BASE;

struct pll_dividers {
	u32	n : 10;
	u32	m : 8;
	u32	p : 4;
	u32	cpcon : 4;
	u32	lfcon : 4;
	u32	: 2;
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
	u32	lfcon : 4;
	u32	: 2;
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
 * the output frequency being OUT = (IN / m) * N / (2^P).
 * Yes, it really is one equation with three unknowns ... */
struct {
	int khz;
	struct pllcx_dividers	pllx;	/* target:  CONFIG_PLLX_KHZ */
	struct pllcx_dividers	pllc;	/* target:  600 MHz */
	struct pllpad_dividers	plld;	/* target:  306 MHz */
	struct pllu_dividers	pllu;	/* target;  960 MHz */
	struct pllcx_dividers	plldp;	/* target;  270 MHz */
	/* Based on T124 TRM (to be updatd), PLLP is set to 408MHz in HW.
	 * Unless configuring PLLP to a frequency other than 408MHz,
	 * software configuration on PLLP is unneeded. */
} static const osc_table[16] = {
	[OSC_FREQ_OSC12]{
		.khz = 12000,
		.pllx = {.n = TEGRA_PLLX_KHZ / 12000, .m =  1, .p = 0},
		.pllc = {.n =  50, .m =  1, .p = 0},
		.plld = {.n = 306, .m = 12, .p = 0, .cpcon = 8},
		.pllu = {.n = 960, .m = 12, .p = 0, .cpcon = 12, .lfcon = 2},
		.plldp = {.n = 90, .m =  1, .p = 3},
	},
	[OSC_FREQ_OSC13]{
		.khz = 13000,
		.pllx = {.n = TEGRA_PLLX_KHZ / 13000, .m =  1, .p = 0},
		.pllc = {.n = 231, .m =  5, .p = 0},		/* 600.6 MHz */
		.plld = {.n = 306, .m = 13, .p = 0, .cpcon = 8},
		.pllu = {.n = 960, .m = 13, .p = 0, .cpcon = 12, .lfcon = 2},
		.plldp = {.n = 83, .m =  1, .p = 3},		/* 269.75 MHz */
	},
	[OSC_FREQ_OSC16P8]{
		.khz = 16800,
		.pllx = {.n = TEGRA_PLLX_KHZ / 16800, .m =  1, .p = 0},
		.pllc = {.n = 250, .m =  7, .p = 0},
		.plld = {.n = 309, .m = 17, .p = 0, .cpcon = 8}, /* 305.4 MHz*/
		.pllu = {.n = 400, .m =  7, .p = 0, .cpcon = 5, .lfcon = 2},
		.plldp = {.n = 64, .m =  1, .p = 3},		/* 268.8 MHz */
	},
	[OSC_FREQ_OSC19P2]{
		.khz = 19200,
		.pllx = {.n = TEGRA_PLLX_KHZ / 19200, .m =  1, .p = 0},
		.pllc = {.n = 125, .m =  4, .p = 0},
		.plld = {.n = 271, .m = 17, .p = 0, .cpcon = 8}, /* 306.1 MHz */
		.pllu = {.n = 200, .m =  4, .p = 0, .cpcon = 3, .lfcon = 2},
		.plldp = {.n = 56, .m =  1, .p = 3},		/* 270.75 MHz */
	},
	[OSC_FREQ_OSC26]{
		.khz = 26000,
		.pllx = {.n = TEGRA_PLLX_KHZ / 26000, .m =  1, .p = 0},
		.pllc = {.n =  23, .m =  1, .p = 0},		   /* 598 MHz */
		.plld = {.n = 306, .m = 26, .p = 0, .cpcon = 8},
		.pllu = {.n = 960, .m = 26, .p = 0, .cpcon = 12, .lfcon = 2},
		.plldp = {.n = 83, .m =  2, .p = 3},		/* 266.50 MHz */
	},
	[OSC_FREQ_OSC38P4]{
		.khz = 38400,
		/*
		 * There is a predivide by 2 before this PLL. Its values
		 * should match the 19.2MHz values.
		 */
		.pllx = {.n = TEGRA_PLLX_KHZ / 19200, .m =  1, .p = 0},
		.pllc = {.n = 125, .m =  4, .p = 0},
		.plld = {.n = 271, .m = 17, .p = 0, .cpcon = 8}, /* 306.1 MHz */
		.pllu = {.n = 200, .m =  4, .p = 0, .cpcon = 3, .lfcon = 2},
		.plldp = {.n = 56, .m =  2, .p = 3},		/* 268 MHz */
	},
	[OSC_FREQ_OSC48]{
		.khz = 48000,
		/*
		 * There is a predivide by 4 before this PLL. Its values
		 * should match the 12MHz values.
		 */
		.pllx = {.n = TEGRA_PLLX_KHZ / 12000, .m =  1, .p = 0},
		.pllc = {.n =  50, .m =  1, .p = 0},
		.plld = {.n = 306, .m = 12, .p = 0, .cpcon = 8},
		.pllu = {.n = 960, .m = 12, .p = 0, .cpcon = 12, .lfcon = 2},
		.plldp = {.n = 90, .m =  4, .p = 3},		/* 264 MHz */
	},
};

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

void clock_init_arm_generic_timer(void)
{
	uint32_t freq = clock_get_osc_khz() * 1000;
	// Set the cntfrq register.
	__asm__ __volatile__("mcr p15, 0, %0, c14, c0, 0\n" :: "r"(freq));

	// Record the system timer frequency.
	write32(freq, &sysctr->cntfid0);
	// Enable the system counter.
	uint32_t cntcr = read32(&sysctr->cntcr);
	cntcr |= SYSCTR_CNTCR_EN | SYSCTR_CNTCR_HDBG;
	write32(cntcr, &sysctr->cntcr);
}

#define SOR0_CLK_SEL0			(1 << 14)
#define SOR0_CLK_SEL1			(1 << 15)

void sor_clock_stop(void)
{
	/* The Serial Output Resource clock has to be off
	 * before we start the plldp. Learned the hard way.
	 * FIXME: this has to be cleaned up a bit more.
	 * Waiting on some new info from Nvidia.
	 */
	clrbits_le32(&clk_rst->clk_src_sor, SOR0_CLK_SEL0 | SOR0_CLK_SEL1);
}

void sor_clock_start(void)
{
	/* uses PLLP, has a non-standard bit layout. */
	setbits_le32(&clk_rst->clk_src_sor, SOR0_CLK_SEL0);
}

static void init_pll(u32 *base, u32 *misc, const union pll_fields pll, u32 lock)
{
	u32 dividers =  pll.div.n << PLL_BASE_DIVN_SHIFT |
			pll.div.m << PLL_BASE_DIVM_SHIFT |
			pll.div.p << PLL_BASE_DIVP_SHIFT;
	u32 misc_con = pll.div.cpcon << PLL_MISC_CPCON_SHIFT |
		       pll.div.lfcon << PLL_MISC_LFCON_SHIFT;

	/* Write dividers but BYPASS the PLL while we're messing with it. */
	writel(dividers | PLL_BASE_BYPASS, base);
	/*
	 * Set Lock bit, CPCON and LFCON fields (default to 0 if it doesn't
	 * exist for this PLL)
	 */
	writel(lock | misc_con, misc);

	/* Enable PLL and take it back out of BYPASS */
	writel(dividers | PLL_BASE_ENABLE, base);

	/* Wait for lock ready */
	while (!(readl(base) & PLL_BASE_LOCK));
}

static void init_utmip_pll(void)
{
	int khz = clock_get_osc_khz();

	/* Shut off PLL crystal clock while we mess with it */
	clrbits_le32(&clk_rst->utmip_pll_cfg2, 1 << 30); /* PHY_XTAL_CLKEN */
	udelay(1);

	write32(80 << 16 |			/* (rst) phy_divn */
		1 << 8 |			/* (rst) phy_divm */
		0, &clk_rst->utmip_pll_cfg0);	/* 960MHz * 1 / 80 == 12 MHz */

	write32(CEIL_DIV(khz, 8000) << 27 |	/* pllu_enbl_cnt / 8 (1us) */
		0 << 16 |			/* PLLU pwrdn */
		0 << 14 |			/* pll_enable pwrdn */
		0 << 12 |			/* pll_active pwrdn */
		CEIL_DIV(khz, 102) << 0 |	/* phy_stbl_cnt / 256 (2.5ms) */
		0, &clk_rst->utmip_pll_cfg1);

	/* TODO: TRM can't decide if actv is 5us or 10us, keep an eye on it */
	write32(0 << 24 |			/* SAMP_D/XDEV pwrdn */
		CEIL_DIV(khz, 3200) << 18 |	/* phy_actv_cnt / 16 (5us) */
		CEIL_DIV(khz, 256) << 6 |	/* pllu_stbl_cnt / 256 (1ms) */
		0 << 4 |			/* SAMP_C/USB3 pwrdn */
		0 << 2 |			/* SAMP_B/XHOST pwrdn */
		0 << 0 |			/* SAMP_A/USBD pwrdn */
		0, &clk_rst->utmip_pll_cfg2);

	setbits_le32(&clk_rst->utmip_pll_cfg2, 1 << 30); /* PHY_XTAL_CLKEN */
}

/* Graphics just has to be different. There's a few more bits we
 * need to set in here, but it makes sense just to restrict all the
 * special bits to this one function.
 */
static void graphics_pll(void)
{
	int osc = clock_get_osc_bits();
	u32 *cfg = &clk_rst->plldp_ss_cfg;
	/* the vendor code sets the dither bit (28)
	 * an undocumented bit (24)
	 * and clamp while we mess with it (22)
	 * Dither is pretty important to display port
	 * so we really do need to handle these bits.
	 * I'm not willing to not clamp it, even if
	 * it might "mostly work" with it not set,
	 * I don't want to find out in a few months
	 * that it is needed.
	 */
	u32 scfg = (1<<28) | (1<<24) | (1<<22);
	writel(scfg, cfg);
	init_pll(&clk_rst->plldp_base, &clk_rst->plldp_misc,
		osc_table[osc].plldp, PLLDPD2_MISC_LOCK_ENABLE);
	/* leave dither and undoc bits set, release clamp */
	scfg = (1<<28) | (1<<24);
	writel(scfg, cfg);

	/* Init clock source for disp1: plld (actually plld_out0) */
	init_pll(&clk_rst->plld_base, &clk_rst->plld_misc,
		osc_table[osc].plld,
		(PLLUD_MISC_LOCK_ENABLE | PLLD_MISC_CLK_ENABLE));
}

/* Initialize the UART and put it on CLK_M so we can use it during clock_init().
 * Will later move it to PLLP in clock_config(). The divisor must be very small
 * to accomodate 12KHz OSCs, so we override the 16.0 UART divider with the 15.1
 * CLK_SOURCE divider to get more precision. (This might still not be enough for
 * some OSCs... if you use 13KHz, be prepared to have a bad time.) The 1800 has
 * been determined through trial and error (must lead to div 13 at 24MHz). */
void clock_early_uart(void)
{
	write32(CLK_M << CLK_SOURCE_SHIFT | CLK_UART_DIV_OVERRIDE |
		CLK_DIVIDER(TEGRA_CLK_M_KHZ, 1800), &clk_rst->clk_src_uarta);
	setbits_le32(&clk_rst->clk_out_enb_l, CLK_L_UARTA);
	udelay(2);
	clrbits_le32(&clk_rst->rst_dev_l, CLK_L_UARTA);
}

/* Enable output clock (CLK1~3) for external peripherals. */
void clock_external_output(int clk_id)
{
	switch (clk_id) {
	case 1:
		setbits_le32(&pmc->clk_out_cntrl, 1 << 2);
		break;
	case 2:
		setbits_le32(&pmc->clk_out_cntrl, 1 << 10);
		break;
	case 3:
		setbits_le32(&pmc->clk_out_cntrl, 1 << 18);
		break;
	default:
		printk(BIOS_CRIT, "ERROR: Unknown output clock id %d\n",
		       clk_id);
		break;
	}
}

/* Start PLLM for SDRAM. */
void clock_sdram(u32 m, u32 n, u32 p, u32 setup, u32 ph45, u32 ph90,
		 u32 ph135, u32 kvco, u32 kcp, u32 stable_time, u32 emc_source,
		 u32 same_freq)
{
	u32 misc1 = ((setup << PLLM_MISC1_SETUP_SHIFT) |
		     (ph45 << PLLM_MISC1_PD_LSHIFT_PH45_SHIFT) |
		     (ph90 << PLLM_MISC1_PD_LSHIFT_PH90_SHIFT) |
		     (ph135 << PLLM_MISC1_PD_LSHIFT_PH135_SHIFT)),
	    misc2 = ((kvco << PLLM_MISC2_KVCO_SHIFT) |
		     (kcp << PLLM_MISC2_KCP_SHIFT)),
	    base;

	if (same_freq)
		emc_source |= CLK_SOURCE_EMC_MC_EMC_SAME_FREQ;
	else
		emc_source &= ~CLK_SOURCE_EMC_MC_EMC_SAME_FREQ;

	/*
	 * Note PLLM_BASE.PLLM_OUT1_RSTN must be in RESET_ENABLE mode, and
	 * PLLM_BASE.ENABLE must be in DISABLE state (both are the default
	 * values after coldboot reset).
	 */

	writel(misc1, &clk_rst->pllm_misc1);
	writel(misc2, &clk_rst->pllm_misc2);

	/* PLLM.BASE needs BYPASS=0, different from general init_pll */
	base = readl(&clk_rst->pllm_base);
	base &= ~(PLLCMX_BASE_DIVN_MASK | PLLCMX_BASE_DIVM_MASK |
		  PLLM_BASE_DIVP_MASK | PLL_BASE_BYPASS);
	base |= ((m << PLL_BASE_DIVM_SHIFT) | (n << PLL_BASE_DIVN_SHIFT) |
		 (p << PLL_BASE_DIVP_SHIFT));
	writel(base, &clk_rst->pllm_base);

	setbits_le32(&clk_rst->pllm_base, PLL_BASE_ENABLE);
	/* stable_time is required, before we can start to check lock. */
	udelay(stable_time);

	while (!(readl(&clk_rst->pllm_base) & PLL_BASE_LOCK)) {
		udelay(1);
	}
	/*
	 * After PLLM reports being locked, we have to delay 10us before
	 * enabling PLLM_OUT.
	 */
	udelay(10);

	/* Put OUT1 out of reset state (start to output). */
	setbits_le32(&clk_rst->pllm_out, PLLM_OUT1_RSTN_RESET_DISABLE);

	/* Enable and start MEM(MC) and EMC. */
	clock_enable_clear_reset(0, CLK_H_MEM | CLK_H_EMC, 0, 0, 0, 0);
	writel(emc_source, &clk_rst->clk_src_emc);
	udelay(IO_STABILIZATION_DELAY);
}

void clock_cpu0_config_and_reset(void *entry)
{
	void * const evp_cpu_reset = (uint8_t *)TEGRA_EVP_BASE + 0x100;

	write32(CONFIG_STACK_TOP, &maincpu_stack_pointer);
	write32((uintptr_t)entry, &maincpu_entry_point);
	write32((uintptr_t)&maincpu_setup, evp_cpu_reset);

	/* Set active CPU cluster to G */
	clrbits_le32(&flow->cluster_control, 1);

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
	setbits_le32(&clk_rst->clk_out_enb_v, CLK_V_CPULP);

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

	// Disable the reset on the non-CPU parts of the slow cluster.
	write32(CRC_RST_CPULP_CLR_NONCPU,
		&clk_rst->rst_cpulp_cmplx_clr);
	// Disable the various resets on the LP CPU.
	write32(CRC_RST_CPULP_CLR_CPU0 | CRC_RST_CPULP_CLR_DBG0 |
		CRC_RST_CPULP_CLR_CORE0 | CRC_RST_CPULP_CLR_CX0 |
		CRC_RST_CPULP_CLR_L2 | CRC_RST_CPULP_CLR_PDBG,
		&clk_rst->rst_cpulp_cmplx_clr);
}

void clock_halt_avp(void)
{
	for (;;) {
		write32(FLOW_EVENT_JTAG | FLOW_EVENT_LIC_IRQ |
			FLOW_EVENT_GIC_IRQ | FLOW_MODE_WAITEVENT,
			&flow->halt_cop_events);
	}
}

void clock_init(void)
{
	u32 osc = clock_get_osc_bits();

	/* Set PLLC dynramp_step A to 0x2b and B to 0xb (from U-Boot -- why? */
	writel(0x2b << 17 | 0xb << 9, &clk_rst->pllc_misc2);

	/* Max out the AVP clock before everything else (need PLLC for that). */
	init_pll(&clk_rst->pllc_base, &clk_rst->pllc_misc,
		osc_table[osc].pllc, PLLC_MISC_LOCK_ENABLE);

	/* Typical ratios are 1:2:2 or 1:2:3 sclk:hclk:pclk (See: APB DMA
	 * features section in the TRM). */
	write32(1 << HCLK_DIVISOR_SHIFT | 0 << PCLK_DIVISOR_SHIFT,
		&clk_rst->clk_sys_rate);	/* pclk = hclk = sclk/2 */
	write32(CLK_DIVIDER(TEGRA_PLLC_KHZ, 300000) << PLL_OUT_RATIO_SHIFT |
		PLL_OUT_CLKEN | PLL_OUT_RSTN, &clk_rst->pllc_out);
	write32(SCLK_SYS_STATE_RUN << SCLK_SYS_STATE_SHIFT |
		SCLK_SOURCE_PLLC_OUT1 << SCLK_RUN_SHIFT,
		&clk_rst->sclk_brst_pol);		/* sclk = 300 MHz */

	/* Change the oscillator drive strength (from U-Boot -- why?) */
	clrsetbits_le32(&clk_rst->osc_ctrl, OSC_XOFS_MASK,
			OSC_DRIVE_STRENGTH << OSC_XOFS_SHIFT);

	/*
	 * Ambiguous quote from u-boot. TODO: what's this mean?
	 * "should update same value in PMC_OSC_EDPD_OVER XOFS
	 * field for warmboot "
	 */
	clrsetbits_le32(&pmc->osc_edpd_over, PMC_OSC_EDPD_OVER_XOFS_MASK,
			OSC_DRIVE_STRENGTH << PMC_OSC_EDPD_OVER_XOFS_SHIFT);

	/* Disable IDDQ for PLLX before we set it up (from U-Boot -- why?) */
	clrbits_le32(&clk_rst->pllx_misc3, PLLX_IDDQ_MASK);

	/* Set up PLLP_OUT(1|2|3|4) divisor to generate (9.6|48|102|204)MHz */
	write32((CLK_DIVIDER(TEGRA_PLLP_KHZ, 9600) << PLL_OUT_RATIO_SHIFT |
		 PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT1_SHIFT |
		(CLK_DIVIDER(TEGRA_PLLP_KHZ, 48000) << PLL_OUT_RATIO_SHIFT |
		 PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT2_SHIFT,
		&clk_rst->pllp_outa);
	write32((CLK_DIVIDER(TEGRA_PLLP_KHZ, 102000) << PLL_OUT_RATIO_SHIFT |
		 PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT3_SHIFT |
		(CLK_DIVIDER(TEGRA_PLLP_KHZ, 204000) << PLL_OUT_RATIO_SHIFT |
		 PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT4_SHIFT,
		&clk_rst->pllp_outb);

	/* init pllx */
	init_pll(&clk_rst->pllx_base, &clk_rst->pllx_misc,
		osc_table[osc].pllx, PLLPAXS_MISC_LOCK_ENABLE);

	/* init pllu */
	init_pll(&clk_rst->pllu_base, &clk_rst->pllu_misc,
		osc_table[osc].pllu, PLLUD_MISC_LOCK_ENABLE);

	init_utmip_pll();
	graphics_pll();
}

void clock_enable_clear_reset(u32 l, u32 h, u32 u, u32 v, u32 w, u32 x)
{
	if (l) writel(l, &clk_rst->clk_enb_l_set);
	if (h) writel(h, &clk_rst->clk_enb_h_set);
	if (u) writel(u, &clk_rst->clk_enb_u_set);
	if (v) writel(v, &clk_rst->clk_enb_v_set);
	if (w) writel(w, &clk_rst->clk_enb_w_set);
	if (x) writel(x, &clk_rst->clk_enb_x_set);

	/* Give clocks time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);

	if (l) writel(l, &clk_rst->rst_dev_l_clr);
	if (h) writel(h, &clk_rst->rst_dev_h_clr);
	if (u) writel(u, &clk_rst->rst_dev_u_clr);
	if (v) writel(v, &clk_rst->rst_dev_v_clr);
	if (w) writel(w, &clk_rst->rst_dev_w_clr);
	if (x) writel(x, &clk_rst->rst_dev_x_clr);
}

void clock_reset_l(u32 bit)
{
	writel(bit, &clk_rst->rst_dev_l_set);
	udelay(1);
	writel(bit, &clk_rst->rst_dev_l_clr);
}

void clock_reset_h(u32 bit)
{
	writel(bit, &clk_rst->rst_dev_h_set);
	udelay(1);
	writel(bit, &clk_rst->rst_dev_h_clr);
}

void clock_reset_u(u32 bit)
{
	writel(bit, &clk_rst->rst_dev_u_set);
	udelay(1);
	writel(bit, &clk_rst->rst_dev_u_clr);
}

void clock_reset_v(u32 bit)
{
	writel(bit, &clk_rst->rst_dev_v_set);
	udelay(1);
	writel(bit, &clk_rst->rst_dev_v_clr);
}

void clock_reset_w(u32 bit)
{
	writel(bit, &clk_rst->rst_dev_w_set);
	udelay(1);
	writel(bit, &clk_rst->rst_dev_w_clr);
}

void clock_reset_x(u32 bit)
{
	writel(bit, &clk_rst->rst_dev_x_set);
	udelay(1);
	writel(bit, &clk_rst->rst_dev_x_clr);
}
