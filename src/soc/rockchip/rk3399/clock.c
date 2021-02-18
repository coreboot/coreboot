/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/i2c.h>
#include <soc/soc.h>
#include <stdint.h>
#include <string.h>

struct pll_div {
	u32 refdiv;
	u32 fbdiv;
	u32 postdiv1;
	u32 postdiv2;
	u32 frac;
	u32 freq;
};

#define PLL_DIVISORS(hz, _refdiv, _postdiv1, _postdiv2) {\
	.refdiv = _refdiv,\
	.fbdiv = (u32)((u64)hz * _refdiv * _postdiv1 * _postdiv2 / OSC_HZ),\
	.postdiv1 = _postdiv1, .postdiv2 = _postdiv2, .freq = hz};\
	_Static_assert(((u64)hz * _refdiv * _postdiv1 * _postdiv2 / OSC_HZ) *\
			 OSC_HZ / (_refdiv * _postdiv1 * _postdiv2) == hz,\
			 STRINGIFY(hz) " Hz cannot be hit with PLL "\
			 "divisors on line " STRINGIFY(__LINE__))

static const struct pll_div gpll_init_cfg = PLL_DIVISORS(GPLL_HZ, 1, 4, 1);
static const struct pll_div cpll_init_cfg = PLL_DIVISORS(CPLL_HZ, 1, 3, 1);
static const struct pll_div ppll_init_cfg = PLL_DIVISORS(PPLL_HZ, 3, 2, 1);

static const struct pll_div apll_1512_cfg = PLL_DIVISORS(1512*MHz, 1, 1, 1);
static const struct pll_div apll_600_cfg = PLL_DIVISORS(600*MHz, 1, 3, 1);

static const struct pll_div *apll_cfgs[] = {
	[APLL_1512_MHZ] = &apll_1512_cfg,
	[APLL_600_MHZ] = &apll_600_cfg,
};

enum {
	/* PLL_CON0 */
	PLL_FBDIV_MASK			= 0xfff,
	PLL_FBDIV_SHIFT			= 0,

	/* PLL_CON1 */
	PLL_POSTDIV2_MASK		= 0x7,
	PLL_POSTDIV2_SHIFT		= 12,
	PLL_POSTDIV1_MASK		= 0x7,
	PLL_POSTDIV1_SHIFT		= 8,
	PLL_REFDIV_MASK			= 0x3f,
	PLL_REFDIV_SHIFT		= 0,

	/* PLL_CON2 */
	PLL_LOCK_STATUS_MASK		= 1,
	PLL_LOCK_STATUS_SHIFT		= 31,
	PLL_FRACDIV_MASK		= 0xffffff,
	PLL_FRACDIV_SHIFT		= 0,

	/* PLL_CON3 */
	PLL_MODE_MASK			= 3,
	PLL_MODE_SHIFT			= 8,
	PLL_MODE_SLOW			= 0,
	PLL_MODE_NORM,
	PLL_MODE_DEEP,
	PLL_DSMPD_MASK			= 1,
	PLL_DSMPD_SHIFT			= 3,
	PLL_FRAC_MODE			= 0,
	PLL_INTEGER_MODE		= 1,

	/* PLL_CON4 */
	PLL_SSMOD_BP_MASK		= 1,
	PLL_SSMOD_BP_SHIFT		= 0,
	PLL_SSMOD_DIS_SSCG_MASK		= 1,
	PLL_SSMOD_DIS_SSCG_SHIFT	= 1,
	PLL_SSMOD_RESET_MASK		= 1,
	PLL_SSMOD_RESET_SHIFT		= 2,
	PLL_SSMOD_DOWNSPEAD_MASK	= 1,
	PLL_SSMOD_DOWNSPEAD_SHIFT	= 3,
	PLL_SSMOD_DIVVAL_MASK		= 0xf,
	PLL_SSMOD_DIVVAL_SHIFT		= 4,
	PLL_SSMOD_SPREADAMP_MASK	= 0x1f,
	PLL_SSMOD_SPREADAMP_SHIFT	= 8,

	/* PMUCRU_CLKSEL_CON0 */
	PMU_PCLK_DIV_CON_MASK		= 0x1f,
	PMU_PCLK_DIV_CON_SHIFT		= 0,

	/* PMUCRU_CLKSEL_CON1 */
	SPI3_PLL_SEL_MASK		= 1,
	SPI3_PLL_SEL_SHIFT		= 7,
	SPI3_PLL_SEL_24M		= 0,
	SPI3_PLL_SEL_PPLL		= 1,
	SPI3_DIV_CON_MASK		= 0x7f,
	SPI3_DIV_CON_SHIFT		= 0x0,

	/* PMUCRU_CLKSEL_CON2 */
	I2C_DIV_CON_MASK		= 0x7f,
	I2C8_DIV_CON_SHIFT		= 8,
	I2C0_DIV_CON_SHIFT		= 0,

	/* PMUCRU_CLKSEL_CON3 */
	I2C4_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON0 / CLKSEL_CON2 */
	ACLKM_CORE_DIV_CON_MASK	= 0x1f,
	ACLKM_CORE_DIV_CON_SHIFT	= 8,
	CLK_CORE_PLL_SEL_MASK		= 3,
	CLK_CORE_PLL_SEL_SHIFT		= 6,
	CLK_CORE_PLL_SEL_ALPLL		= 0x0,
	CLK_CORE_PLL_SEL_ABPLL		= 0x1,
	CLK_CORE_PLL_SEL_DPLL		= 0x10,
	CLK_CORE_PLL_SEL_GPLL		= 0x11,
	CLK_CORE_DIV_MASK		= 0x1f,
	CLK_CORE_DIV_SHIFT		= 0,

	/* CLKSEL_CON1 / CLKSEL_CON3 */
	PCLK_DBG_DIV_MASK		= 0x1f,
	PCLK_DBG_DIV_SHIFT		= 0x8,
	ATCLK_CORE_DIV_MASK		= 0x1f,
	ATCLK_CORE_DIV_SHIFT		= 0,

	/* CLKSEL_CON14 */
	PCLK_PERIHP_DIV_CON_MASK	= 0x7,
	PCLK_PERIHP_DIV_CON_SHIFT	= 12,
	HCLK_PERIHP_DIV_CON_MASK	= 3,
	HCLK_PERIHP_DIV_CON_SHIFT	= 8,
	ACLK_PERIHP_PLL_SEL_MASK	= 1,
	ACLK_PERIHP_PLL_SEL_SHIFT	= 7,
	ACLK_PERIHP_PLL_SEL_CPLL	= 0,
	ACLK_PERIHP_PLL_SEL_GPLL	= 1,
	ACLK_PERIHP_DIV_CON_MASK	= 0x1f,
	ACLK_PERIHP_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON21 */
	ACLK_EMMC_PLL_SEL_MASK          = 0x1,
	ACLK_EMMC_PLL_SEL_SHIFT         = 7,
	ACLK_EMMC_PLL_SEL_GPLL          = 0x1,
	ACLK_EMMC_DIV_CON_MASK          = 0x1f,
	ACLK_EMMC_DIV_CON_SHIFT         = 0,

	/* CLKSEL_CON22 */
	CLK_EMMC_PLL_MASK               = 0x7,
	CLK_EMMC_PLL_SHIFT              = 8,
	CLK_EMMC_PLL_SEL_GPLL           = 0x1,
	CLK_EMMC_DIV_CON_MASK           = 0x7f,
	CLK_EMMC_DIV_CON_SHIFT          = 0,

	/* CLKSEL_CON23 */
	PCLK_PERILP0_DIV_CON_MASK	= 0x7,
	PCLK_PERILP0_DIV_CON_SHIFT	= 12,
	HCLK_PERILP0_DIV_CON_MASK	= 3,
	HCLK_PERILP0_DIV_CON_SHIFT	= 8,
	ACLK_PERILP0_PLL_SEL_MASK	= 1,
	ACLK_PERILP0_PLL_SEL_SHIFT	= 7,
	ACLK_PERILP0_PLL_SEL_CPLL	= 0,
	ACLK_PERILP0_PLL_SEL_GPLL	= 1,
	ACLK_PERILP0_DIV_CON_MASK	= 0x1f,
	ACLK_PERILP0_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON25 */
	PCLK_PERILP1_DIV_CON_MASK	= 0x7,
	PCLK_PERILP1_DIV_CON_SHIFT	= 8,
	HCLK_PERILP1_PLL_SEL_MASK	= 1,
	HCLK_PERILP1_PLL_SEL_SHIFT	= 7,
	HCLK_PERILP1_PLL_SEL_CPLL	= 0,
	HCLK_PERILP1_PLL_SEL_GPLL	= 1,
	HCLK_PERILP1_DIV_CON_MASK	= 0x1f,
	HCLK_PERILP1_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON26 */
	CLK_SARADC_DIV_CON_MASK		= 0xff,
	CLK_SARADC_DIV_CON_SHIFT	= 8,

	/* CLKSEL_CON27 */
	CLK_TSADC_SEL_X24M		= 0x0,
	CLK_TSADC_SEL_MASK		= 1,
	CLK_TSADC_SEL_SHIFT		= 15,
	CLK_TSADC_DIV_CON_MASK		= 0x3ff,
	CLK_TSADC_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON44 */
	CLK_PCLK_EDP_PLL_SEL_MASK	= 1,
	CLK_PCLK_EDP_PLL_SEL_SHIFT	= 15,
	CLK_PCLK_EDP_PLL_SEL_CPLL	= 0,
	CLK_PCLK_EDP_DIV_CON_MASK	= 0x3f,
	CLK_PCLK_EDP_DIV_CON_SHIFT	= 8,

	/* CLKSEL_CON47 & CLKSEL_CON48 */
	ACLK_VOP_PLL_SEL_MASK		= 0x3,
	ACLK_VOP_PLL_SEL_SHIFT		= 6,
	ACLK_VOP_PLL_SEL_CPLL		= 0x1,
	ACLK_VOP_DIV_CON_MASK		= 0x1f,
	ACLK_VOP_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON49 & CLKSEL_CON50 */
	DCLK_VOP_DCLK_SEL_MASK          = 1,
	DCLK_VOP_DCLK_SEL_SHIFT         = 11,
	DCLK_VOP_DCLK_SEL_DIVOUT        = 0,
	DCLK_VOP_PLL_SEL_MASK   = 3,
	DCLK_VOP_PLL_SEL_SHIFT          = 8,
	DCLK_VOP_PLL_SEL_VPLL   = 0,
	DCLK_VOP_DIV_CON_MASK   = 0xff,
	DCLK_VOP_DIV_CON_SHIFT          = 0,

	/* CLKSEL_CON58 */
	CLK_SPI_PLL_SEL_MASK		= 1,
	CLK_SPI_PLL_SEL_CPLL		= 0,
	CLK_SPI_PLL_SEL_GPLL		= 1,
	CLK_SPI_PLL_DIV_CON_MASK	= 0x7f,
	CLK_SPI5_PLL_DIV_CON_SHIFT	= 8,
	CLK_SPI5_PLL_SEL_SHIFT		= 15,

	/* CLKSEL_CON59 */
	CLK_SPI1_PLL_SEL_SHIFT		= 15,
	CLK_SPI1_PLL_DIV_CON_SHIFT	= 8,
	CLK_SPI0_PLL_SEL_SHIFT		= 7,
	CLK_SPI0_PLL_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON60 */
	CLK_SPI4_PLL_SEL_SHIFT		= 15,
	CLK_SPI4_PLL_DIV_CON_SHIFT	= 8,
	CLK_SPI2_PLL_SEL_SHIFT		= 7,
	CLK_SPI2_PLL_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON61 */
	CLK_I2C_PLL_SEL_MASK		= 1,
	CLK_I2C_PLL_SEL_CPLL		= 0,
	CLK_I2C_PLL_SEL_GPLL		= 1,
	CLK_I2C5_PLL_SEL_SHIFT		= 15,
	CLK_I2C5_DIV_CON_SHIFT		= 8,
	CLK_I2C1_PLL_SEL_SHIFT		= 7,
	CLK_I2C1_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON62 */
	CLK_I2C6_PLL_SEL_SHIFT		= 15,
	CLK_I2C6_DIV_CON_SHIFT		= 8,
	CLK_I2C2_PLL_SEL_SHIFT		= 7,
	CLK_I2C2_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON63 */
	CLK_I2C7_PLL_SEL_SHIFT		= 15,
	CLK_I2C7_DIV_CON_SHIFT		= 8,
	CLK_I2C3_PLL_SEL_SHIFT		= 7,
	CLK_I2C3_DIV_CON_SHIFT		= 0,

	/* CRU_SOFTRST_CON4 */
	RESETN_DDR0_REQ_MASK		= 1,
	RESETN_DDR0_REQ_SHIFT		= 8,
	RESETN_DDRPHY0_REQ_MASK		= 1,
	RESETN_DDRPHY0_REQ_SHIFT	= 9,
	RESETN_DDR1_REQ_MASK		= 1,
	RESETN_DDR1_REQ_SHIFT		= 12,
	RESETN_DDRPHY1_REQ_MASK		= 1,
	RESETN_DDRPHY1_REQ_SHIFT	= 13,
};

#define VCO_MAX_KHZ	(3200 * (MHz / KHz))
#define VCO_MIN_KHZ	(800 * (MHz / KHz))
#define OUTPUT_MAX_KHZ	(3200 * (MHz / KHz))
#define OUTPUT_MIN_KHZ	(16 * (MHz / KHz))

/* the div restrictions of pll in integer mode,
 * these are defined in * CRU_*PLL_CON0 or PMUCRU_*PLL_CON0
 */
#define PLL_DIV_MIN	16
#define PLL_DIV_MAX	3200

/* How to calculate the PLL(from TRM V0.3 Part 1 Page 63):
 * Formulas also embedded within the Fractional PLL Verilog model:
 * If DSMPD = 1 (DSM is disabled, "integer mode")
 * FOUTVCO = FREF / REFDIV * FBDIV
 * FOUTPOSTDIV = FOUTVCO / POSTDIV1 / POSTDIV2
 * Where:
 * FOUTVCO = Fractional PLL non-divided output frequency
 * FOUTPOSTDIV = Fractional PLL divided output frequency
 *               (output of second post divider)
 * FREF = Fractional PLL input reference frequency, (the OSC_HZ 24MHz input)
 * REFDIV = Fractional PLL input reference clock divider
 * FBDIV = Integer value programmed into feedback divide
 *
 */
static void rkclk_set_pll(u32 *pll_con, const struct pll_div *div)
{
	/* All 8 PLLs have same VCO and output frequency range restrictions. */
	u32 vco_khz = OSC_HZ / 1000 * div->fbdiv / div->refdiv;
	u32 output_khz = vco_khz / div->postdiv1 / div->postdiv2;

	printk(BIOS_DEBUG, "PLL at %p: fbdiv=%d, refdiv=%d, postdiv1=%d, "
			   "postdiv2=%d, vco=%u kHz, output=%u kHz\n",
			   pll_con, div->fbdiv, div->refdiv, div->postdiv1,
			   div->postdiv2, vco_khz, output_khz);
	assert(vco_khz >= VCO_MIN_KHZ && vco_khz <= VCO_MAX_KHZ &&
	       output_khz >= OUTPUT_MIN_KHZ && output_khz <= OUTPUT_MAX_KHZ &&
	       div->fbdiv >= PLL_DIV_MIN && div->fbdiv <= PLL_DIV_MAX);

	/* When power on or changing PLL setting,
	 * we must force PLL into slow mode to ensure output stable clock.
	 */
	write32(&pll_con[3], RK_CLRSETBITS(PLL_MODE_MASK << PLL_MODE_SHIFT,
					   PLL_MODE_SLOW << PLL_MODE_SHIFT));

	/* use integer mode */
	write32(&pll_con[3],
		RK_CLRSETBITS(PLL_DSMPD_MASK << PLL_DSMPD_SHIFT,
			      PLL_INTEGER_MODE << PLL_DSMPD_SHIFT));

	write32(&pll_con[0], RK_CLRSETBITS(PLL_FBDIV_MASK << PLL_FBDIV_SHIFT,
					   div->fbdiv << PLL_FBDIV_SHIFT));
	write32(&pll_con[1],
		RK_CLRSETBITS(PLL_POSTDIV2_MASK << PLL_POSTDIV2_SHIFT |
			      PLL_POSTDIV1_MASK << PLL_POSTDIV1_SHIFT |
			      PLL_REFDIV_MASK | PLL_REFDIV_SHIFT,
			      (div->postdiv2 << PLL_POSTDIV2_SHIFT) |
			      (div->postdiv1 << PLL_POSTDIV1_SHIFT) |
			      (div->refdiv << PLL_REFDIV_SHIFT)));

	/* waiting for pll lock */
	while (!(read32(&pll_con[2]) & (1 << PLL_LOCK_STATUS_SHIFT)))
		udelay(1);

	/* pll enter normal mode */
	write32(&pll_con[3], RK_CLRSETBITS(PLL_MODE_MASK << PLL_MODE_SHIFT,
					   PLL_MODE_NORM << PLL_MODE_SHIFT));
}

/*
 * Configure the DPLL spread spectrum feature on memory clock.
 * Configure sequence:
 * 1. PLL been configured as frac mode, and DACPD should be set to 1'b0.
 * 2. Configure DOWNSPERAD, SPREAD, DIVVAL(option: configure xPLL_CON5 with
 *    extern wave table).
 * 3. set ssmod_disable_sscg = 1'b0, and set ssmod_bp = 1'b0.
 * 4. Assert RESET = 1'b1 to SSMOD.
 * 5. RESET = 1'b0 on SSMOD.
 * 6. Adjust SPREAD/DIVVAL/DOWNSPREAD.
 */
static void rkclk_set_dpllssc(struct pll_div *dpll_cfg)
{
	u32 divval;

	assert(dpll_cfg->refdiv && dpll_cfg->refdiv <= 6);

	/*
	 * Need to acquire ~30kHZ which is the target modulation frequency.
	 * The modulation frequency ~ 30kHz= OSC_HZ/revdiv/128/divval
	 * (the 128 is the number points in the query table).
	 */
	divval = OSC_HZ / 128 / (30 * KHz) / dpll_cfg->refdiv;

	/*
	 * Use frac mode.
	 * Make sure the output frequency isn't offset, set 0 for Fractional
	 * part of feedback divide.
	 */
	write32(&cru_ptr->dpll_con[3],
		RK_CLRSETBITS(PLL_DSMPD_MASK << PLL_DSMPD_SHIFT,
			      PLL_FRAC_MODE << PLL_DSMPD_SHIFT));
	clrsetbits32(&cru_ptr->dpll_con[2],
		     PLL_FRACDIV_MASK << PLL_FRACDIV_SHIFT,
		     0 << PLL_FRACDIV_SHIFT);

	/*
	 * Configure SSC divval.
	 * Spread amplitude range = 0.1 * SPREAD[4:0] (%).
	 * The below 8 means SPREAD[4:0] that appears to mitigate EMI on boards
	 * tested. Center and down spread modulation amplitudes based on the
	 * value of SPREAD.
	 * SPREAD[4:0]	Center Spread	Down Spread
	 *	0	0		0
	 *	1	+/-0.1%		-0.10%
	 *	2	+/-0.2%		-0.20%
	 *	3	+/-0.3%		-0.30%
	 *	4	+/-0.4%		-0.40%
	 *	5	+/-0.5%		-0.50%
	 *	...
	 *	31	+/-3.1%		-3.10%
	 */
	write32(&cru_ptr->dpll_con[4],
		RK_CLRSETBITS(PLL_SSMOD_DIVVAL_MASK << PLL_SSMOD_DIVVAL_SHIFT,
			      divval << PLL_SSMOD_DIVVAL_SHIFT));
	write32(&cru_ptr->dpll_con[4],
		RK_CLRSETBITS(PLL_SSMOD_SPREADAMP_MASK <<
			      PLL_SSMOD_SPREADAMP_SHIFT,
			      8 << PLL_SSMOD_SPREADAMP_SHIFT));

	/* Enable SSC for DPLL */
	write32(&cru_ptr->dpll_con[4],
		RK_CLRBITS(PLL_SSMOD_BP_MASK << PLL_SSMOD_BP_SHIFT |
			   PLL_SSMOD_DIS_SSCG_MASK << PLL_SSMOD_DIS_SSCG_SHIFT));

	/* Deassert reset SSMOD */
	write32(&cru_ptr->dpll_con[4],
		RK_CLRBITS(PLL_SSMOD_RESET_MASK << PLL_SSMOD_RESET_SHIFT));

	udelay(20);
}

static int pll_para_config(u32 freq_hz, struct pll_div *div)
{
	u32 ref_khz = OSC_HZ / KHz, refdiv, fbdiv = 0;
	u32 postdiv1, postdiv2 = 1;
	u32 fref_khz;
	u32 diff_khz, best_diff_khz;
	const u32 max_refdiv = 63, max_fbdiv = 3200, min_fbdiv = 16;
	const u32 max_postdiv1 = 7, max_postdiv2 = 7;
	u32 vco_khz;
	u32 freq_khz = freq_hz / KHz;

	if (!freq_hz) {
		printk(BIOS_ERR, "%s: the frequency can't be 0 Hz\n", __func__);
		return -1;
	}

	postdiv1 = DIV_ROUND_UP(VCO_MIN_KHZ, freq_khz);
	if (postdiv1 > max_postdiv1) {
		postdiv2 = DIV_ROUND_UP(postdiv1, max_postdiv1);
		postdiv1 = DIV_ROUND_UP(postdiv1, postdiv2);
	}

	vco_khz = freq_khz * postdiv1 * postdiv2;

	if (vco_khz < VCO_MIN_KHZ || vco_khz > VCO_MAX_KHZ ||
	    postdiv2 > max_postdiv2) {
		printk(BIOS_ERR, "%s: Cannot find out a supported VCO"
		       " for Frequency (%uHz).\n", __func__, freq_hz);
		return -1;
	}

	div->postdiv1 = postdiv1;
	div->postdiv2 = postdiv2;

	best_diff_khz = vco_khz;
	for (refdiv = 1; refdiv < max_refdiv && best_diff_khz; refdiv++) {
		fref_khz = ref_khz / refdiv;

		fbdiv = vco_khz / fref_khz;
		if ((fbdiv >= max_fbdiv) || (fbdiv <= min_fbdiv))
			continue;
		diff_khz = vco_khz - fbdiv * fref_khz;
		if (fbdiv + 1 < max_fbdiv && diff_khz > fref_khz / 2) {
			fbdiv++;
			diff_khz = fref_khz - diff_khz;
		}

		if (diff_khz >= best_diff_khz)
			continue;

		best_diff_khz = diff_khz;
		div->refdiv = refdiv;
		div->fbdiv = fbdiv;
	}

	if (best_diff_khz > 4 * (MHz/KHz)) {
		printk(BIOS_ERR, "%s: Failed to match output frequency %u, "
		       "difference is %u Hz,exceed 4MHZ\n", __func__, freq_hz,
		       best_diff_khz * KHz);
		return -1;
	}
	return 0;
}

void rkclk_init(void)
{
	u32 aclk_div;
	u32 hclk_div;
	u32 pclk_div;

	/* some cru registers changed by bootrom, we'd better reset them to
	 * reset/default values described in TRM to avoid confusion in kernel.
	 * Please consider these three lines as a fix of bootrom bug.
	 */
	write32(&cru_ptr->clksel_con[12], 0xffff4101);
	write32(&cru_ptr->clksel_con[19], 0xffff033f);
	write32(&cru_ptr->clksel_con[56], 0x00030003);

	/* configure pmu pll(ppll) */
	rkclk_set_pll(&pmucru_ptr->ppll_con[0], &ppll_init_cfg);

	/* configure pmu pclk */
	pclk_div = PPLL_HZ / PMU_PCLK_HZ - 1;
	assert((unsigned int)(PPLL_HZ - (pclk_div + 1) * PMU_PCLK_HZ) <= pclk_div
	       && pclk_div <= 0x1f);
	write32(&pmucru_ptr->pmucru_clksel[0],
		RK_CLRSETBITS(PMU_PCLK_DIV_CON_MASK << PMU_PCLK_DIV_CON_SHIFT,
			      pclk_div << PMU_PCLK_DIV_CON_SHIFT));

	/* configure gpll cpll */
	rkclk_set_pll(&cru_ptr->gpll_con[0], &gpll_init_cfg);
	rkclk_set_pll(&cru_ptr->cpll_con[0], &cpll_init_cfg);

	/* configure perihp aclk, hclk, pclk */
	aclk_div = GPLL_HZ / PERIHP_ACLK_HZ - 1;
	assert((aclk_div + 1) * PERIHP_ACLK_HZ == GPLL_HZ && aclk_div <= 0x1f);

	hclk_div = PERIHP_ACLK_HZ / PERIHP_HCLK_HZ - 1;
	assert((hclk_div + 1) * PERIHP_HCLK_HZ ==
	       PERIHP_ACLK_HZ && (hclk_div <= 0x3));

	pclk_div = PERIHP_ACLK_HZ / PERIHP_PCLK_HZ - 1;
	assert((pclk_div + 1) * PERIHP_PCLK_HZ ==
	       PERIHP_ACLK_HZ && (pclk_div <= 0x7));

	write32(&cru_ptr->clksel_con[14],
		RK_CLRSETBITS(PCLK_PERIHP_DIV_CON_MASK <<
						PCLK_PERIHP_DIV_CON_SHIFT |
			      HCLK_PERIHP_DIV_CON_MASK <<
						HCLK_PERIHP_DIV_CON_SHIFT |
			      ACLK_PERIHP_PLL_SEL_MASK <<
						ACLK_PERIHP_PLL_SEL_SHIFT |
			      ACLK_PERIHP_DIV_CON_MASK <<
						ACLK_PERIHP_DIV_CON_SHIFT,
			      pclk_div << PCLK_PERIHP_DIV_CON_SHIFT |
			      hclk_div << HCLK_PERIHP_DIV_CON_SHIFT |
			      ACLK_PERIHP_PLL_SEL_GPLL <<
						ACLK_PERIHP_PLL_SEL_SHIFT |
			      aclk_div << ACLK_PERIHP_DIV_CON_SHIFT));

	/* configure perilp0 aclk, hclk, pclk */
	aclk_div = GPLL_HZ / PERILP0_ACLK_HZ - 1;
	assert((aclk_div + 1) * PERILP0_ACLK_HZ == GPLL_HZ && aclk_div <= 0x1f);

	hclk_div = PERILP0_ACLK_HZ / PERILP0_HCLK_HZ - 1;
	assert((hclk_div + 1) * PERILP0_HCLK_HZ ==
	       PERILP0_ACLK_HZ && (hclk_div <= 0x3));

	pclk_div = PERILP0_ACLK_HZ / PERILP0_PCLK_HZ - 1;
	assert((pclk_div + 1) * PERILP0_PCLK_HZ ==
	       PERILP0_ACLK_HZ && (pclk_div <= 0x7));

	write32(&cru_ptr->clksel_con[23],
		RK_CLRSETBITS(PCLK_PERILP0_DIV_CON_MASK <<
						PCLK_PERILP0_DIV_CON_SHIFT |
			      HCLK_PERILP0_DIV_CON_MASK <<
						HCLK_PERILP0_DIV_CON_SHIFT |
			      ACLK_PERILP0_PLL_SEL_MASK <<
						ACLK_PERILP0_PLL_SEL_SHIFT |
			      ACLK_PERILP0_DIV_CON_MASK <<
						ACLK_PERILP0_DIV_CON_SHIFT,
			      pclk_div << PCLK_PERILP0_DIV_CON_SHIFT |
			      hclk_div << HCLK_PERILP0_DIV_CON_SHIFT |
			      ACLK_PERILP0_PLL_SEL_GPLL <<
						ACLK_PERILP0_PLL_SEL_SHIFT |
			      aclk_div << ACLK_PERILP0_DIV_CON_SHIFT));

	/* perilp1 hclk select gpll as source */
	hclk_div = GPLL_HZ / PERILP1_HCLK_HZ - 1;
	assert((hclk_div + 1) * PERILP1_HCLK_HZ ==
	       GPLL_HZ && (hclk_div <= 0x1f));

	pclk_div = PERILP1_HCLK_HZ / PERILP1_PCLK_HZ - 1;
	assert((pclk_div + 1) * PERILP1_PCLK_HZ ==
	       PERILP1_HCLK_HZ && (pclk_div <= 0x7));

	write32(&cru_ptr->clksel_con[25],
		RK_CLRSETBITS(PCLK_PERILP1_DIV_CON_MASK <<
						PCLK_PERILP1_DIV_CON_SHIFT |
			      HCLK_PERILP1_DIV_CON_MASK <<
						HCLK_PERILP1_DIV_CON_SHIFT |
			      HCLK_PERILP1_PLL_SEL_MASK <<
						HCLK_PERILP1_PLL_SEL_SHIFT,
			      pclk_div << PCLK_PERILP1_DIV_CON_SHIFT |
			      hclk_div << HCLK_PERILP1_DIV_CON_SHIFT |
			      HCLK_PERILP1_PLL_SEL_GPLL <<
						HCLK_PERILP1_PLL_SEL_SHIFT));
}

void rkclk_configure_cpu(enum apll_frequencies freq, enum cpu_cluster cluster)
{
	u32 aclkm_div, atclk_div, pclk_dbg_div, apll_hz;
	int con_base, parent;
	u32 *pll_con;

	switch (cluster) {
	case CPU_CLUSTER_LITTLE:
		con_base = 0;
		parent = CLK_CORE_PLL_SEL_ALPLL;
		pll_con = &cru_ptr->apll_l_con[0];
		break;
	case CPU_CLUSTER_BIG:
	default:
		con_base = 2;
		parent = CLK_CORE_PLL_SEL_ABPLL;
		pll_con = &cru_ptr->apll_b_con[0];
		break;
	}

	apll_hz = apll_cfgs[freq]->freq;
	rkclk_set_pll(pll_con, apll_cfgs[freq]);

	aclkm_div = DIV_ROUND_UP(apll_hz, ACLKM_CORE_HZ) - 1;
	pclk_dbg_div = DIV_ROUND_UP(apll_hz, PCLK_DBG_HZ) - 1;
	atclk_div = DIV_ROUND_UP(apll_hz, ATCLK_CORE_HZ) - 1;

	write32(&cru_ptr->clksel_con[con_base],
		RK_CLRSETBITS(ACLKM_CORE_DIV_CON_MASK <<
						ACLKM_CORE_DIV_CON_SHIFT |
			      CLK_CORE_PLL_SEL_MASK << CLK_CORE_PLL_SEL_SHIFT |
			      CLK_CORE_DIV_MASK << CLK_CORE_DIV_SHIFT,
			      aclkm_div << ACLKM_CORE_DIV_CON_SHIFT |
			      parent << CLK_CORE_PLL_SEL_SHIFT |
			      0 << CLK_CORE_DIV_SHIFT));

	write32(&cru_ptr->clksel_con[con_base + 1],
		RK_CLRSETBITS(PCLK_DBG_DIV_MASK << PCLK_DBG_DIV_SHIFT |
			      ATCLK_CORE_DIV_MASK << ATCLK_CORE_DIV_SHIFT,
			      pclk_dbg_div << PCLK_DBG_DIV_SHIFT |
			      atclk_div << ATCLK_CORE_DIV_SHIFT));
}

void rkclk_configure_ddr(unsigned int hz)
{
	struct pll_div dpll_cfg;

	/* IC ECO bug, need to set this register */
	write32(&rk3399_pmusgrf->ddr_rgn_con[16], 0xc000c000);

	/* clk_ddrc == DPLL = 24MHz / refdiv * fbdiv / postdiv1 / postdiv2 */
	switch (hz) {
	case 200*MHz:
		dpll_cfg = (struct pll_div)
		{.refdiv = 1, .fbdiv = 50, .postdiv1 = 3, .postdiv2 = 2};
		break;
	case 300*MHz:
		dpll_cfg = (struct pll_div)
		{.refdiv = 2, .fbdiv = 100, .postdiv1 = 4, .postdiv2 = 1};
		break;
	case 666*MHz:
		dpll_cfg = (struct pll_div)
		{.refdiv = 2, .fbdiv = 111, .postdiv1 = 2, .postdiv2 = 1};
		break;
	case 800*MHz:
		dpll_cfg = (struct pll_div)
		{.refdiv = 1, .fbdiv = 100, .postdiv1 = 3, .postdiv2 = 1};
		break;
	case 933*MHz:
		dpll_cfg = (struct pll_div)
		{.refdiv = 1, .fbdiv = 116, .postdiv1 = 3, .postdiv2 = 1};
		break;
	default:
		die("Unsupported SDRAM frequency, add to clock.c!");
	}
	rkclk_set_pll(&cru_ptr->dpll_con[0], &dpll_cfg);

	if (CONFIG(RK3399_SPREAD_SPECTRUM_DDR))
		rkclk_set_dpllssc(&dpll_cfg);
}

#define CRU_SFTRST_DDR_CTRL(ch, n)	((1 << 16 | (n)) << (8 + (ch) * 4))
#define CRU_SFTRST_DDR_PHY(ch, n)	((1 << 16 | (n)) << (9 + (ch) * 4))

void rkclk_ddr_reset(u32 channel, u32 ctl, u32 phy)
{
	write32(&cru_ptr->softrst_con[4],
		CRU_SFTRST_DDR_CTRL(channel, ctl) |
		CRU_SFTRST_DDR_PHY(channel, phy));
}

#define SPI_CLK_REG_VALUE(bus, clk_div) \
		RK_CLRSETBITS(CLK_SPI_PLL_SEL_MASK << \
					CLK_SPI ##bus## _PLL_SEL_SHIFT | \
			      CLK_SPI_PLL_DIV_CON_MASK << \
					CLK_SPI ##bus## _PLL_DIV_CON_SHIFT, \
			      CLK_SPI_PLL_SEL_GPLL << \
					CLK_SPI ##bus## _PLL_SEL_SHIFT | \
			      (clk_div - 1) << \
					CLK_SPI ##bus## _PLL_DIV_CON_SHIFT)

void rkclk_configure_spi(unsigned int bus, unsigned int hz)
{
	int src_clk_div;
	int pll;

	/* spi3 src clock from ppll, while spi0,1,2,4,5 src clock from gpll */
	pll = (bus == 3) ? PPLL_HZ : GPLL_HZ;
	src_clk_div = pll / hz;
	assert((src_clk_div - 1 <= 127) && (src_clk_div * hz == pll));

	switch (bus) {
	case 0:
		write32(&cru_ptr->clksel_con[59],
			SPI_CLK_REG_VALUE(0, src_clk_div));
		break;
	case 1:
		write32(&cru_ptr->clksel_con[59],
			SPI_CLK_REG_VALUE(1, src_clk_div));
		break;
	case 2:
		write32(&cru_ptr->clksel_con[60],
			SPI_CLK_REG_VALUE(2, src_clk_div));
		break;
	case 3:
		write32(&pmucru_ptr->pmucru_clksel[1],
			RK_CLRSETBITS(SPI3_PLL_SEL_MASK << SPI3_PLL_SEL_SHIFT |
				      SPI3_DIV_CON_MASK << SPI3_DIV_CON_SHIFT,
				      SPI3_PLL_SEL_PPLL << SPI3_PLL_SEL_SHIFT |
				      (src_clk_div - 1) << SPI3_DIV_CON_SHIFT));
		break;
	case 4:
		write32(&cru_ptr->clksel_con[60],
			SPI_CLK_REG_VALUE(4, src_clk_div));
		break;
	case 5:
		write32(&cru_ptr->clksel_con[58],
			SPI_CLK_REG_VALUE(5, src_clk_div));
		break;
	default:
		printk(BIOS_ERR, "do not support this spi bus\n");
	}
}

#define I2C_CLK_REG_VALUE(bus, clk_div) \
		RK_CLRSETBITS(I2C_DIV_CON_MASK << \
					CLK_I2C ##bus## _DIV_CON_SHIFT | \
			      CLK_I2C_PLL_SEL_MASK << \
					CLK_I2C ##bus## _PLL_SEL_SHIFT, \
			      (clk_div - 1) << \
					CLK_I2C ##bus## _DIV_CON_SHIFT | \
			      CLK_I2C_PLL_SEL_GPLL << \
					CLK_I2C ##bus## _PLL_SEL_SHIFT)
#define PMU_I2C_CLK_REG_VALUE(bus, clk_div) \
		RK_CLRSETBITS(I2C_DIV_CON_MASK << I2C ##bus## _DIV_CON_SHIFT, \
			      (clk_div - 1) << I2C ##bus## _DIV_CON_SHIFT)

uint32_t rkclk_i2c_clock_for_bus(unsigned int bus)
{
	int src_clk_div, pll, freq;

	/* i2c0,4,8 src clock from ppll, i2c1,2,3,5,6,7 src clock from gpll */
	if (bus == 0 || bus == 4 || bus == 8) {
		pll = PPLL_HZ;
		freq = 338*MHz;
	} else {
		pll = GPLL_HZ;
		freq = 198*MHz;
	}
	src_clk_div = pll / freq;
	assert((src_clk_div - 1 <= 127) && (src_clk_div * freq == pll));

	switch (bus) {
	case 0:
		write32(&pmucru_ptr->pmucru_clksel[2],
			PMU_I2C_CLK_REG_VALUE(0, src_clk_div));
		break;
	case 1:
		write32(&cru_ptr->clksel_con[61],
			I2C_CLK_REG_VALUE(1, src_clk_div));
		break;
	case 2:
		write32(&cru_ptr->clksel_con[62],
			I2C_CLK_REG_VALUE(2, src_clk_div));
		break;
	case 3:
		write32(&cru_ptr->clksel_con[63],
			I2C_CLK_REG_VALUE(3, src_clk_div));
		break;
	case 4:
		write32(&pmucru_ptr->pmucru_clksel[3],
			PMU_I2C_CLK_REG_VALUE(4, src_clk_div));
		break;
	case 5:
		write32(&cru_ptr->clksel_con[61],
			I2C_CLK_REG_VALUE(5, src_clk_div));
		break;
	case 6:
		write32(&cru_ptr->clksel_con[62],
			I2C_CLK_REG_VALUE(6, src_clk_div));
		break;
	case 7:
		write32(&cru_ptr->clksel_con[63],
			I2C_CLK_REG_VALUE(7, src_clk_div));
		break;
	case 8:
		write32(&pmucru_ptr->pmucru_clksel[2],
			PMU_I2C_CLK_REG_VALUE(8, src_clk_div));
		break;
	default:
		die("unknown i2c bus\n");
	}

	return freq;
}

static u32 clk_gcd(u32 a, u32 b)
{
	while (b != 0) {
		int r = b;
		b = a % b;
		a = r;
	}
	return a;
}

void rkclk_configure_i2s(unsigned int hz)
{
	int n, d;
	int v;

	/**
	 * clk_i2s0_sel: divider output from fraction
	 * clk_i2s0_pll_sel source clock: cpll
	 * clk_i2s0_div_con: 1 (div+1)
	 */
	write32(&cru_ptr->clksel_con[28],
		RK_CLRSETBITS(3 << 8 | 1 << 7 | 0x7f << 0,
			      1 << 8 | 0 << 7 | 0 << 0));

	/* make sure and enable i2s0 path gates */
	write32(&cru_ptr->clkgate_con[8],
		RK_CLRBITS(1 << 12 | 1 << 5 | 1 << 4 | 1 << 3));

	/* set frac divider */
	v = clk_gcd(CPLL_HZ, hz);
	n = (CPLL_HZ / v) & (0xffff);
	d = (hz / v) & (0xffff);
	assert(hz == (u64)CPLL_HZ * d / n);
	write32(&cru_ptr->clksel_con[96], d << 16 | n);

	/**
	 * clk_i2sout_sel clk_i2s
	 * clk_i2s_ch_sel: clk_i2s0
	 */
	write32(&cru_ptr->clksel_con[31],
		RK_CLRSETBITS(1 << 2 | 3 << 0,
			      0 << 2 | 0 << 0));
}

void rkclk_configure_saradc(unsigned int hz)
{
	int src_clk_div;

	/* saradc src clk from 24MHz */
	src_clk_div = 24 * MHz / hz;
	assert((src_clk_div - 1 <= 255) && (src_clk_div * hz == 24 * MHz));

	write32(&cru_ptr->clksel_con[26],
		RK_CLRSETBITS(CLK_SARADC_DIV_CON_MASK <<
						CLK_SARADC_DIV_CON_SHIFT,
			      (src_clk_div - 1) << CLK_SARADC_DIV_CON_SHIFT));
}

void rkclk_configure_vop_aclk(u32 vop_id, u32 aclk_hz)
{
	u32 div;
	void *reg_addr = vop_id ? &cru_ptr->clksel_con[48] :
				  &cru_ptr->clksel_con[47];

	/* vop aclk source clk: cpll */
	div = CPLL_HZ / aclk_hz;
	assert((div - 1 <= 31) && (div * aclk_hz == CPLL_HZ));

	write32(reg_addr, RK_CLRSETBITS(
			ACLK_VOP_PLL_SEL_MASK << ACLK_VOP_PLL_SEL_SHIFT |
			ACLK_VOP_DIV_CON_MASK << ACLK_VOP_DIV_CON_SHIFT,
			ACLK_VOP_PLL_SEL_CPLL << ACLK_VOP_PLL_SEL_SHIFT |
			(div - 1) << ACLK_VOP_DIV_CON_SHIFT));
}

int rkclk_configure_vop_dclk(u32 vop_id, u32 dclk_hz)
{
	struct pll_div vpll_config = {0};
	void *reg_addr = vop_id ? &cru_ptr->clksel_con[50] :
				  &cru_ptr->clksel_con[49];

	/* vop dclk source from vpll, and equals to vpll(means div == 1) */
	if (pll_para_config(dclk_hz, &vpll_config))
		return -1;

	rkclk_set_pll(&cru_ptr->vpll_con[0], &vpll_config);

	write32(reg_addr, RK_CLRSETBITS(
			DCLK_VOP_DCLK_SEL_MASK << DCLK_VOP_DCLK_SEL_SHIFT |
			DCLK_VOP_PLL_SEL_MASK << DCLK_VOP_PLL_SEL_SHIFT |
			DCLK_VOP_DIV_CON_MASK << DCLK_VOP_DIV_CON_SHIFT,
			DCLK_VOP_DCLK_SEL_DIVOUT << DCLK_VOP_DCLK_SEL_SHIFT |
			DCLK_VOP_PLL_SEL_VPLL << DCLK_VOP_PLL_SEL_SHIFT |
			(1 - 1) << DCLK_VOP_DIV_CON_SHIFT));

	return 0;
}

void rkclk_configure_tsadc(unsigned int hz)
{
	int src_clk_div;

	/* use 24M as src clock */
	src_clk_div = OSC_HZ / hz;
	assert((src_clk_div - 1 <= 1023) && (src_clk_div * hz == OSC_HZ));

	write32(&cru_ptr->clksel_con[27], RK_CLRSETBITS(
			CLK_TSADC_DIV_CON_MASK << CLK_TSADC_DIV_CON_SHIFT |
			CLK_TSADC_SEL_MASK << CLK_TSADC_SEL_SHIFT,
			src_clk_div << CLK_TSADC_DIV_CON_SHIFT |
			CLK_TSADC_SEL_X24M << CLK_TSADC_SEL_SHIFT));
}

void rkclk_configure_emmc(void)
{
	int src_clk_div;
	int aclk_emmc = 148500*KHz;
	int clk_emmc = 148500*KHz;

	/* Select aclk_emmc source from GPLL */
	src_clk_div = GPLL_HZ / aclk_emmc;
	assert((src_clk_div - 1 <= 31) && (src_clk_div * aclk_emmc == GPLL_HZ));

	write32(&cru_ptr->clksel_con[21],
		RK_CLRSETBITS(ACLK_EMMC_PLL_SEL_MASK <<
						ACLK_EMMC_PLL_SEL_SHIFT |
			      ACLK_EMMC_DIV_CON_MASK << ACLK_EMMC_DIV_CON_SHIFT,
			      ACLK_EMMC_PLL_SEL_GPLL <<
						ACLK_EMMC_PLL_SEL_SHIFT |
			      (src_clk_div - 1) << ACLK_EMMC_DIV_CON_SHIFT));

	/* Select clk_emmc source from GPLL too */
	src_clk_div = GPLL_HZ / clk_emmc;
	assert((src_clk_div - 1 <= 127) && (src_clk_div * clk_emmc == GPLL_HZ));

	write32(&cru_ptr->clksel_con[22],
		RK_CLRSETBITS(CLK_EMMC_PLL_MASK << CLK_EMMC_PLL_SHIFT |
			      CLK_EMMC_DIV_CON_MASK << CLK_EMMC_DIV_CON_SHIFT,
			      CLK_EMMC_PLL_SEL_GPLL << CLK_EMMC_PLL_SHIFT |
			      (src_clk_div - 1) << CLK_EMMC_DIV_CON_SHIFT));
}

int rkclk_was_watchdog_reset(void)
{
	/* Bits 5 and 4 are "second" and "first" global watchdog reset. */
	return read32(&cru_ptr->glb_rst_st) & 0x30;
}

void rkclk_configure_edp(unsigned int hz)
{
	int src_clk_div;

	src_clk_div = CPLL_HZ / hz;
	assert((src_clk_div - 1 <= 63) && (src_clk_div * hz == CPLL_HZ));

	write32(&cru_ptr->clksel_con[44],
		RK_CLRSETBITS(CLK_PCLK_EDP_PLL_SEL_MASK <<
			      CLK_PCLK_EDP_PLL_SEL_SHIFT |
			      CLK_PCLK_EDP_DIV_CON_MASK <<
			      CLK_PCLK_EDP_DIV_CON_SHIFT,
			      CLK_PCLK_EDP_PLL_SEL_CPLL <<
			      CLK_PCLK_EDP_PLL_SEL_SHIFT |
			      (src_clk_div - 1) <<
			      CLK_PCLK_EDP_DIV_CON_SHIFT));
}

void rkclk_configure_mipi(void)
{
	/* Enable clk_mipidphy_ref and clk_mipidphy_cfg */
	write32(&cru_ptr->clkgate_con[11],
		RK_CLRBITS(1 << 14 | 1 << 15));
	/* Enable pclk_mipi_dsi0 */
	write32(&cru_ptr->clkgate_con[29],
		RK_CLRBITS(1 << 1));
}
