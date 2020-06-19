/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <boardid.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/mt6391.h>
#include <soc/pll.h>
#include <soc/rtc.h>
#include <timer.h>

void platform_romstage_main(void)
{
	int stabilize_usec;
	struct stopwatch sw;

	rtc_boot();

	/* Raise CPU voltage to allow higher frequency */
	stabilize_usec = mt6391_configure_ca53_voltage(1125000);

	stopwatch_init_usecs_expire(&sw, stabilize_usec);

	/* init memory */
	mt_mem_init(get_sdram_config());

	stopwatch_wait_until_expired(&sw);

	/* Set to maximum frequency */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		mt_pll_raise_little_cpu_freq(1600 * MHz);
	else
		mt_pll_raise_little_cpu_freq(1700 * MHz);

	mtk_mmu_after_dram();
}
