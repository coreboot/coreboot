/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_PLL_COMMON_H
#define SOC_MEDIATEK_PLL_COMMON_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <types.h>

/* These need to be macros for use in static initializers. */
#define mtk_topckgen ((struct mtk_topckgen_regs *)CKSYS_BASE)
#define mtk_apmixed  ((struct mtk_apmixed_regs *)APMIXED_BASE)

#define PLL_PWR_ON		(1 << 0)
#define PLL_EN			(1 << 0)
#define PLL_ISO			(1 << 1)
#define PLL_RSTB_SHIFT		(24)
#define NO_RSTB_SHIFT		(255)
#define PLL_PCW_CHG		(1 << 31)
#define PLL_POSTDIV_MASK	0x7

struct mux {
	void *reg;
	void *set_reg;
	void *clr_reg;
	void *upd_reg;
	u8 mux_shift;
	u8 mux_width;
	u8 upd_shift;
};

struct pll {
	void *reg;
	void *pwr_reg;
	void *div_reg;
	void *pcw_reg;
	const u32 *div_rate;
	u8 rstb_shift;
	u8 pcwbits;
	u8 div_shift;
	u8 pcw_shift;
};

#define PLL(_id, _reg, _pwr_reg, _rstb, _pcwbits, _div_reg, _div_shift,	\
			_pcw_reg, _pcw_shift, _div_rate)		\
	[_id] = {							\
		.reg = &mtk_apmixed->_reg,				\
		.pwr_reg = &mtk_apmixed->_pwr_reg,			\
		.rstb_shift = _rstb,					\
		.pcwbits = _pcwbits,					\
		.div_reg = &mtk_apmixed->_div_reg,			\
		.div_shift = _div_shift,				\
		.pcw_reg = &mtk_apmixed->_pcw_reg,			\
		.pcw_shift = _pcw_shift,				\
		.div_rate = _div_rate,					\
	}

/* every PLL can share the same POWER_EN/ISO_EN/EN bits, use the common BITFIELD macro */
DEFINE_BIT(PLL_ENABLE, 0)

DEFINE_BIT(PLL_POWER_ENABLE, 0)
DEFINE_BIT(PLL_ISO_ENABLE, 1)
DEFINE_BITFIELD(PLL_POWER_ISO_ENABLE, 1, 0)

DEFINE_BITFIELD(PLL_CON1, 31, 0)

void pll_set_pcw_change(const struct pll *pll);
void mux_set_sel(const struct mux *mux, u32 sel);
int pll_set_rate(const struct pll *pll, u32 rate);
void mt_pll_init(void);
void mt_pll_raise_little_cpu_freq(u32 freq);
void mt_pll_raise_cci_freq(u32 freq);
void mt_pll_set_tvd_pll1_freq(u32 freq);
void edp_mux_set_sel(u32 sel);
void mt_pll_set_usb_clock(void);

enum fmeter_type {
	FMETER_ABIST = 0,
	FMETER_CKGEN,
};
u32 mt_fmeter_get_freq_khz(enum fmeter_type type, u32 id);

#endif
