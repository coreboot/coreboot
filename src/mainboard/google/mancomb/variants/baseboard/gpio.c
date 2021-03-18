/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* GPIO configuration in ramstage*/
static const struct soc_amd_gpio base_gpio_table[] = {
	/* PWR_BTN_L */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* WAKE_L */
	PAD_NF(GPIO_2, WAKE_L, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* AGPIO4 */
	PAD_NC(GPIO_4),
	/* AGPIO5 */
	PAD_NC(GPIO_5),
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_6, LOW),
	/* AGPIO7 */
	PAD_NC(GPIO_7),
	/* EN_PP3300_LAN */
	PAD_GPO(GPIO_8, LOW),
	/* SD_EX_PRSNT_L */
	PAD_GPI(GPIO_9, PULL_NONE),
	/* S0A3 */
	PAD_NF(GPIO_10, S0A3, PULL_NONE),
	/* AGPIO11 */
	PAD_NC(GPIO_11),
	/* SLP_S3_GATED */
	PAD_GPO(GPIO_12, LOW),
	/* GPIO_13 - GPIO_15: Not available */
	/* USB_FAULT_ODL */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* AGPIO17 */
	PAD_NC(GPIO_17),
	/* LAN_AUX_RESET_L */
	PAD_GPO(GPIO_18, LOW),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* AGPIO21 */
	PAD_NC(GPIO_21),
	/* EC_SOC_WAKE_ODL */
	PAD_WAKE(GPIO_22, PULL_NONE, EDGE_LOW, S0i3),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* HUB_RST_L */
	PAD_GPO(GPIO_24, LOW),
	/* GPIO_25: Not available */
	/* PCIE_RST0_L */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),
	/* PCIE_RST1_L */
	PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
	/* GPIO_28: Not available */
	/* WLAN_AUX_RESET */
	PAD_GPO(GPIO_29, HIGH),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* AGPIO31 */
	PAD_NC(GPIO_31),
	/* AGPIO32 */
	PAD_NC(GPIO_32),
	/* GPIO_33 - GPIO_39: Not available */
	/* SSD_AUX_RESET_L */
	PAD_GPO(GPIO_40, LOW),
	/* GPIO_41: Not available */
	/* EGPIO42 */
	PAD_NC(GPIO_42),
	/* GPIO_43 - GPIO_66: Not available */
	/* SOC_BIOS_WP_L */
	PAD_GPI(GPIO_67, PULL_NONE),
	/* AGPIO68 */
	PAD_NC(GPIO_68),
	/* AGPIO69 */
	PAD_NC(GPIO_69),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_70, LOW),
	/* GPIO_71 - GPIO_73: Not available */
	/* EGPIO74 */
	PAD_NC(GPIO_74),
	/* EGPIO75 */
	PAD_NC(GPIO_75),
	/* EGPIO76 */
	PAD_NC(GPIO_76),
	/* GPIO_77 - GPIO_83: Not available */
	/* EC_SOC_INT_ODL */
	PAD_GPI(GPIO_84, PULL_NONE),
	/* AGPIO85 */
	PAD_NC(GPIO_85),
	/* SPI_CLK2 */
	PAD_NF(GPIO_86, SPI_CLK, PULL_NONE),
	/* AGPIO87 */
	PAD_NC(GPIO_87),
	/* AGPIO88 */
	PAD_NC(GPIO_88),
	/* AGPIO89 */
	PAD_NC(GPIO_89),
	/* HP_INT_ODL */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* PWM_3V3_BUZZER */
	PAD_GPO(GPIO_91, LOW),
	/* CLK_REQ0_L */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* GPIO_93 - GPIO_103: Not available */
	/* GPIO_104 - GPIO_108: eSPI configured in early stage */
	/* EGPIO109 */
	PAD_NC(GPIO_109),
	/* GPIO_110 - GPIO_112: Not available */
	/* I2C2_SCL */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* CLK_REQ1_L */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_NONE),
	/* CLK_REQ2_L */
	PAD_NF(GPIO_116, CLK_REQ2_L, PULL_NONE),
	/* GPIO_117 - GPIO_119: Not available */
	/* EGPIO120 */
	PAD_NC(GPIO_120),
	/* EGPIO121 */
	PAD_NC(GPIO_121),
	/* GPIO_122 - GPIO_128: Not available */
	/* AGPIO129 */
	PAD_NC(GPIO_129),
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_130, HIGH),
	/* CLK_REQ3_L */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_NONE),
	/* BT_DISABLE */
	PAD_GPO(GPIO_132, HIGH),
	/* EGPIO140 */
	PAD_NC(GPIO_140),
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* EGPIO142 */
	PAD_NC(GPIO_142),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* AGPIO144 */
	PAD_NC(GPIO_144),
	/* I2C0_SCL */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),
	/* I2C0_SDA */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),
	/* I2C1_SCL */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),
	/* I2C1_SDA */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
};

/* Early GPIO configuration */
static const struct soc_amd_gpio early_gpio_table[] = {
	/* ESPI1_DATA0 */
	PAD_NF(GPIO_104, SPI2_DO_ESPI2_D0, PULL_NONE),
	/* ESPI1_DATA1 */
	PAD_NF(GPIO_105, SPI2_DI_ESPI2_D1, PULL_NONE),
	/* ESPI1_DATA2 */
	PAD_NF(GPIO_106, EMMC_SPI2_WP_L_ESPI2_D2, PULL_NONE),
	/* ESPI1_DATA3 */
	PAD_NF(GPIO_107, SPI2_HOLD_L_ESPI2_D3, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_108, ESPI_ALERT_D1, PULL_NONE),
};

/* GPIO configuration for sleep */
static const struct soc_amd_gpio sleep_gpio_table[] = {
	/* TODO: Fill sleep gpio configuration */
};

const struct soc_amd_gpio *__weak variant_base_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(base_gpio_table);
	return base_gpio_table;
}
const struct soc_amd_gpio *__weak variant_override_gpio_table(size_t *size)
{
	*size = 0;
	return NULL;
}
const struct soc_amd_gpio *__weak variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
const __weak struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(sleep_gpio_table);
	return sleep_gpio_table;
}
