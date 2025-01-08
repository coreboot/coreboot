/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/power_limit.h>

#define NO_BATTERY_PL4_WATTS_LIMIT	45

static bool get_pd_power_watts(u32 *watts)
{
	int rv;
	enum usb_chg_type type = USB_CHG_TYPE_UNKNOWN;
	u16 volts_mv, current_ma;

	rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);
	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD. Base on max value of adapter */
		*watts = ((u32)current_ma * volts_mv) / 1000000;
		return true;
	}

	printk(BIOS_WARNING, "Cannot get PD power info. rv = %d, usb_chg_type: %d\n", rv, type);
	return false;
}

void variant_devtree_update(void)
{
	struct soc_power_limits_config *soc_config;
	u32 watts;
	u32 pl4_watts = NO_BATTERY_PL4_WATTS_LIMIT;

	soc_config = variant_get_soc_power_limit_config();
	if (soc_config == NULL)
		return;

	/*
	 * If battery is not present or battery level is at or below critical threshold
	 * to boot a platform with the power efficient configuration, limit PL4
	 * settings.
	 */
	if (!google_chromeec_is_battery_present()) {
		/* Adjust PL4 values according to current PD power */
		if (get_pd_power_watts(&watts)) {
			if (watts < NO_BATTERY_PL4_WATTS_LIMIT)
				pl4_watts = watts - 5;
			else
				pl4_watts = 40;
		}
		if (soc_config->tdp_pl4 > pl4_watts) {
			printk(BIOS_INFO, "override PL4 settings to %d watts\n", pl4_watts);
			soc_config->tdp_pl4 = pl4_watts;
		}
	}
}
