/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <gpio.h>

/* GPIO configuration in ramstage */
static const struct soc_amd_gpio override_gpio_table[] = {

	/* SOC_PEN_DETECT_ODL */
	PAD_NC(GPIO_3),

	/* EN_PWR_WWAN_X */
	PAD_NC(GPIO_8),

	/* SD_AUX_RST_SOC_L */
	PAD_NC(GPIO_27),

	/* WWAN_RST_L */
	PAD_NC(GPIO_42),

};

void variant_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(override_gpio_table);
	*gpio = override_gpio_table;
}
