/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CANNONLAKE_GPIO_H_
#define _SOC_CANNONLAKE_GPIO_H_

#if CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
#include <soc/gpio_defs_cnp_h.h>
#define CROS_GPIO_DEVICE_NAME	"INT3450:00"
#else
#include <soc/gpio_defs.h>
#define CROS_GPIO_DEVICE_NAME	"INT34BB:00"
#endif
#include <intelblocks/gpio.h> /* IWYU pragma: export */

/* Enable GPIO community power management configuration */
#define MISCCFG_GPIO_PM_CONFIG_BITS (MISCCFG_GPSIDEDPCGEN | \
	MISCCFG_GPRCOMPCDLCGEN | MISCCFG_GPRTCDLCGEN | MISCCFG_GSXSLCGEN \
	| MISCCFG_GPDPCGEN | MISCCFG_GPDLCGEN)

#ifndef __ACPI__
struct pad_config;

/*
 * Routine to perform below operations:
 * 1. SoC routine to fill GPIO PM mask and value for GPIO_MISCCFG register
 * 2. Program GPIO PM configuration based on PM mask and value
 */
void soc_gpio_pm_configuration(void);
#endif

#endif
