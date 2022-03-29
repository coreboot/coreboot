/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <ec/google/chromeec/ec.h>

enum {
	SKU_37_DROID = 37, /* LTE */
	SKU_38_DROID = 38, /* LTE + Touch */
	SKU_39_DROID = 39, /* LTE + KB backlight*/
	SKU_40_DROID = 40, /* LTE + Touch + KB backlight*/
};

static const struct pad_config default_override_table[] = {
	PAD_NC(GPIO_104, UP_20K),

	/* GPIO_137 -- HP_INT_ODL and would be amend by SSFC. */
	PAD_CFG_GPI_APIC_IOS(GPIO_137, NONE, DEEP, LEVEL, INVERT, HIZCRx1,
				DISPUPD),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
	/* GPIO_140 -- PEN_RESET */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_140, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),

	PAD_NC(GPIO_213, DN_20K),
};

static const struct pad_config lte_override_table[] = {
	/* Default override table. */
	PAD_NC(GPIO_104, UP_20K),

	/* GPIO_137 -- HP_INT_ODL and would be amend by SSFC. */
	PAD_CFG_GPI_APIC_IOS(GPIO_137, NONE, DEEP, LEVEL, INVERT, HIZCRx1,
				DISPUPD),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
	/* GPIO_140 -- PEN_RESET */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_140, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),

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
	case SKU_37_DROID:
	case SKU_38_DROID:
	case SKU_39_DROID:
	case SKU_40_DROID:
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

/* GPIOs needed to be set in romstage. */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	 /* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 1, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
	/* GPIO_140 -- PEN_RESET */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_140, 1, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
};

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
