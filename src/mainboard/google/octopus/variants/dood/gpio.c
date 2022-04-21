/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <ec/google/chromeec/ec.h>

enum {
	SKU_1_LTE  = 1, /* Wifi + LTE */
	SKU_2_WIFI = 2, /* Wifi */
	SKU_3_LTE_2CAM = 3, /* Wifi + LTE + dual camera */
	SKU_4_WIFI_2CAM = 4, /* Wifi + dual camera */
};

static const struct pad_config default_override_table[] = {
	PAD_NC(GPIO_104, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 0, DEEP, NONE, Tx0RxDCRx0,
				     DISPUPD),

	PAD_NC(GPIO_213, DN_20K),
};

static const struct pad_config lte_override_table[] = {
	/* Default override table. */
	PAD_NC(GPIO_104, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 0, DEEP, NONE, Tx0RxDCRx0,
				     DISPUPD),

	PAD_NC(GPIO_213, DN_20K),

	/* Be specific to LTE SKU */
	/* UART2-CTS_B -- EN_PP3300_DX_LTE_SOC */
	PAD_CFG_GPO(GPIO_67, 1, PWROK),

	/* PCIE_WAKE1_B -- FULL_CARD_POWER_OFF */
	PAD_CFG_GPO(GPIO_117, 1, PWROK),

	/* AVS_I2S1_MCLK -- PLT_RST_LTE_L */
	PAD_CFG_GPO(GPIO_161, 1, DEEP),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	uint32_t sku_id;
	sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_1_LTE:
	case SKU_3_LTE_2CAM:
		*num = ARRAY_SIZE(lte_override_table);
		return lte_override_table;
	default:
		*num = ARRAY_SIZE(default_override_table);
		return default_override_table;
	}
}

static const struct pad_config lte_early_override_table[] = {
	/* UART2-CTS_B -- EN_PP3300_DX_LTE_SOC */
	PAD_CFG_GPO(GPIO_67, 1, PWROK),

	/* PCIE_WAKE1_B -- FULL_CARD_POWER_OFF */
	PAD_CFG_GPO(GPIO_117, 1, PWROK),

	/* AVS_I2S1_MCLK -- PLT_RST_LTE_L */
	PAD_CFG_GPO(GPIO_161, 0, DEEP),
};

const struct pad_config *variant_early_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(lte_early_override_table);

	return lte_early_override_table;
}
