/* SPDX-License-Identifier: GPL-2.0-only */

#include <stddef.h>
#include <soc/romstage.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

#include <gpio.h>
#include <soc/gpio.h>
#include <variant/gpio.h>

void variant_romstage_entry(void)
{
	/* Power the wifi card */
	gpio_set(EN_PWR_WIFI, 1);
}

static const struct soc_amd_gpio berknip_gpio_set_wifi[] = {
	/* EN_PWR_WIFI - Power off. Pull high in romstage.c */
	PAD_GPO(GPIO_29, LOW),
};

const struct soc_amd_gpio *variant_wifi_romstage_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(berknip_gpio_set_wifi);
	return berknip_gpio_set_wifi;
}
