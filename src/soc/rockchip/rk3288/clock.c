/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <assert.h>
#include <stdlib.h>
#include <arch/io.h>
#include <stdint.h>
#include <console/console.h>
#include <delay.h>
#include "clock.h"
#include "grf.h"
#include "addressmap.h"
#include "cpu.h"

struct pll_div {
	u32	nr;
	u32	nf;
	u32	no;
};

struct rk3288_cru_reg {
	u32 cru_apll_con[4];
	u32 cru_dpll_con[4];
	u32 cru_cpll_con[4];
	u32 cru_gpll_con[4];
	u32 cru_npll_con[4];
	u32 cru_mode_con;
	u32 reserved0[3];
	u32 cru_clksel_con[43];
	u32 reserved1[21];
	u32 cru_clkgate_con[19];
	u32 reserved2;
	u32 cru_glb_srst_fst_value;
	u32 cru_glb_srst_snd_value;
	u32 cru_softrst_con[12];
	u32 cru_misc_con;
	u32 cru_glb_cnt_th;
	u32 cru_glb_rst_con;
	u32 reserved3;
	u32 cru_glb_rst_st;
	u32 reserved4;
	u32 cru_sdmmc_con[2];
	u32 cru_sdio0_con[2];
	u32 cru_sdio1_con[2];
	u32 cru_emmc_con[2];
};
check_member(rk3288_cru_reg, cru_emmc_con[1], 0x021c);

static struct rk3288_cru_reg * const cru_ptr = (void *)CRU_BASE;

#define PLL_DIVISORS(hz, _nr, _no) {\
	.nr = _nr, .nf = (u32)((u64)hz * _nr * _no / 24000000), .no = _no};\
	_Static_assert(((u64)hz * _nr * _no / 24000000) * 24000000 /\
			(_nr * _no) == hz,\
	#hz "Hz cannot be hit with PLL divisors in " __FILE__);

/* apll = 816MHz, gpll = 594MHz, cpll = 384MHz */
static const struct pll_div apll_init_cfg = PLL_DIVISORS(APLL_HZ, 1, 2);
static const struct pll_div gpll_init_cfg = PLL_DIVISORS(GPLL_HZ, 2, 4);
static const struct pll_div cpll_init_cfg = PLL_DIVISORS(CPLL_HZ, 2, 4);

/*******************PLL CON0 BITS***************************/
#define PLL_OD_MSK	(0x0F)

#define PLL_NR_MSK	(0x3F << 8)
#define PLL_NR_SHIFT	(8)

/*******************PLL CON1 BITS***************************/
#define PLL_NF_MSK	(0x1FFF)

/*******************PLL CON2 BITS***************************/
#define PLL_BWADJ_MSK	(0x0FFF)

/*******************PLL CON3 BITS***************************/
#define PLL_RESET_MSK	(1 << 5)
#define PLL_RESET	(1 << 5)
#define PLL_RESET_RESUME	(0 << 5)

/*******************CLKSEL0 BITS***************************/
/* core clk pll sel: amr or general */
#define CORE_SEL_PLL_MSK	(1 << 15)
#define CORE_SEL_APLL	(0 << 15)
#define CORE_SEL_GPLL	(1 << 15)

/* a12 core clock div: clk_core = clk_src / (div_con + 1) */
#define A12_DIV_SHIFT	(8)
#define A12_DIV_MSK	(0x1F << 8)

/* mp core axi clock div: clk = clk_src / (div_con + 1) */
#define MP_DIV_SHIFT	(4)
#define MP_DIV_MSK	(0xF << 4)

/* m0 core axi clock div: clk = clk_src / (div_con + 1) */
#define M0_DIV_MSK	(0xF)

/*******************CLKSEL10 BITS***************************/
/* peripheral bus clk pll sel: codec or general */
#define PERI_SEL_PLL_MSK	(1 << 15)
#define PERI_SEL_CPLL	(0 << 15)
#define PERI_SEL_GPLL	(1 << 15)

/* peripheral bus pclk div:
 * aclk_bus: pclk_bus = 1:1 or 2:1 or 4:1 or 8:1
 */
#define PERI_PCLK_DIV_SHIFT	(12)
#define PERI_PCLK_DIV_MSK	(0x7 << 12)

/* peripheral bus hclk div:
 * aclk_bus: hclk_bus = 1:1 or 2:1 or 4:1
 */
#define PERI_HCLK_DIV_SHIFT	(8)
#define PERI_HCLK_DIV_MSK	(0x3 << 8)

/* peripheral bus aclk div:
 * aclk_periph =
 * periph_clk_src / (peri_aclk_div_con + 1)
 */
#define PERI_ACLK_DIV_MSK	(0x1F)

/*******************CLKSEL37 BITS***************************/
#define L2_DIV_MSK	(0x7)

#define ATCLK_DIV_MSK	(0x1F << 4)
#define ATCLK_DIV_SHIFT	(4)

#define PCLK_DBG_DIV_MSK	(0x1F << 9)
#define PCLK_DBG_DIV_SHIFT	(9)

#define APLL_MODE_MSK	(0x3)
#define APLL_MODE_SLOW	(0)
#define APLL_MODE_NORM	(1)

#define DPLL_MODE_MSK	(0x3 << 4)
#define DPLL_MODE_SLOW	(0 << 4)
#define DPLL_MODE_NORM	(1 << 4)

#define CPLL_MODE_MSK	(0x3 << 8)
#define CPLL_MODE_SLOW	(0 << 8)
#define CPLL_MODE_NORM	(1 << 8)

#define GPLL_MODE_MSK	(0x3 << 12)
#define GPLL_MODE_SLOW	(0 << 12)
#define GPLL_MODE_NORM	(1 << 12)

#define SOCSTS_DPLL_LOCK	(1 << 5)
#define SOCSTS_APLL_LOCK	(1 << 6)
#define SOCSTS_CPLL_LOCK	(1 << 7)
#define SOCSTS_GPLL_LOCK	(1 << 8)

static int rkclk_set_pll(u32 *pll_con, const struct pll_div *pll_div_cfg)
{
	/* enter rest */
	writel(RK_SETBITS(PLL_RESET_MSK), &pll_con[3]);

	writel(RK_CLRSETBITS(PLL_NR_MSK, (pll_div_cfg->nr - 1) << PLL_NR_SHIFT)
	      | RK_CLRSETBITS(PLL_OD_MSK, (pll_div_cfg->no - 1)), &pll_con[0]);

	writel(RK_CLRSETBITS(PLL_NF_MSK, (pll_div_cfg->nf - 1)),
		&pll_con[1]);

	writel(RK_CLRSETBITS(PLL_BWADJ_MSK, ((pll_div_cfg->nf >> 1) - 1)),
		&pll_con[2]);

	udelay(10);

	/* return form rest */
	writel(RK_CLRBITS(PLL_RESET_MSK), &pll_con[3]);

	return 0;
}

void rkclk_init(void)
{
	/* pll enter slow-mode */
	writel(RK_CLRSETBITS(APLL_MODE_MSK, APLL_MODE_SLOW)
		| RK_CLRSETBITS(GPLL_MODE_MSK, GPLL_MODE_SLOW)
		| RK_CLRSETBITS(CPLL_MODE_MSK, CPLL_MODE_SLOW),
		&cru_ptr->cru_mode_con);

	/* init pll */
	rkclk_set_pll(&cru_ptr->cru_apll_con[0], &apll_init_cfg);
	rkclk_set_pll(&cru_ptr->cru_gpll_con[0], &gpll_init_cfg);
	rkclk_set_pll(&cru_ptr->cru_cpll_con[0], &cpll_init_cfg);

	/* waiting for pll lock */
	while (1) {
		if ((readl(&rk3288_grf->soc_status[1])
			& (SOCSTS_APLL_LOCK | SOCSTS_CPLL_LOCK
			   | SOCSTS_GPLL_LOCK))
			== (SOCSTS_APLL_LOCK | SOCSTS_CPLL_LOCK
			   | SOCSTS_GPLL_LOCK))
			break;
		udelay(1);
	}

	/*
	 * core clock pll source selection and
	 * set up dependent divisors for MPAXI/M0AXI and ARM clocks.
	 * core clock select apll, apll clk = 816MHz
	 * arm clk = 816MHz, mpclk = 204MHz, m0clk = 408MHz
	 */
	writel(RK_CLRBITS(CORE_SEL_PLL_MSK)
		| RK_CLRSETBITS(A12_DIV_MSK, 0 << A12_DIV_SHIFT)
		| RK_CLRSETBITS(MP_DIV_MSK, 3 << MP_DIV_SHIFT)
		| RK_CLRSETBITS(M0_DIV_MSK, 1 << 0),
		&cru_ptr->cru_clksel_con[0]);

	/*
	 * set up dependent divisors for L2RAM/ATCLK and PCLK clocks.
	 * l2ramclk = 408MHz, atclk = 204MHz, pclk_dbg = 204MHz
	 */
	writel(RK_CLRSETBITS(L2_DIV_MSK, 1 << 0)
		| RK_CLRSETBITS(ATCLK_DIV_MSK, (3 << ATCLK_DIV_SHIFT))
		| RK_CLRSETBITS(PCLK_DBG_DIV_MSK, (3 << PCLK_DBG_DIV_SHIFT)),
		&cru_ptr->cru_clksel_con[37]);

	/*
	 * peri clock pll source selection and
	 * set up dependent divisors for PCLK/HCLK and ACLK clocks.
	 * peri clock select gpll, gpll clk = 594MHz
	 * aclk = 148.5MHz, hclk = 148.5Mhz, pclk = 74.25MHz
	 */
	writel(RK_SETBITS(PERI_SEL_PLL_MSK)
		| RK_CLRSETBITS(PERI_PCLK_DIV_MSK, 1 << PERI_PCLK_DIV_SHIFT)
		| RK_CLRSETBITS(PERI_HCLK_DIV_MSK, 0 << PERI_HCLK_DIV_SHIFT)
		| RK_CLRSETBITS(PERI_ACLK_DIV_MSK, 3 << 0),
		&cru_ptr->cru_clksel_con[10]);

	/* PLL enter normal-mode */
	writel(RK_CLRSETBITS(APLL_MODE_MSK, APLL_MODE_NORM)
		| RK_CLRSETBITS(GPLL_MODE_MSK, GPLL_MODE_NORM)
		| RK_CLRSETBITS(CPLL_MODE_MSK, CPLL_MODE_NORM),
		&cru_ptr->cru_mode_con);

}

void rkclk_configure_ddr(unsigned int hz)
{
	struct pll_div dpll_cfg;

	if (hz <= 150000000) {
		dpll_cfg.nr = 3;
		dpll_cfg.no = 8;
	} else if (hz <= 540000000) {
		dpll_cfg.nr = 6;
		dpll_cfg.no = 4;
	} else {
		dpll_cfg.nr = 1;
		dpll_cfg.no = 1;
	}

	dpll_cfg.nf = (hz / 1000 * dpll_cfg.nr * dpll_cfg.no) / 24000;
	assert(dpll_cfg.nf < 4096
		&& hz == dpll_cfg.nf * 24000 / (dpll_cfg.nr * dpll_cfg.no)
		* 1000);
	/* pll enter slow-mode */
	writel(RK_CLRSETBITS(DPLL_MODE_MSK, DPLL_MODE_SLOW),
		&cru_ptr->cru_mode_con);

	rkclk_set_pll(&cru_ptr->cru_dpll_con[0], &dpll_cfg);

	/* waiting for pll lock */
	while (1) {
		if (readl(&rk3288_grf->soc_status[1]) & SOCSTS_DPLL_LOCK)
			break;
		udelay(1);
	}

	/* PLL enter normal-mode */
	writel(RK_CLRSETBITS(DPLL_MODE_MSK, DPLL_MODE_NORM),
		&cru_ptr->cru_mode_con);
}

void rkclk_ddr_reset(u32 ch, u32 ctl, u32 phy)
{
	u32 phy_ctl_srstn_shift = 4 + 5 * ch;
	u32 ctl_psrstn_shift = 3 + 5 * ch;
	u32 ctl_srstn_shift = 2 + 5 * ch;
	u32 phy_psrstn_shift = 1 + 5 * ch;
	u32 phy_srstn_shift = 5 * ch;

	writel(RK_CLRSETBITS(1 << phy_ctl_srstn_shift,
			     phy << phy_ctl_srstn_shift)
		| RK_CLRSETBITS(1 << ctl_psrstn_shift, ctl << ctl_psrstn_shift)
		| RK_CLRSETBITS(1 << ctl_srstn_shift, ctl << ctl_srstn_shift)
		| RK_CLRSETBITS(1 << phy_psrstn_shift, phy << phy_psrstn_shift)
		| RK_CLRSETBITS(1 << phy_srstn_shift, phy << phy_srstn_shift),
		&cru_ptr->cru_softrst_con[10]);
}

void rkclk_ddr_phy_ctl_reset(u32 ch, u32 n)
{
	u32 phy_ctl_srstn_shift = 4 + 5 * ch;

	writel(RK_CLRSETBITS(1 << phy_ctl_srstn_shift,
			     n << phy_ctl_srstn_shift),
		&cru_ptr->cru_softrst_con[10]);
}

void rkclk_configure_spi(unsigned int bus, unsigned int hz)
{
	int src_clk_div = GPLL_HZ / hz;

	assert((src_clk_div - 1 < 127) && (src_clk_div * hz == GPLL_HZ));

	switch (bus) {	/*select gpll as spi src clk, and set div*/
	case 0:
		writel(RK_CLRSETBITS(1 << 7 | 0x1f << 0, 1 << 7
					     | (src_clk_div - 1) << 0),
					     &cru_ptr->cru_clksel_con[25]);
		break;
	case 1:
		writel(RK_CLRSETBITS(1 << 15 | 0x1f << 8, 1 << 15
					      | (src_clk_div - 1) << 8),
					      &cru_ptr->cru_clksel_con[25]);
		break;
	case 2:
		writel(RK_CLRSETBITS(1 << 7 | 0x1f << 0, 1 << 7
					     | (src_clk_div - 1) << 0),
					     &cru_ptr->cru_clksel_con[39]);
		break;
	default:
		printk(BIOS_ERR, "do not support this spi bus\n");
	}
}
