/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>

void variant_isst_override(void)
{
	config_t *cfg = config_of_soc();
	uint32_t board_ver;

	/* Override/Disable ISST in boards where board version is not populated. */
	if (google_chromeec_get_board_version(&board_ver))
		cfg->speed_shift_enable = 0;
}
