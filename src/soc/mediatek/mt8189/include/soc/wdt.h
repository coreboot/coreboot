/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 8.2
 */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_WDT_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_WDT_H__

#include <soc/wdt_common.h>

#define MTK_WDT_SPM_THERMAL_VAL 1

DEFINE_BIT(MTK_WDT_SPM_THERMAL_EN, 0)
DEFINE_BIT(MTK_WDT_THERMAL_EN, 18)
DEFINE_BIT(MTK_WDT_THERMAL_IRQ, 18)

#endif /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_WDT_H__ */
