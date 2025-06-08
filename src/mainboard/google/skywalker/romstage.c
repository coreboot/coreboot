/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/stages.h>
#include <delay.h>
#include <soc/clkbuf_ctl.h>
#include <soc/dvfs.h>
#include <soc/emi.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pll.h>
#include <soc/pmif.h>
#include <soc/regulator.h>
#include <soc/rtc.h>
#include <soc/srclken_rc.h>
#include <soc/thermal.h>

static void raise_little_cpu_freq(void)
{
	mainboard_set_regulator_voltage(MTK_REGULATOR_VSRAM_PROC12, 1050000);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VPROC12, 1050000);
	udelay(200);
	mt_pll_raise_little_cpu_freq(2000 * MHz);
	mt_pll_raise_cci_freq(1540 * MHz);
}

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6315_init();
	mt6359p_init();
	raise_little_cpu_freq();
	rtc_boot();
	dvfs_init();
	mtk_dram_init();
	srclken_rc_init();
	clk_buf_init();
	thermal_init();
}
