/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct pad_config default_override_table[] = {
	PAD_NC(GPIO_50, UP_20K), /* PCH_I2C_PEN_SDA -- unused */
	PAD_NC(GPIO_51, UP_20K), /* PCH_I2C_PEN_SCL -- unused */
	PAD_NC(GPIO_52, UP_20K), /* PCH_I2C_P_SENSOR_SDA -- unused */
	PAD_NC(GPIO_53, UP_20K), /* PCH_I2C_P_SENSOR_SCL -- unused */

	PAD_NC(GPIO_67, UP_20K), /* EN_PP3300_DX_LTE_SOC -- unused */

	PAD_NC(GPIO_105, UP_20K), /* TOUCHSCREEN_RST -- unused */
	PAD_NC(GPIO_108, NONE), /* PMU_SUSCLK -- unused */

	PAD_NC(GPIO_114, UP_20K), /* I2C7 Touchscreen -- unused */
	PAD_NC(GPIO_115, UP_20K), /* I2C7 Touchscreen -- unused */

	PAD_NC(GPIO_117, UP_20K), /* PCIE_WAKE1_B - No LTE*/
	PAD_NC(GPIO_119, UP_20K), /* PCIE_WAKE3_B - only use CNVI */

	/* PCIE_CLKREQ3_B -- unused */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_123, UP_20K, DEEP, NF1, HIZCRx1, ENPU),

	PAD_NC(GPIO_138, UP_20K), /* PEN_PDCT_ODL -- unused */
	PAD_NC(GPIO_139, UP_20K), /* PEN_INT_ODL -- unused */
	PAD_NC(GPIO_140, UP_20K), /* PEN_RESET -- unused */

	PAD_NC(GPIO_143, UP_20K), /* LTE_SAR_ODL -- unused */
	PAD_NC(GPIO_144, UP_20K), /* PEN_EJECT(wake) -- unused */
	PAD_NC(GPIO_145, UP_20K), /* PEN_EJECT(notification) -- unused */

	PAD_NC(GPIO_161, UP_20K), /* LTE_OFF_ODL -- unused */
	PAD_NC(GPIO_164, UP_20K), /* WLAN_PE_RST -- unused */

	PAD_NC(GPIO_212, UP_20K), /* TOUCHSCREEN_INT_ODL -- unused */
	PAD_NC(GPIO_213, UP_20K), /* EN_PP3300_TOUCHSCREEN -- unused */
	PAD_NC(GPIO_214, UP_20K), /* P_SENSOR_INT_L -- unused */
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(default_override_table);
	return default_override_table;
}
