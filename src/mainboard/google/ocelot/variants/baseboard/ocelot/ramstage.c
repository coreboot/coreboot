/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

/*
 * Placeholder to check if variant has support for barrel jack for powering
 * on the device.
 *
 * Most of the chromebook device is powering on with USB-C hence, unless overridden
 * by some variant, assume barrel jack not present.
 */
__weak bool variant_is_barrel_charger_present(void)
{
	return false;
}

void baseboard_devtree_update(void)
{
	/* Don't optimize the power limit if booting with barrel attached */
	if (variant_is_barrel_charger_present())
		return;

	/* TODO: Add power limit override code for Wildcat Lake */
	if (!google_chromeec_is_battery_present())
		printk(BIOS_DEBUG, "TODO: Add support for power optimized boot configuration limits\n");
}
