/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_OC_WDT_H
#define SOC_INTEL_COMMON_OC_WDT_H

#include <stdbool.h>

/* Starts and reloads the OC watchdog if enabled in Kconfig */
void setup_oc_wdt(void);

/* Reloads the OC watchdog (if enabled) preserving the current settings. */
void oc_wdt_reload(void);

/* Returns currently programmed OC watchdog timeout in seconds */
unsigned int oc_wdt_get_current_timeout(void);

#endif
