/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/thermal.h>

void thermal_sram_init(void)
{
	thermal_cls_sram();
	thermal_stat_cls_sram();
}
