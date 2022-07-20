/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <console/console.h>
#include <delay.h>
#include <soc/emi.h>
#include <soc/mt6366.h>
#include <soc/pll_common.h>
#include <soc/regulator.h>
#include <soc/rtc.h>

static void raise_little_cpu_freq(void)
{
	mainboard_set_regulator_voltage(MTK_REGULATOR_VPROC12, 1031250);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VSRAM_PROC12, 1118750);
	udelay(200);
	mt_pll_raise_little_cpu_freq(2000 * MHz);
	mt_pll_raise_cci_freq(1385 * MHz);

	printk(BIOS_INFO, "Check CPU freq: %u KHz, cci: %u KHz\n",
	       mt_fmeter_get_freq_khz(FMETER_ABIST, 9),
	       mt_fmeter_get_freq_khz(FMETER_ABIST, 7));
}

void platform_romstage_main(void)
{
	mt6366_init();
	raise_little_cpu_freq();
	rtc_boot();
	mtk_dram_init();
}
