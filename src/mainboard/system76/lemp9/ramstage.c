/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "gpio.h"
#include <smbios.h>

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
