/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016, 2017 Google Inc.
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

#include <stdint.h>
#include <drivers/intel/gma/opregion.h>
#include <ec/google/chromeec/ec.h>
#include "baseboard/variants.h"
#include <soc/cpu.h>
#include <soc/intel/apollolake/chip.h>
#include <soc/gpio.h>

enum {
	SKU_0_ASTRONAUT = 0,
	SKU_1_ASTRONAUT = 1,
	SKU_2_SANTA = 2,
	SKU_3_SANTA = 3,
	SKU_4_LAVA = 4,
	SKU_5_LAVA = 5,
	SKU_9_LAVA = 9,
	SKU_10_LAVA = 10,
	SKU_61_ASTRONAUT = 61,
	SKU_62_ASTRONAUT = 62,
	SKU_160_NASHER = 160,
	SKU_161_NASHER = 161,
	SKU_162_NASHER = 162,
	SKU_163_NASHER360 = 163,
	SKU_164_NASHER360 = 164,
	SKU_165_NASHER360 = 165,
	SKU_166_NASHER360 = 166,
};

uint8_t variant_board_sku(void)
{
	static int sku = -1;

	if (sku == -1)
		sku = google_chromeec_get_sku_id();

	return sku;
}

void variant_nhlt_oem_overrides(const char **oem_id,
				const char **oem_table_id,
				uint32_t *oem_revision)
{
	*oem_id = "coral";
	*oem_table_id = CONFIG_VARIANT_DIR;
	*oem_revision = variant_board_sku();
}

void mainboard_devtree_update(struct device *dev)
{
       /* Override dev tree settings per board */
	struct soc_intel_apollolake_config *cfg = dev->chip_info;
	uint8_t sku_id;

	sku_id = variant_board_sku();

	switch (sku_id) {
        case SKU_0_ASTRONAUT:
        case SKU_1_ASTRONAUT:
		cfg->usb2eye[1].Usb20PerPortPeTxiSet = 7;
		cfg->usb2eye[1].Usb20PerPortTxiSet = 2;
		break;
	case SKU_2_SANTA:
	case SKU_3_SANTA:
		cfg->usb2eye[1].Usb20PerPortPeTxiSet = 7;
		cfg->usb2eye[1].Usb20PerPortTxiSet = 2;
		break;
	case SKU_4_LAVA:
	case SKU_5_LAVA:
	case SKU_9_LAVA:
	case SKU_10_LAVA:
		cfg->usb2eye[1].Usb20PerPortPeTxiSet = 7;
		cfg->usb2eye[1].Usb20PerPortTxiSet = 2;
		break;
        case SKU_61_ASTRONAUT:
        case SKU_62_ASTRONAUT:
		cfg->usb2eye[1].Usb20PerPortPeTxiSet = 7;
		cfg->usb2eye[1].Usb20PerPortTxiSet = 5;
                break;
	default:
		break;
	}
}

const char *mainboard_vbt_filename(void)
{
	int sku_id = variant_board_sku();

	switch (sku_id) {
        case SKU_0_ASTRONAUT:
        case SKU_1_ASTRONAUT:
		return "vbt-astronaut.bin";
		break;
	case SKU_2_SANTA:
	case SKU_3_SANTA:
		return "vbt-santa.bin";
		break;
	case SKU_160_NASHER:
	case SKU_161_NASHER:
	case SKU_162_NASHER:
	case SKU_163_NASHER360:
	case SKU_164_NASHER360:
	case SKU_165_NASHER360:
	case SKU_166_NASHER360:
		return "vbt-nasher.bin";
		break;
	default:
		return "vbt.bin";
		break;
	}
}

static const struct pad_config nasher_gpio_tables[] = {
	/* AVS_DMIC_CLK_A1 */
	PAD_CFG_NF_IOSSTATE(GPIO_79, NATIVE, DEEP, NF1, Tx1RXDCRx0),
	/* AVS_DMIC_CLK_B1 */
	PAD_CFG_NF_IOSSTATE(GPIO_80, NATIVE, DEEP, NF1, Tx1RXDCRx0),
};

const struct pad_config *variant_sku_gpio_table(size_t *num)
{
	int sku_id = variant_board_sku();
	const struct pad_config *board_gpio_tables;

	switch (sku_id) {
	case SKU_160_NASHER:
	case SKU_161_NASHER:
	case SKU_162_NASHER:
	case SKU_163_NASHER360:
	case SKU_164_NASHER360:
	case SKU_165_NASHER360:
	case SKU_166_NASHER360:
		*num = ARRAY_SIZE(nasher_gpio_tables);
		board_gpio_tables = nasher_gpio_tables;
		break;
	default:
		*num = 0;
		board_gpio_tables = NULL;
		break;
	}
	return board_gpio_tables;
}
