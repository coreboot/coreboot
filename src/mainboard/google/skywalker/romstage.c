/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/stages.h>
#include <soc/emi.h>

void platform_romstage_main(void)
{
	mtk_dram_init();
}
