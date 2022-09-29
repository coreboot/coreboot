/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <lib.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/i2c.h>
#include <soc/soc.h>
#include <stdint.h>
#include <string.h>

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
	.nr = _nr, .nf = (u32)((u64)hz * _nr * _no / OSC_HZ), .no = _no};\
	_Static_assert(((u64)hz * _nr * _no / OSC_HZ) * OSC_HZ /\
		       (_nr * _no) == hz, #hz "Hz cannot be hit with PLL "\
		       "divisors on line " STRINGIFY(__LINE__))

/* Keep divisors as low as possible to reduce jitter and power usage. */
static const struct pll_div gpll_init_cfg = PLL_DIVISORS(GPLL_HZ, 2, 2);
static const struct pll_div cpll_init_cfg = PLL_DIVISORS(CPLL_HZ, 1, 2);

/* See linux/drivers/clk/rockchip/clk-rk3288.c for more APLL combinations */
static const struct pll_div apll_1800_cfg = PLL_DIVISORS(1800*MHz, 1, 1);
static const struct pll_div apll_1416_cfg = PLL_DIVISORS(1416*MHz, 1, 1);
static const struct pll_div apll_600_cfg = PLL_DIVISORS(600*MHz, 1, 2);
static const struct pll_div *apll_cfgs[] = {
	[APLL_1800_MHZ] = &apll_1800_cfg,
	[APLL_1416_MHZ] = &apll_1416_cfg,
	[APLL_600_MHZ] = &apll_600_cfg,
};

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

/*******************CLKSEL1 BITS***************************/
/* pd bus clk pll sel: codec or general */
#define PD_BUS_SEL_PLL_MSK	(1 << 15)
#define PD_BUS_SEL_CPLL	(0 << 15)
#define PD_BUS_SEL_GPLL	(1 << 15)

/* pd bus pclk div:
 * pclk = pd_bus_aclk /(div + 1)
 */
#define PD_BUS_PCLK_DIV_SHIFT	(12)
#define PD_BUS_PCLK_DIV_MSK	(0x7 << 12)

/* pd bus hclk div:
 * aclk_bus: hclk_bus = 1:1 or 2:1 or 4:1
 */
#define PD_BUS_HCLK_DIV_SHIFT	(8)
#define PD_BUS_HCLK_DIV_MSK	(0x3 << 8)

/* pd bus aclk div:
 * pd_bus_aclk = pd_bus_src_clk /(div0 * div1)
 */
#define PD_BUS_ACLK_DIV0_SHIFT	(3)
#define PD_BUS_ACLK_DIV0_MASK	(0x1f << 3)
#define PD_BUS_ACLK_DIV1_SHIFT	(0)
#define PD_BUS_ACLK_DIV1_MASK	(0x7 << 0)

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
#define PERI_ACLK_DIV_SHIFT	(0x0)
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

#define NPLL_MODE_MSK	(0x3 << 14)
#define NPLL_MODE_SLOW	(0 << 14)
#define NPLL_MODE_NORM	(1 << 14)

#define SOCSTS_DPLL_LOCK	(1 << 5)
#define SOCSTS_APLL_LOCK	(1 << 6)
#define SOCSTS_CPLL_LOCK	(1 << 7)
#define SOCSTS_GPLL_LOCK	(1 << 8)
#define SOCSTS_NPLL_LOCK	(1 << 9)

#define VCO_MAX_KHZ	(2200 * (MHz/KHz))
#define VCO_MIN_KHZ	(440 * (MHz/KHz))
#define OUTPUT_MAX_KHZ	(2200 * (MHz/KHz))
#define OUTPUT_MIN_KHZ	27500
#define FREF_MAX_KHZ	(2200 * (MHz/KHz))
#define FREF_MIN_KHZ	269

static int rkclk_set_pll(u32 *pll_con, const struct pll_div *div)
{
	/* All PLLs have same VCO and output frequency range restrictions. */
	u32 vco_khz = OSC_HZ/KHz * div->nf / div->nr;
	u32 output_khz = vco_khz / div->no;

	printk(BIOS_DEBUG, "Configuring PLL at %p with NF = %d, NR = %d and "
	       "NO = %d (VCO = %uKHz, output = %uKHz)\n",
	       pll_con, div->nf, div->nr, div->no, vco_khz, output_khz);
	assert(vco_khz >= VCO_MIN_KHZ && vco_khz <= VCO_MAX_KHZ &&
	       output_khz >= OUTPUT_MIN_KHZ && output_khz <= OUTPUT_MAX_KHZ &&
	       (div->no == 1 || !(div->no % 2)));

	/* enter rest */
	write32(&pll_con[3], RK_SETBITS(PLL_RESET_MSK));

	write32(&pll_con[0],
		RK_CLRSETBITS(PLL_NR_MSK, (div->nr - 1) << PLL_NR_SHIFT) |
		RK_CLRSETBITS(PLL_OD_MSK, (div->no - 1)));

	write32(&pll_con[1], RK_CLRSETBITS(PLL_NF_MSK, (div->nf - 1)));

	write32(&pll_con[2],
		RK_CLRSETBITS(PLL_BWADJ_MSK, ((div->nf >> 1) - 1)));

	udelay(10);

	/* return form rest */
	write32(&pll_con[3], RK_CLRBITS(PLL_RESET_MSK));

	return 0;
}

void rkclk_init(void)
{
	u32 aclk_div;
	u32 hclk_div;
	u32 pclk_div;

	/* pll enter slow-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(GPLL_MODE_MSK, GPLL_MODE_SLOW) |
		RK_CLRSETBITS(CPLL_MODE_MSK, CPLL_MODE_SLOW));

	/* init pll */
	rkclk_set_pll(&cru_ptr->cru_gpll_con[0], &gpll_init_cfg);
	rkclk_set_pll(&cru_ptr->cru_cpll_con[0], &cpll_init_cfg);

	/* waiting for pll lock */
	while (1) {
		if ((read32(&rk3288_grf->soc_status[1])
			& (SOCSTS_CPLL_LOCK | SOCSTS_GPLL_LOCK))
			== (SOCSTS_CPLL_LOCK | SOCSTS_GPLL_LOCK))
			break;
		udelay(1);
	}

	/*
	 * pd_bus clock pll source selection and
	 * set up dependent divisors for PCLK/HCLK and ACLK clocks.
	 */
	aclk_div = GPLL_HZ / PD_BUS_ACLK_HZ - 1;
	assert((aclk_div + 1) * PD_BUS_ACLK_HZ == GPLL_HZ && aclk_div <= 0x1f);
	hclk_div = PD_BUS_ACLK_HZ / PD_BUS_HCLK_HZ - 1;
	assert((hclk_div + 1) * PD_BUS_HCLK_HZ ==
		PD_BUS_ACLK_HZ && (hclk_div <= 0x3) && (hclk_div != 0x2));

	pclk_div = PD_BUS_ACLK_HZ / PD_BUS_PCLK_HZ - 1;
	assert((pclk_div + 1) * PD_BUS_PCLK_HZ ==
		PD_BUS_ACLK_HZ && pclk_div <= 0x7);

	write32(&cru_ptr->cru_clksel_con[1], RK_SETBITS(PD_BUS_SEL_GPLL) |
		RK_CLRSETBITS(PD_BUS_PCLK_DIV_MSK,
			      pclk_div << PD_BUS_PCLK_DIV_SHIFT) |
		RK_CLRSETBITS(PD_BUS_HCLK_DIV_MSK,
			      hclk_div << PD_BUS_HCLK_DIV_SHIFT) |
		RK_CLRSETBITS(PD_BUS_ACLK_DIV0_MASK,
			      aclk_div << PD_BUS_ACLK_DIV0_SHIFT) |
		RK_CLRSETBITS(PD_BUS_ACLK_DIV1_MASK, 0 << 0));

	/*
	 * peri clock pll source selection and
	 * set up dependent divisors for PCLK/HCLK and ACLK clocks.
	 */
	aclk_div = GPLL_HZ / PERI_ACLK_HZ - 1;
	assert((aclk_div + 1) * PERI_ACLK_HZ == GPLL_HZ && aclk_div <= 0x1f);

	hclk_div = log2(PERI_ACLK_HZ / PERI_HCLK_HZ);
	assert((1 << hclk_div) * PERI_HCLK_HZ ==
		PERI_ACLK_HZ && (hclk_div <= 0x2));

	pclk_div = log2(PERI_ACLK_HZ / PERI_PCLK_HZ);
	assert((1 << pclk_div) * PERI_PCLK_HZ ==
		PERI_ACLK_HZ && (pclk_div <= 0x3));

	write32(&cru_ptr->cru_clksel_con[10], RK_SETBITS(PERI_SEL_GPLL) |
		RK_CLRSETBITS(PERI_PCLK_DIV_MSK,
			      pclk_div << PERI_PCLK_DIV_SHIFT) |
		RK_CLRSETBITS(PERI_HCLK_DIV_MSK,
			      hclk_div << PERI_HCLK_DIV_SHIFT) |
		RK_CLRSETBITS(PERI_ACLK_DIV_MSK,
			      aclk_div << PERI_ACLK_DIV_SHIFT));

	/* PLL enter normal-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(GPLL_MODE_MSK, GPLL_MODE_NORM) |
		RK_CLRSETBITS(CPLL_MODE_MSK, CPLL_MODE_NORM));

}

void rkclk_configure_cpu(enum apll_frequencies apll_freq)
{
	/* pll enter slow-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(APLL_MODE_MSK, APLL_MODE_SLOW));

	rkclk_set_pll(&cru_ptr->cru_apll_con[0], apll_cfgs[apll_freq]);

	/* waiting for pll lock */
	while (1) {
		if (read32(&rk3288_grf->soc_status[1]) & SOCSTS_APLL_LOCK)
			break;
		udelay(1);
	}

	/*
	 * core clock pll source selection and
	 * set up dependent divisors for MPAXI/M0AXI and ARM clocks.
	 * core clock select apll, apll clk = 1800MHz
	 * arm clk = 1800MHz, mpclk = 450MHz, m0clk = 900MHz
	 */
	write32(&cru_ptr->cru_clksel_con[0], RK_CLRBITS(CORE_SEL_PLL_MSK) |
		RK_CLRSETBITS(A12_DIV_MSK, 0 << A12_DIV_SHIFT) |
		RK_CLRSETBITS(MP_DIV_MSK, 3 << MP_DIV_SHIFT) |
		RK_CLRSETBITS(M0_DIV_MSK, 1 << 0));

	/*
	 * set up dependent divisors for L2RAM/ATCLK and PCLK clocks.
	 * l2ramclk = 900MHz, atclk = 450MHz, pclk_dbg = 450MHz
	 */
	write32(&cru_ptr->cru_clksel_con[37],
		RK_CLRSETBITS(L2_DIV_MSK, 1 << 0) |
		RK_CLRSETBITS(ATCLK_DIV_MSK, (3 << ATCLK_DIV_SHIFT)) |
		RK_CLRSETBITS(PCLK_DBG_DIV_MSK, (3 << PCLK_DBG_DIV_SHIFT)));

	/* PLL enter normal-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(APLL_MODE_MSK, APLL_MODE_NORM));
}

void rkclk_configure_ddr(unsigned int hz)
{
	struct pll_div dpll_cfg;

	switch (hz) {
	case 300*MHz:
		dpll_cfg = (struct pll_div){.nf = 50, .nr = 2, .no = 2};
		break;
	case 533*MHz:	/* actually 533.3P MHz */
		dpll_cfg = (struct pll_div){.nf = 400, .nr = 9, .no = 2};
		break;
	case 666*MHz:	/* actually 666.6P MHz */
		dpll_cfg = (struct pll_div){.nf = 500, .nr = 9, .no = 2};
		break;
	case 800*MHz:
		dpll_cfg = (struct pll_div){.nf = 100, .nr = 3, .no = 1};
		break;
	default:
		die("Unsupported SDRAM frequency, add to clock.c!");
	}

	/* pll enter slow-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(DPLL_MODE_MSK, DPLL_MODE_SLOW));

	rkclk_set_pll(&cru_ptr->cru_dpll_con[0], &dpll_cfg);

	/* waiting for pll lock */
	while (1) {
		if (read32(&rk3288_grf->soc_status[1]) & SOCSTS_DPLL_LOCK)
			break;
		udelay(1);
	}

	/* PLL enter normal-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(DPLL_MODE_MSK, DPLL_MODE_NORM));
}

void rkclk_ddr_reset(u32 ch, u32 ctl, u32 phy)
{
	u32 phy_ctl_srstn_shift = 4 + 5 * ch;
	u32 ctl_psrstn_shift = 3 + 5 * ch;
	u32 ctl_srstn_shift = 2 + 5 * ch;
	u32 phy_psrstn_shift = 1 + 5 * ch;
	u32 phy_srstn_shift = 5 * ch;

	write32(&cru_ptr->cru_softrst_con[10],
		RK_CLRSETBITS(1 << phy_ctl_srstn_shift,
			      phy << phy_ctl_srstn_shift) |
		RK_CLRSETBITS(1 << ctl_psrstn_shift, ctl << ctl_psrstn_shift) |
		RK_CLRSETBITS(1 << ctl_srstn_shift, ctl << ctl_srstn_shift) |
		RK_CLRSETBITS(1 << phy_psrstn_shift, phy << phy_psrstn_shift) |
		RK_CLRSETBITS(1 << phy_srstn_shift, phy << phy_srstn_shift));
}

void rkclk_ddr_phy_ctl_reset(u32 ch, u32 n)
{
	u32 phy_ctl_srstn_shift = 4 + 5 * ch;

	write32(&cru_ptr->cru_softrst_con[10],
		RK_CLRSETBITS(1 << phy_ctl_srstn_shift,
		n << phy_ctl_srstn_shift));
}

void rkclk_configure_spi(unsigned int bus, unsigned int hz)
{
	int src_clk_div = GPLL_HZ / hz;

	assert((src_clk_div - 1 <= 127) && (src_clk_div * hz == GPLL_HZ));

	switch (bus) {	/*select gpll as spi src clk, and set div*/
	case 0:
		write32(&cru_ptr->cru_clksel_con[25],
			RK_CLRSETBITS(1 << 7 | 0x1f << 0,
				      1 << 7 | (src_clk_div - 1) << 0));
		break;
	case 1:
		write32(&cru_ptr->cru_clksel_con[25],
			RK_CLRSETBITS(1 << 15 | 0x1f << 8,
				      1 << 15 | (src_clk_div - 1) << 8));
		break;
	case 2:
		write32(&cru_ptr->cru_clksel_con[39],
			RK_CLRSETBITS(1 << 7 | 0x1f << 0,
				      1 << 7 | (src_clk_div - 1) << 0));
		break;
	default:
		printk(BIOS_ERR, "do not support this spi bus\n");
	}
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

	/* i2s source clock: gpll
	   i2s0_outclk_sel: clk_i2s
	   i2s0_clk_sel: divider output from fraction
	   i2s0_pll_div_con: 0*/
	write32(&cru_ptr->cru_clksel_con[4],
		RK_CLRSETBITS(1 << 15 | 1 << 12 | 3 << 8 | 0x7f << 0,
			      1 << 15 | 0 << 12 | 1 << 8 | 0 << 0));

	/* set frac divider */
	v = clk_gcd(GPLL_HZ, hz);
	n = (GPLL_HZ / v) & (0xffff);
	d = (hz / v) & (0xffff);
	assert(hz == GPLL_HZ / n * d);
	write32(&cru_ptr->cru_clksel_con[8], d << 16 | n);
}

void rkclk_configure_crypto(unsigned int hz)
{
	u32 div = PD_BUS_ACLK_HZ / hz;

	assert((div - 1 <= 3) && (div * hz == PD_BUS_ACLK_HZ));
	assert(hz <= 150*MHz);	/* Suggested max in TRM. */
	write32(&cru_ptr->cru_clksel_con[26],
		RK_CLRSETBITS(0x3 << 6, (div - 1) << 6));
}

void rkclk_configure_tsadc(unsigned int hz)
{
	u32 div;
	u32 src_clk = 32 * KHz; /* tsadc source clock is 32KHz*/

	div = src_clk / hz;
	assert((div - 1 <= 63) && (div * hz == 32 * KHz));
	write32(&cru_ptr->cru_clksel_con[2],
		RK_CLRSETBITS(0x3f << 0, (div - 1) << 0));
}

static int pll_para_config(u32 freq_hz, struct pll_div *div, u32 *ext_div)
{
	u32 ref_khz = OSC_HZ / KHz, nr, nf = 0;
	u32 fref_khz;
	u32 diff_khz, best_diff_khz;
	const u32 max_nr = 1 << 6, max_nf = 1 << 12, max_no = 1 << 4;
	u32 vco_khz;
	u32 no = 1;
	u32 freq_khz = freq_hz / KHz;

	if (!freq_hz) {
		printk(BIOS_ERR, "%s: the frequency can not be 0 Hz\n", __func__);
		return -1;
	}

	no = DIV_ROUND_UP(VCO_MIN_KHZ, freq_khz);
	if (ext_div) {
		*ext_div = DIV_ROUND_UP(no, max_no);
		no = DIV_ROUND_UP(no, *ext_div);
	}

	/* only even divisors (and 1) are supported */
	if (no > 1)
		no = DIV_ROUND_UP(no, 2) * 2;

	vco_khz = freq_khz * no;
	if (ext_div)
		vco_khz *= *ext_div;

	if (vco_khz < VCO_MIN_KHZ || vco_khz > VCO_MAX_KHZ || no > max_no) {
		printk(BIOS_ERR, "%s: Cannot find out a supported VCO"
		" for Frequency (%uHz).\n", __func__, freq_hz);
		return -1;
	}

	div->no = no;

	best_diff_khz = vco_khz;
	for (nr = 1; nr < max_nr && best_diff_khz; nr++) {
		fref_khz = ref_khz / nr;
		if (fref_khz < FREF_MIN_KHZ)
			break;
		if (fref_khz > FREF_MAX_KHZ)
			continue;

		nf = vco_khz / fref_khz;
		if (nf >= max_nf)
			continue;
		diff_khz = vco_khz - nf * fref_khz;
		if (nf + 1 < max_nf && diff_khz > fref_khz / 2) {
			nf++;
			diff_khz = fref_khz - diff_khz;
		}

		if (diff_khz >= best_diff_khz)
			continue;

		best_diff_khz = diff_khz;
		div->nr = nr;
		div->nf = nf;
	}

	if (best_diff_khz > 4 * (MHz/KHz)) {
		printk(BIOS_ERR, "%s: Failed to match output frequency %u, "
		       "difference is %u Hz,exceed 4MHZ\n", __func__, freq_hz,
		       best_diff_khz * KHz);
		return -1;
	}

	return 0;
}

void rkclk_configure_edp(void)
{
	/* clk_edp_24M source: 24M */
	write32(&cru_ptr->cru_clksel_con[28], RK_SETBITS(1 << 15));

	/* rst edp */
	write32(&cru_ptr->cru_softrst_con[6], RK_SETBITS(1 << 15));
	udelay(1);
	write32(&cru_ptr->cru_softrst_con[6], RK_CLRBITS(1 << 15));
}

void rkclk_configure_hdmi(void)
{
	/* enable pclk hdmi ctrl */
	write32(&cru_ptr->cru_clkgate_con[16], RK_CLRBITS(1 << 9));

	/* software reset hdmi */
	write32(&cru_ptr->cru_softrst_con[7], RK_SETBITS(1 << 9));
	udelay(1);
	write32(&cru_ptr->cru_softrst_con[7], RK_CLRBITS(1 << 9));
}

void rkclk_configure_vop_aclk(u32 vop_id, u32 aclk_hz)
{
	u32 div;

	/* vop aclk source clk: cpll */
	div = CPLL_HZ / aclk_hz;
	assert((div - 1 <= 63) && (div * aclk_hz == CPLL_HZ));

	switch (vop_id) {
	case 0:
		write32(&cru_ptr->cru_clksel_con[31],
			RK_CLRSETBITS(3 << 6 | 0x1f << 0,
				      0 << 6 | (div - 1) << 0));
		break;

	case 1:
		write32(&cru_ptr->cru_clksel_con[31],
			RK_CLRSETBITS(3 << 14 | 0x1f << 8,
				      0 << 14 | (div - 1) << 8));
		break;
	}
}

int rkclk_configure_vop_dclk(u32 vop_id, u32 dclk_hz)
{
	struct pll_div npll_config = {0};
	u32 lcdc_div;

	if (pll_para_config(dclk_hz, &npll_config, &lcdc_div))
		return -1;

	/* npll enter slow-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(NPLL_MODE_MSK, NPLL_MODE_SLOW));

	rkclk_set_pll(&cru_ptr->cru_npll_con[0], &npll_config);

	/* waiting for pll lock */
	while (1) {
		if (read32(&rk3288_grf->soc_status[1]) & SOCSTS_NPLL_LOCK)
			break;
		udelay(1);
	}

	/* npll enter normal-mode */
	write32(&cru_ptr->cru_mode_con,
		RK_CLRSETBITS(NPLL_MODE_MSK, NPLL_MODE_NORM));

	/* vop dclk source clk: npll,dclk_div: 1 */
	switch (vop_id) {
	case 0:
		write32(&cru_ptr->cru_clksel_con[27],
			RK_CLRSETBITS(0xff << 8 | 3 << 0,
				      (lcdc_div - 1) << 8 | 2 << 0));
		break;

	case 1:
		write32(&cru_ptr->cru_clksel_con[29],
			RK_CLRSETBITS(0xff << 8 | 3 << 6,
				      (lcdc_div - 1) << 8 | 2 << 6));
		break;
	}
	return 0;
}

int rkclk_was_watchdog_reset(void)
{
	/* Bits 5 and 4 are "second" and "first" global watchdog reset. */
	return read32(&cru_ptr->cru_glb_rst_st) & 0x30;
}

unsigned int rkclk_i2c_clock_for_bus(unsigned int bus)
{
	/*i2c0,i2c2 src clk from pd_bus_pclk
	other i2c src clk from peri_pclk
	*/
	switch (bus) {
	case 0:
	case 2:
		return PD_BUS_PCLK_HZ;

	case 1:
	case 3:
	case 4:
	case 5:
		return PERI_PCLK_HZ;

	default:
		return -1; /* Should never happen. */
	}

}
