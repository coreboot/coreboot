/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_GPIO_H_
#define _SOC_GPIO_H_

#include <soc/gpio_soc_defs.h>
#include <intelblocks/gpio.h> /* IWYU pragma: export */

/*
 * XEON SP series doesn't support dynamic GPIO PM hence GPIO community
 * MISCCFG register doesn't have PM bits
 */
#define MISCCFG_GPIO_PM_CONFIG_BITS 0

#endif
