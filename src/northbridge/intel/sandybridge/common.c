/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include "sandybridge.h"

enum platform_type get_platform_type(void)
{
	const int id = get_platform_id();
	if (id != 1 && id != 4)
		printk(BIOS_WARNING, "WARN: Unknown platform id 0x%x\n", id);

	return (id == 4) ? PLATFORM_MOBILE : PLATFORM_DESKTOP_SERVER;
}
