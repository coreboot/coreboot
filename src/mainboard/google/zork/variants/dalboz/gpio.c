/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>

/* This table is used by dalboz variant with board version < 2. */
static const struct soc_amd_gpio bid_1_gpio_set_stage_ram[] = {
	/* DMIC_SEL */
	PAD_GPO(GPIO_6, LOW), // Select Camera 1 DMIC
	/* USB_OC2_L - USB A0 & A1 */
	PAD_NF(GPIO_18, USB_OC2_L, PULL_NONE),
	/* TP */
	PAD_NC(GPIO_32),
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_67, HIGH),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
	/* Unused */
	PAD_NC(GPIO_143),
};

static const struct soc_amd_gpio bid_2_gpio_set_stage_ram[] = {
	/* DMIC_SEL */
	PAD_GPO(GPIO_6, LOW), // Select Camera 1 DMIC
	/* TP */
	PAD_NC(GPIO_32),
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_67, HIGH),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
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
	} else if (board_version == 2) {
		*size = ARRAY_SIZE(bid_2_gpio_set_stage_ram);
		return bid_2_gpio_set_stage_ram;
	}

	*size = 0;
	return NULL;
}
