/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <variant/gpio.h>
#include <smbios.h>

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

static void mainboard_init(void *chip_info)
{
	variant_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
