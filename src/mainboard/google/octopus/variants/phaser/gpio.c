/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>

#define SKU_UNKNOWN            0xFFFFFFFF

static const struct pad_config default_override_table[] = {
	PAD_NC(GPIO_52, UP_20K),
	PAD_NC(GPIO_53, UP_20K),
	PAD_NC(GPIO_67, UP_20K),
	PAD_NC(GPIO_117, UP_20K),
	PAD_CFG_GPI_APIC_IOS(GPIO_137, NONE, DEEP, LEVEL, INVERT, HIZCRx1,
					DISPUPD),
	PAD_NC(GPIO_143, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
	/* GPIO_140 -- PEN_RESET */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_140, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),

	PAD_NC(GPIO_161, DN_20K),

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
	PAD_CFG_GPI_APIC_IOS(GPIO_137, NONE, DEEP, LEVEL, INVERT, HIZCRx1,
					DISPUPD),
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

bool no_touchscreen_sku(uint32_t sku_id)
{
	if ((sku_id == 1) || (sku_id == 6))
		return true;
	else
		return false;
}

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	const struct pad_config *c;
	uint32_t sku_id = SKU_UNKNOWN;

	google_chromeec_cbi_get_sku_id(&sku_id);
	if (no_touchscreen_sku(sku_id)) {
		c = sku1_default_override_table;
		*num = ARRAY_SIZE(sku1_default_override_table);
	} else {
		c = default_override_table;
		*num = ARRAY_SIZE(default_override_table);
	}

	return c;
}


/* Touchscreen GPIOs needed to be set in romstage. */
static const struct pad_config romstage_touch_gpio_table[] = {
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
	const struct pad_config *c;
	uint32_t sku_id = SKU_UNKNOWN;

	google_chromeec_cbi_get_sku_id(&sku_id);
	if (no_touchscreen_sku(sku_id)) {
		c = NULL;
		*num = 0;
	} else {
		c = romstage_touch_gpio_table;
		*num = ARRAY_SIZE(romstage_touch_gpio_table);
	}

	return c;
}
