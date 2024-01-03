/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <vendorcode/google/chromeos/chromeos.h>

const char *bmp_logo_filename(void)
{
	if (chromeos_device_branded_plus())
		return "cb_plus_logo.bmp";
	else
		return "cb_logo.bmp";
}
