/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ELKHARTLAKE_GPIO_H_
#define _SOC_ELKHARTLAKE_GPIO_H_

#include <soc/gpio_defs.h>
#include <intelblocks/gpio.h> /* IWYU pragma: export */

#define CROS_GPIO_NAME		"INTC1020"
#define CROS_GPIO_DEVICE_NAME	"INTC1020:00"

/* Enable GPIO community power management configuration */
#define MISCCFG_GPIO_PM_CONFIG_BITS (MISCCFG_GPVNNREQEN | \
	MISCCFG_GPPGCBDPCGEN | MISCCFG_GPSIDEDPCGEN | \
	MISCCFG_GPRCOMPCDLCGEN | MISCCFG_GPRTCDLCGEN | MISCCFG_GSXSLCGEN \
	| MISCCFG_GPDPCGEN | MISCCFG_GPDLCGEN)

#endif
