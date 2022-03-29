/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>

static const struct pad_config default_override_table[] = {

	PAD_NC(GPIO_50, UP_20K), /* PCH_I2C_PEN_SDA -- unused */
	PAD_NC(GPIO_51, UP_20K), /* PCH_I2C_PEN_SCL -- unused */
	PAD_NC(GPIO_52, UP_20K), /* PCH_I2C_P_SENSOR_SDA -- unused */
	PAD_NC(GPIO_53, UP_20K), /* PCH_I2C_P_SENSOR_SCL -- unused */

	PAD_NC(GPIO_67, UP_20K), /* EN_PP3300_DX_LTE_SOC -- unused */

	PAD_NC(GPIO_117, UP_20K), /* PCIE_WAKE1_B -- LTE_WAKE_L - No LTE*/

	PAD_NC(GPIO_138, UP_20K), /* PEN_PDCT_ODL -- unused */
	PAD_NC(GPIO_139, UP_20K), /* PEN_INT_ODL -- unused */
	PAD_NC(GPIO_140, UP_20K), /* PEN_RESET -- unused */

	PAD_NC(GPIO_143, UP_20K), /* LTE_SAR_ODL -- unused */
	PAD_NC(GPIO_144, UP_20K), /* PEN_EJECT(wake) -- unused */
	PAD_NC(GPIO_145, UP_20K), /* PEN_EJECT(notification) -- unused */

	PAD_NC(GPIO_161, UP_20K), /* LTE_OFF_ODL -- unused */

	/* GPIO_137 -- HP_INT_ODL and would be amend by SSFC. */
	PAD_CFG_GPI_APIC_IOS(GPIO_137, NONE, DEEP, LEVEL, INVERT, HIZCRx1,
				DISPUPD),

	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 0, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0,
				     DISPUPD),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(default_override_table);
	return default_override_table;
}

/* GPIOs needed to be set in romstage. */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	 /* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 1, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
	/* GPIO_105 -- TOUCHSCREEN_RST */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_105, 1, DEEP, NONE, Tx1RxDCRx0, DISPUPD),
};

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
