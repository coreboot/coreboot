/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_JASPERLAKE_GPIO_H_
#define _SOC_JASPERLAKE_GPIO_H_

#include <soc/gpio_defs.h>
#include <intelblocks/gpio.h> /* IWYU pragma: export */

#define CROS_GPIO_NAME		"INT34C8"
#define CROS_GPIO_COMM0_NAME    "INT34C8:00"
#define CROS_GPIO_COMM1_NAME    "INT34C8:01"
#define CROS_GPIO_COMM4_NAME    "INT34C8:02"
#define CROS_GPIO_COMM5_NAME    "INT34C8:03"

/* Enable GPIO community power management configuration */
#define MISCCFG_GPIO_PM_CONFIG_BITS (MISCCFG_GPVNNREQEN | \
	MISCCFG_GPPGCBDPCGEN | MISCCFG_GPSIDEDPCGEN | \
	MISCCFG_GPRCOMPCDLCGEN | MISCCFG_GPRTCDLCGEN | MISCCFG_GSXSLCGEN \
	| MISCCFG_GPDPCGEN | MISCCFG_GPDLCGEN)

#endif
