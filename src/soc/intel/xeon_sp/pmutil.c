/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Helper functions for dealing with power management registers
 * and the differences between PCH variants.
 */

#include <intelblocks/rtc.h>
#include <soc/pmc.h>
#include <soc/pci_devs.h>
#include <device/pci.h>
#include <console/console.h>

int soc_get_rtc_failed(void)
{
	uint32_t pmcon_b = pci_s_read_config32(PCH_DEV_PMC, GEN_PMCON_B);
	int rtc_fail = !!(pmcon_b & RTC_BATTERY_DEAD);

	if (rtc_fail)
		printk(BIOS_ERR, "%s: RTC battery dead or removed\n", __func__);

	return rtc_fail;
}
