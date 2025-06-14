/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <vendorcode/google/chromeos/chromeos.h>

const char *bmp_logo_filename(void)
{
	/*
	 * If the device has a custom boot splash footer logo (SPLASH_SCREEN_FOOTER Kconfig
	 * is enabled and a logo path is provided), we only return the main OEM logo, which
	 * will be displayed in the center of the splash screen.
	 */
	if (CONFIG(SPLASH_SCREEN_FOOTER))
		return "cb_logo.bmp";

	if (chromeos_device_branded_plus_hard() || chromeos_device_branded_plus_soft())
		return "cb_plus_logo.bmp";

	return "cb_logo.bmp";
}
