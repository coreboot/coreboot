/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <boardid.h>
#include <sar.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>

enum {
	SKU_37_DROID = 37, /* LTE */
	SKU_38_DROID = 38, /* LTE + Touch */
	SKU_39_DROID = 39, /* LTE + KB backlight*/
	SKU_40_DROID = 40, /* LTE + Touch + KB backlight*/
};

struct gpio_with_delay {
	gpio_t gpio;
	unsigned int delay_msecs;
};

static void power_off_lte_module(u8 slp_typ)
{
	const struct gpio_with_delay lte_power_off_gpios[] = {
		{
			GPIO_161, /* AVS_I2S1_MCLK -- PLT_RST_LTE_L */
			30,
		},
		{
			GPIO_117, /* PCIE_WAKE1_B -- FULL_CARD_POWER_OFF */
			100
		},
		{
			GPIO_67, /* UART2-CTS_B -- EN_PP3300_DX_LTE_SOC */
			0
		}
	};

	for (int i = 0; i < ARRAY_SIZE(lte_power_off_gpios); i++) {
		gpio_output(lte_power_off_gpios[i].gpio, 0);
		mdelay(lte_power_off_gpios[i].delay_msecs);
	}
}

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id = get_board_sku();

	if (sku_id == 33 || sku_id == 34 || sku_id == 35 || sku_id == 36 || sku_id == 41 ||
		sku_id == 42 || sku_id == 43 || sku_id == 44)
		filename = "wifi_sar-droid.hex";

	return filename;
}

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	switch (get_board_sku()) {
	case SKU_37_DROID:
	case SKU_38_DROID:
	case SKU_39_DROID:
	case SKU_40_DROID:
		power_off_lte_module(slp_typ);
		return;
	default:
		return;
	}
}
