/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>

#define SKU_UNKNOWN            0xFFFFFFFF

static const struct pad_config default_override_table[] = {
       /* disable I2C7 SCL and SDA */
	PAD_NC(GPIO_114, UP_20K),     /* LPSS_I2C7_SDA */
	PAD_NC(GPIO_115, UP_20K),     /* LPSS_I2C7_SCL */

	PAD_NC(GPIO_52, UP_20K),
	PAD_NC(GPIO_53, UP_20K),
	PAD_NC(GPIO_67, UP_20K),
	PAD_NC(GPIO_117, UP_20K),
	PAD_NC(GPIO_143, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 0, DEEP, NONE, Tx0RxDCRx0, DISPUPD),

	PAD_NC(GPIO_161, DN_20K),

	/* EN_PP3300_WLAN_L */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_178, 0, DEEP, NONE, Tx0RxDCRx0, DISPUPD),

	PAD_NC(GPIO_213, DN_20K),
	PAD_NC(GPIO_214, DN_20K),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(default_override_table);
	return default_override_table;
}
