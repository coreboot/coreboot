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
	/* AGPIO3 */
	PAD_NC(GPIO_3),
	/* AGPIO4 */
	PAD_NC(GPIO_4),
	/* AGPIO5 */
	PAD_NC(GPIO_5),
	/* AGPIO6 */
	PAD_NC(GPIO_6),
	/* AGPIO7 */
	PAD_NC(GPIO_7),
	/* AGPIO8 */
	PAD_NC(GPIO_8),
	/* AGPIO9 */
	PAD_NC(GPIO_9),
	/* S0A3 */
	PAD_NF(GPIO_10, S0A3, PULL_NONE),
	/* AGPIO11 */
	PAD_NC(GPIO_11),
	/* AGPIO12 */
	PAD_NC(GPIO_12),
	/* GPIO_13 - GPIO_15: Not available */
	/* USB_OC0_L */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* AGPIO17 */
	PAD_NC(GPIO_17),
	/* AGPIO18 */
	PAD_NC(GPIO_18),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* AGPIO21 */
	PAD_NC(GPIO_21),
	/* AGPIO22 */
	PAD_NC(GPIO_22),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* AGPIO24 */
	PAD_NC(GPIO_24),
	/* GPIO_25: Not available */
	/* PCIE_RST0_L */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),
	/* PCIE_RST1_L */
	PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
	/* GPIO_28: Not available */
	/* AGPIO29 */
	PAD_NC(GPIO_29),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* SPI_CS3_L */
	PAD_NF(GPIO_31, SPI_CS3_L, PULL_NONE),
	/* AGPIO32 */
	PAD_NC(GPIO_32),
	/* GPIO_33 - GPIO_39: Not available */
	/* AGPIO40 */
	PAD_NC(GPIO_40),
	/* GPIO_41: Not available */
	/* EGPIO42 */
	PAD_NC(GPIO_42),
	/* GPIO_43 - GPIO_66: Not available */
	/* EGPIO67 */
	PAD_NC(GPIO_67),
	/* AGPIO68 */
	PAD_NC(GPIO_68),
	/* AGPIO69 */
	PAD_NC(GPIO_69),
	/* EGPIO70 */
	PAD_NC(GPIO_70),
	/* GPIO_71 - GPIO_73: Not available */
	/* EGPIO74 */
	PAD_NC(GPIO_74),
	/* EGPIO75 */
	PAD_NC(GPIO_75),
	/* EGPIO76 */
	PAD_NC(GPIO_76),
	/* GPIO_77 - GPIO_83: Not available */
	/* AGPIO84 */
	PAD_NC(GPIO_84),
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
	/* AGPIO90 */
	PAD_NC(GPIO_90),
	/* AGPIO91 */
	PAD_NC(GPIO_91),
	/* CLK_REQ0_L */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* GPIO_93 - GPIO_103: Not available */
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
	/* AGPIO130 */
	PAD_NC(GPIO_130),
	/* CLK_REQ3_L */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_NONE),
	/* EGPIO132 */
	PAD_NC(GPIO_132),
	/* UART1_TXD */
	PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART1_RXD */
	PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
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

/* Early GPIO configuration in bootblock */
static const struct soc_amd_gpio bootblock_gpio_table[] = {
	/* TODO: Fill bootblock gpio configuration */
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

const struct soc_amd_gpio *__weak variant_bootblock_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(bootblock_gpio_table);
	return bootblock_gpio_table;
}
