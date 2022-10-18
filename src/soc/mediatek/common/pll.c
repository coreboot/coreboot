/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <soc/pll.h>
#include <types.h>

void pll_mux_set_sel(const struct mux *mux, u32 sel)
{
	u32 mask = GENMASK(mux->mux_width - 1, 0);
	u32 val = read32(mux->reg);

	if (mux->set_reg && mux->clr_reg) {
		write32(mux->clr_reg, mask << mux->mux_shift);
		write32(mux->set_reg, sel << mux->mux_shift);
	} else {
		val &= ~(mask << mux->mux_shift);
		val |= (sel & mask) << mux->mux_shift;
		write32(mux->reg, val);
	}

	if (mux->upd_reg)
		write32(mux->upd_reg, 1 << mux->upd_shift);
}

static void pll_calc_values(const struct pll *pll, u32 *pcw, u32 *postdiv,
			    u32 freq)
{
	const u32 fin_hz = CLK26M_HZ;
	const u32 *div_rate = pll->div_rate;
	u32 val;

	assert(freq <= div_rate[0]);
	assert(freq >= 1 * GHz / 16);

	for (val = 1; div_rate[val] != 0; val++) {
		if (freq > div_rate[val])
			break;
	}
	val--;
	*postdiv = val;

	/* _pcw = freq * 2^postdiv / fin * 2^pcwbits_fractional */
	val += pll->pcwbits - PCW_INTEGER_BITS;

	*pcw = ((u64)freq << val) / fin_hz;
}

static void pll_set_rate_regs(const struct pll *pll, u32 pcw, u32 postdiv)
{
	u32 val;

	/* set postdiv */
	val = read32(pll->div_reg);
	val &= ~(PLL_POSTDIV_MASK << pll->div_shift);
	val |= postdiv << pll->div_shift;

	/* set postdiv and pcw at the same time if on the same register */
	if (pll->div_reg != pll->pcw_reg) {
		write32(pll->div_reg, val);
		val = read32(pll->pcw_reg);
	}

	/* set pcw */
	val &= ~GENMASK(pll->pcw_shift + pll->pcwbits - 1, pll->pcw_shift);
	val |= pcw << pll->pcw_shift;
	write32(pll->pcw_reg, val);

	pll_set_pcw_change(pll);
}

int pll_set_rate(const struct pll *pll, u32 rate)
{
	u32 pcw, postdiv;

	pll_calc_values(pll, &pcw, &postdiv, rate);
	pll_set_rate_regs(pll, pcw, postdiv);

	return 0;
}
