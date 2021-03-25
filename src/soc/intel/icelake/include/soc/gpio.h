/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ICELAKE_GPIO_H_
#define _SOC_ICELAKE_GPIO_H_

#include <soc/gpio_defs.h>
#include <intelblocks/gpio.h>

#define CROS_GPIO_DEVICE_NAME	"INT3455:00"

/* Enable GPIO community power management configuration */
#define MISCCFG_GPIO_PM_CONFIG_BITS (MISCCFG_GPSIDEDPCGEN | \
	MISCCFG_GPRCOMPCDLCGEN | MISCCFG_GPRTCDLCGEN | MISCCFG_GSXSLCGEN \
	| MISCCFG_GPDPCGEN | MISCCFG_GPDLCGEN)

#endif
