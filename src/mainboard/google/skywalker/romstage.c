/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/stages.h>
#include <soc/emi.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pmif.h>
#include <soc/rtc.h>

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6315_init();
	mt6359p_init();
	rtc_boot();
	mtk_dram_init();
}
