/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_PCH_LOCKDOWN_H
#define SOC_INTEL_COMMON_PCH_LOCKDOWN_H

/*
 * This function will get lockdown config specific to soc.
 *
 * Return values:
 *  0 = CHIPSET_LOCKDOWN_FSP = use FSP's lockdown functionality to lockdown IPs
 *  1 = CHIPSET_LOCKDOWN_COREBOOT = Use coreboot to lockdown IPs
 */
int get_lockdown_config(void);

/*
 * Common PCH lockdown will perform lock down operation for DMI, FAST_SPI.
 * And SoC should implement any other PCH lockdown if applicable as
 * per silicon security guideline (i.e. LPC, PMC etc.)
 *
 * Input:
 * chipset_lockdown = Return value from get_lockdown_config() function
 */
void soc_lockdown_config(int chipset_lockdown);

#endif /* SOC_INTEL_COMMON_PCH_LOCKDOWN_H */
