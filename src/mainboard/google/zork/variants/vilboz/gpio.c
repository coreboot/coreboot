/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <ec/google/chromeec/ec.h>

/* This table is used by dalboz variant with board version < 2. */
static const struct soc_amd_gpio bid_1_gpio_set_stage_ram[] = {
	/* DMIC_SEL */
	PAD_GPO(GPIO_6, LOW), // Select Camera 1 DMIC
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_67, HIGH),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version;

	/*
	 * If board version cannot be read, assume that this is an older revision of the board
	 * and so apply overrides. If board version is provided by the EC, then apply overrides
	 * if version < 2.
	 */
	if (google_chromeec_cbi_get_board_version(&board_version))
		board_version = 1;

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid_1_gpio_set_stage_ram);
		return bid_1_gpio_set_stage_ram;
	}

	*size = 0;
	return NULL;
}
