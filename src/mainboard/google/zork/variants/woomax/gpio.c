/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/gpio.h>

static const struct soc_amd_gpio woomax_gpio_set_stage_ram[] = {
	/* GPIO_4 NC */
	PAD_NC(GPIO_4),
	/* GPIO_5 NC */
	PAD_NC(GPIO_5),
	/* GPIO_6 NC */
	PAD_NC(GPIO_6),
	/* GPIO_11 NC */
	PAD_NC(GPIO_11),
	/* GPIO_32 NC */
	PAD_NC(GPIO_32),
	/* GPIO_69 NC */
	PAD_NC(GPIO_69),
	/* RAM_ID_4  */
	PAD_NC(GPIO_84),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
	/* GPIO_141 NC */
	PAD_NC(GPIO_141),
	/* GPIO_143 NC */
	PAD_NC(GPIO_143),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(woomax_gpio_set_stage_ram);
	return woomax_gpio_set_stage_ram;
}
