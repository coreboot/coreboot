/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_AMD_COMMON_BLOCK_PMLIB_H
#define SOC_AMD_COMMON_BLOCK_PMLIB_H

enum {
	MAINBOARD_POWER_STATE_OFF,
	MAINBOARD_POWER_STATE_ON,
	MAINBOARD_POWER_STATE_PREVIOUS,
};

/*
 * Configure power state to go into when power is reapplied.
 *
 * This function is invoked by SoC during the boot and configures the power state based on
 * selected config items.
 */
void pm_set_power_failure_state(void);

#endif /* SOC_AMD_COMMON_BLOCK_PMLIB_H */
