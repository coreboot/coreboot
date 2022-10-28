/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_OC_WDT_H
#define SOC_INTEL_COMMON_OC_WDT_H

#include <stdbool.h>

/*
 * Starts and reloads the OC watchdog with given timeout.
 *
 * timeout - Time in seconds before OC watchdog times out. Supported range = 70 - 1024
 */
void oc_wdt_start(unsigned int timeout);

/* Reloads the OC watchdog (if enabled) preserving the current settings. */
void oc_wdt_reload(void);

/* Disables the OC WDT */
void oc_wdt_disable(void);

/* Checks if OC WDT is enabled and returns true if so, otherwise false */
bool is_oc_wdt_enabled(void);

/* Returns currently programmed OC watchdog timeout in seconds */
unsigned int oc_wdt_get_current_timeout(void);

#endif
