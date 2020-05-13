/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>

bool variant_mem_is_half_populated(void)
{
	uint32_t board_ver;

	/* On boards where board version is populated, ram strap is also populated */
	if (!google_chromeec_get_board_version(&board_ver))
		return !gpio_get(GPIO_MEM_CH_SEL);

	return false;
}
