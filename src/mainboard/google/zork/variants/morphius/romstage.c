/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stddef.h>
#include <soc/romstage.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

#include <gpio.h>
#include <soc/gpio.h>
#include <variant/gpio.h>
#include <delay.h>

void variant_romstage_entry(void)
{
	//SET WIFI_PCIE_RESET_L HIGH
	gpio_set(WIFI_PCIE_RESET_L, 1);

	/* Power the wifi card */
	/* wait 10ms to discharge EN_PWR_WIFI to 0V */
	mdelay(10);
	gpio_set(EN_PWR_WIFI, 1);

	/* SET WIFI_PCIE_RESET_L LOW */
	gpio_set(WIFI_PCIE_RESET_L, 0);

	/* Qualcomm Atheros NFA344A needs at least 10ms delay */
	mdelay(10);

	/* SET WIFI_PCIE_RESET_L HIGH */
	gpio_set(WIFI_PCIE_RESET_L, 1);
}

static const struct soc_amd_gpio morphius_gpio_set_wifi[] = {
	/* EN_PWR_WIFI - Power off. Pull high in romstage.c */
	PAD_GPO(GPIO_29, LOW),
};

const struct soc_amd_gpio *variant_wifi_romstage_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(morphius_gpio_set_wifi);
	return morphius_gpio_set_wifi;
}
