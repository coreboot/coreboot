/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>

static const struct soc_amd_gpio bid_1_gpio_set_stage_ram[] = {
	/* TP */
	PAD_NC(GPIO_32),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
};

static const struct soc_amd_gpio vilboz_gpio_set_stage_ram[] = {
	/* P sensor INT */
	PAD_INT(GPIO_40, PULL_NONE, LEVEL_LOW, STATUS_DELIVERY),
	/* LTE_RST_L */
	PAD_GPO(GPIO_89, HIGH),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version;

	/*
	 * If board version cannot be read, assume that this is an older revision of the board
	 * and so apply overrides. If board version is provided by the EC, then apply overrides
	 * if version < 2.
	 */
	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		board_version = 1;

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid_1_gpio_set_stage_ram);
		return bid_1_gpio_set_stage_ram;
	}

	*size = ARRAY_SIZE(vilboz_gpio_set_stage_ram);
	return vilboz_gpio_set_stage_ram;
}
