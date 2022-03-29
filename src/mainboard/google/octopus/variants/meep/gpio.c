/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <variant/sku.h>
#include <ec/google/chromeec/ec.h>

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
};

static const struct pad_config hdmi_sku_override_table[] = {
	PAD_NC(GPIO_104, UP_20K),

	/* HV_DDI1_DDC_SDA */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_126, NONE, DEEP, NF1, HIZCRx1,
					DISPUPD),
	/* HV_DDI1_DDC_SCL */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_127, NONE, DEEP, NF1, HIZCRx1,
					DISPUPD),

	/* GPIO_137 -- HP_INT_ODL and would be amend by SSFC. */
	PAD_CFG_GPI_APIC_IOS(GPIO_137, NONE, DEEP, LEVEL, INVERT, HIZCRx1,
				DISPUPD),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
	/* GPIO_140 -- PEN_RESET */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_140, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	uint32_t sku_id;
	sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_33_DORP:
	case SKU_34_DORP:
	case SKU_35_DORP:
	case SKU_36_DORP:
		*num = ARRAY_SIZE(hdmi_sku_override_table);
		return hdmi_sku_override_table;
	default:
		*num = ARRAY_SIZE(default_override_table);
		return default_override_table;
	}
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
