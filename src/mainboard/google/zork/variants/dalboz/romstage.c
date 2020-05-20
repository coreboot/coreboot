/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stddef.h>
#include <soc/romstage.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <variant/gpio.h>

void variant_romstage_entry(void)
{
	uint32_t board_version;

	if (google_chromeec_cbi_get_board_version(&board_version))
		board_version = 1;

	if (board_version < 2) {
		/* SET PCIE_RST0_L HIGH */
		gpio_set(WIFI_PCIE_RESET_L, 1);
	} else {
		/* SET PCIE_RST1_L HIGH */
		gpio_set(PCIE_RST1_L, 1);
	}
}
