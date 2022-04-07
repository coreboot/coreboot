/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ALDERLAKE_GPIO_H_
#define _SOC_ALDERLAKE_GPIO_H_

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
#include <soc/gpio_defs_pch_s.h>
#define CROS_GPIO_NAME		"INTC1056"
#define CROS_GPIO_DEVICE_NAME	"INTC1056:00"
#elif CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
#include <soc/gpio_defs.h>
#define CROS_GPIO_NAME		"INTC1057"
#define CROS_GPIO_DEVICE_NAME	"INTC1057:00"
#else
#include <soc/gpio_defs.h>
#define CROS_GPIO_NAME		"INTC1055"
#define CROS_GPIO_DEVICE_NAME	"INTC1055:00"
#endif

#include <intelblocks/gpio.h>

/* Enable GPIO community power management configuration */
#define MISCCFG_GPIO_PM_CONFIG_BITS (MISCCFG_GPVNNREQEN | \
	MISCCFG_GPPGCBDPCGEN | MISCCFG_GPSIDEDPCGEN | \
	MISCCFG_GPRCOMPCDLCGEN | MISCCFG_GPRTCDLCGEN | MISCCFG_GSXSLCGEN \
	| MISCCFG_GPDPCGEN | MISCCFG_GPDLCGEN)

#endif
