/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <mainboard/google/auron/variant.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <smbios.h>
#include <variant/board_version.h>
#include <variant/onboard.h>

const char *smbios_mainboard_version(void)
{
	return samus_board_version();
}

int variant_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	/* N/A for SAMUS */
	return 0;
}

void mainboard_post_raminit(const int s3resume)
{
	if (!s3resume)
		google_chromeec_kbbacklight(100);

	printk(BIOS_INFO, "MLB: board version %s\n", samus_board_version());

	/* Bring SSD out of reset */
	gpio_set(BOARD_SSD_RESET_GPIO, 1);

	/*
	 * Enable PP3300_AUTOBAHN_EN after initial GPIO setup
	 * to prevent possible brownout.
	 */
	gpio_set(BOARD_PP3300_AUTOBAHN_GPIO, 1);
}
