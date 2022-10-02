/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <elog.h>
#include <pc80/mc146818rtc.h>
#include <security/vboot/vbnv.h>

#include "pmutil.h"
#include "rtc.h"

#define PCH_LPC_DEV	PCI_DEV(0, 0x1f, 0)

int rtc_failure(void)
{
	return !!(pci_read_config8(PCH_LPC_DEV, D31F0_GEN_PMCON_3) & RTC_BATTERY_DEAD);
}

void sb_rtc_init(void)
{
	int rtc_failed = rtc_failure();

	if (rtc_failed) {
		if (CONFIG(ELOG))
			elog_add_event(ELOG_TYPE_RTC_RESET);
		pci_update_config8(PCH_LPC_DEV, D31F0_GEN_PMCON_3,
				   ~RTC_BATTERY_DEAD, 0);
	}

	printk(BIOS_DEBUG, "RTC: failed = 0x%x\n", rtc_failed);

	cmos_init(rtc_failed);
}

int vbnv_cmos_failed(void)
{
	return rtc_failure();
}
