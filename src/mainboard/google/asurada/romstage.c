/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <delay.h>
#include <soc/clkbuf.h>
#include <soc/emi.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/rtc.h>
#include <soc/srclken_rc.h>

static void raise_little_cpu_freq(void)
{
	mt6359p_buck_set_voltage(MT6359P_SRAM_PROC2, 1000 * 1000);
	mt6315_buck_set_voltage(MT6315_CPU, MT6315_BUCK_3, 925 * 1000);
	udelay(200);
	mt_pll_raise_little_cpu_freq(2000 * MHz);
	mt_pll_raise_cci_freq(1400 * MHz);
}

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6359p_init();
	mt6315_init();
	srclken_rc_init();
	clk_buf_init();
	rtc_boot();
	raise_little_cpu_freq();
	mtk_dram_init();
}
