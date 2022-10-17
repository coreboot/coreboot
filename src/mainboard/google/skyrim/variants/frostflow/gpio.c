/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* GPIO configuration in ramstage */
static const struct soc_amd_gpio override_gpio_table[] = {
	/* EN_PWR_WWAN_X => CAM_PSW_L */
	PAD_INT(GPIO_8, PULL_NONE, BOTH_EDGES, STATUS_DELIVERY),
	/* SOC_SAR_INT_L => Unused */
	PAD_NC(GPIO_17),
	/* WWAN_RST_L => Unused */
	PAD_NC(GPIO_42),
};

void variant_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(override_gpio_table);
	*gpio = override_gpio_table;
}
