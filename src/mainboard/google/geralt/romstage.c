/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pmif.h>

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6315_init();
	mt6359p_init();
}
