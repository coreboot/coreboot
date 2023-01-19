/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_GPIO_H_
#define _SOC_GPIO_H_

#include <soc/gpio_defs.h>
/* intelblocks/gpio.h depends on definitions in soc/gpio_defs.h */
#include <intelblocks/gpio.h> /* IWYU pragma: export */

#define CROS_GPIO_DEVICE_NAME	"INT344B:00"

/*
 * SPT PCH doesn't support dynamic GPIO PM hence GPIO community
 * MISCCFG register doesn't have PM bits
 */
#define MISCCFG_GPIO_PM_CONFIG_BITS 0

#endif
