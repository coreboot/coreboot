/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>

static const struct pad_config default_override_table[] = {

	PAD_NC(GPIO_52, UP_20K),
	PAD_NC(GPIO_53, UP_20K),
	/* UART2-CTS_B -- EN_PP3300_DX_LTE_SOC */
	PAD_CFG_GPO(GPIO_67, 1, PWROK),
	/* PCIE_WAKE1_B -- FULL_CARD_POWER_OFF */
	PAD_CFG_GPO(GPIO_117, 1, PWROK),

	PAD_NC(GPIO_143, UP_20K),
	PAD_NC(GPIO_144, UP_20K),
	PAD_NC(GPIO_145, UP_20K),

	/* EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_146, 0, DEEP, NONE, Tx0RxDCRx0,
					DISPUPD),

	/* AVS_I2S1_MCLK -- PLT_RST_LTE_L */
	PAD_CFG_GPO(GPIO_161, 1, DEEP),

	PAD_NC(GPIO_213, DN_20K),
	PAD_NC(GPIO_214, DN_20K),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(default_override_table);

	return default_override_table;
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
