/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APL_GPIO_H_
#define _SOC_APL_GPIO_H_

#if CONFIG(SOC_INTEL_GEMINILAKE)
#include <soc/gpio_glk.h>
#else
#include <soc/gpio_apl.h>
#endif
#include <intelblocks/gpio.h>/* intelblocks/gpio.h depends on definitions in
				soc/gpio_glk.h and soc/gpio_apl.h */
#endif
