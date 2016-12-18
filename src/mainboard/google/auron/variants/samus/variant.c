/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <smbios.h>
#include <variant/board_version.h>
#include <variant/onboard.h>
#include <mainboard/google/auron/variant.h>

const char *smbios_mainboard_version(void)
{
	return samus_board_version();
}

int variant_smbios_data(device_t dev, int *handle,
				 unsigned long *current)
{
	/* N/A for SAMUS */
	return 0;
}

void variant_romstage_entry(struct romstage_params *rp)
{
	if (rp->power_state->prev_sleep_state != ACPI_S3)
		google_chromeec_kbbacklight(100);

	printk(BIOS_INFO, "MLB: board version %s\n", samus_board_version());

	/* Bring SSD out of reset */
	set_gpio(BOARD_SSD_RESET_GPIO, GPIO_OUT_HIGH);

	/*
	 * Enable PP3300_AUTOBAHN_EN after initial GPIO setup
	 * to prevent possible brownout.
	 */
	set_gpio(BOARD_PP3300_AUTOBAHN_GPIO, GPIO_OUT_HIGH);
}