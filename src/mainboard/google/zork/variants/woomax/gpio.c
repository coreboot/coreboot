/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <gpio.h>

static const struct soc_amd_gpio woomax_bid0_gpio_set_stage_ram[] = {
	/* GPIO_4 NC */
	PAD_NC(GPIO_4),
	/* GPIO_5 NC */
	PAD_NC(GPIO_5),
	/* GPIO_6 NC */
	PAD_NC(GPIO_6),
	/* GPIO_69 NC */
	PAD_NC(GPIO_69),
	/* RAM_ID_4  */
	PAD_NC(GPIO_84),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* TP */
	PAD_NC(GPIO_90),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
	/* GPIO_141 NC */
	PAD_NC(GPIO_141),
	/* GPIO_143 NC */
	PAD_NC(GPIO_143),
};

static const struct soc_amd_gpio woomax_gpio_set_stage_ram[] = {
	/* GPIO_4 NC */
	PAD_NC(GPIO_4),
	/* GPIO_5 NC */
	PAD_NC(GPIO_5),
	/* GPIO_6 NC */
	PAD_NC(GPIO_6),
	/* GPIO_69 NC */
	PAD_NC(GPIO_69),
	/* RAM_ID_4  */
	PAD_NC(GPIO_84),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
	/* GPIO_141 NC */
	PAD_NC(GPIO_141),
	/* GPIO_143 NC */
	PAD_NC(GPIO_143),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version;

	/* If board version cannot be read, assume it is board_version 0. */
	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		board_version = 0;

	if (board_version == 0) {
		*size = ARRAY_SIZE(woomax_bid0_gpio_set_stage_ram);
		return woomax_bid0_gpio_set_stage_ram;
	}
	*size = ARRAY_SIZE(woomax_gpio_set_stage_ram);
	return woomax_gpio_set_stage_ram;
}
