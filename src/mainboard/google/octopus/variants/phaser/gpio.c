/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <ec/google/chromeec/ec.h>
#include <console/console.h>

#define SKU_UNKNOWN            0xFFFFFFFF

static const struct pad_config default_override_table[] = {
	PAD_NC(GPIO_52, UP_20K),
	PAD_NC(GPIO_53, UP_20K),
	PAD_NC(GPIO_67, UP_20K),
	PAD_NC(GPIO_117, UP_20K),
	PAD_NC(GPIO_143, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 0, DEEP, NONE, Tx0RxDCRx0,
					DISPUPD),

	PAD_NC(GPIO_161, DN_20K),

	/* EN_PP3300_WLAN_L */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_178, 0, DEEP, NONE, Tx0RxDCRx0, DISPUPD),

	PAD_NC(GPIO_213, DN_20K),
	PAD_NC(GPIO_214, DN_20K),
};

static const struct pad_config sku1_default_override_table[] = {
       /* disable I2C7 SCL and SDA */
	PAD_NC(GPIO_114, UP_20K),     /* LPSS_I2C7_SDA */
	PAD_NC(GPIO_115, UP_20K),     /* LPSS_I2C7_SCL */

	PAD_NC(GPIO_52, UP_20K),
	PAD_NC(GPIO_53, UP_20K),
	PAD_NC(GPIO_67, UP_20K),
	PAD_NC(GPIO_117, UP_20K),
	PAD_NC(GPIO_143, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 0, DEEP, NONE, Tx0RxDCRx0,
					DISPUPD),

	PAD_NC(GPIO_161, DN_20K),

	/* EN_PP3300_WLAN_L */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_178, 0, DEEP, NONE, Tx0RxDCRx0,
					DISPUPD),

	PAD_NC(GPIO_213, DN_20K),
	PAD_NC(GPIO_214, DN_20K),
};

static const struct pad_config bid0_override_table[] = {
	PAD_NC(GPIO_52, UP_20K),
	PAD_NC(GPIO_53, UP_20K),
	PAD_NC(GPIO_67, UP_20K),
	PAD_NC(GPIO_117, UP_20K),
	PAD_NC(GPIO_143, UP_20K),
	PAD_NC(GPIO_161, DN_20K),
	PAD_NC(GPIO_214, DN_20K),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	const struct pad_config *c;
	uint32_t sku_id = SKU_UNKNOWN;

	switch (board_id()) {
	case 0:
	case UNDEFINED_STRAPPING_ID:
		c = bid0_override_table;
		*num = ARRAY_SIZE(bid0_override_table);
		break;
	default:
		google_chromeec_cbi_get_sku_id(&sku_id);
		if (sku_id == 1) {
			c = sku1_default_override_table;
			*num = ARRAY_SIZE(sku1_default_override_table);
		} else {
			c = default_override_table;
			*num = ARRAY_SIZE(default_override_table);
		}
	}
	return c;
}

/* GPIOs needed prior to ramstage. */
static const struct pad_config early_gpio_table[] = {
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPIO_190, NONE, DEEP),
	/* H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC_IOS(GPIO_63, NONE, DEEP, LEVEL, INVERT, TxDRxE,
		DISPUPD),
	/* H1_SLAVE_SPI_CLK_R */
	PAD_CFG_NF(GPIO_79, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_CS_L_R */
	PAD_CFG_NF(GPIO_80, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MISO */
	PAD_CFG_NF(GPIO_82, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPIO_83, NONE, DEEP, NF1),

	/* WLAN_PE_RST */
	PAD_CFG_GPO(GPIO_164, 0, DEEP),

	/*
	 * ESPI_IO1 acts as ALERT# (which is open-drain) and requies a weak
	 * pull-up for proper operation. Since there is no external pull present
	 * on this platform, configure an internal weak pull-up.
	 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_151, UP_20K, DEEP, NF2, HIZCRx1,
					ENPU),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	/*
	 * This is a hack to configure EN_PP3300_WLAN based on board id. Once
	 * board id 0 is deprecated, we can get rid of this.
	 */
	uint32_t bid = board_id();

	if (bid == UNDEFINED_STRAPPING_ID || bid < 1)
		gpio_output(GPIO_178, 1);
	else
		gpio_output(GPIO_178, 0);

	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
