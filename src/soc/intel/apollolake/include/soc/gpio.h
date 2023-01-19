/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APL_GPIO_H_
#define _SOC_APL_GPIO_H_

#if CONFIG(SOC_INTEL_GEMINILAKE)
#include <soc/gpio_glk.h>
#else
#include <soc/gpio_apl.h>
#endif
/* intelblocks/gpio.h depends on definitions in soc/gpio_glk.h and soc/gpio_apl.h */
#include <intelblocks/gpio.h> /* IWYU pragma: export */

/*
 * APL/GLK doesn't support dynamic GPIO PM hence GPIO community
 * MISCCFG register doesn't have PM bits
 */
#define MISCCFG_GPIO_PM_CONFIG_BITS 0

#endif
