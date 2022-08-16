/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clkbuf.h>
#include <soc/emi.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/regulator.h>
#include <soc/rtc.h>

static void raise_little_cpu_freq(void)
{
	mainboard_set_regulator_voltage(MTK_REGULATOR_VPROC11, 900000);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VSRAM_PROC11, 1000000);
	udelay(200);
	mt_pll_raise_little_cpu_freq(2000 * MHz);
	mt_pll_raise_cci_freq(1600 * MHz);

	printk(BIOS_INFO, "Check CPU freq: %u KHz, cci: %u KHz\n",
	       mt_fmeter_get_freq_khz(FMETER_ABIST, 1),
	       mt_fmeter_get_freq_khz(FMETER_ABIST, 3));
}

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6315_init();
	mt6359p_init();
	raise_little_cpu_freq();
	clk_buf_init();
	rtc_boot();
	mtk_dram_init();
}
