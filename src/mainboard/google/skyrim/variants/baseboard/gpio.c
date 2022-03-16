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
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),
	/* SOC_PEN_DETECT_ODL */
	PAD_WAKE(GPIO_3, PULL_NONE, EDGE_LOW, S0i3),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_4, HIGH),
	/* EN_PP3300_TCHPAD */
	PAD_GPO(GPIO_5, HIGH),
	/* SSD_AUX_RESET_L */
	PAD_GPO(GPIO_6, HIGH),
	/* WLAN_AUX_RST_L */
	PAD_GPO(GPIO_7, HIGH),
	/* EN_PWR_WWAN_X */
	PAD_GPO(GPIO_8, HIGH),
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_9, HIGH),
	/* BT_DISABLE */
	PAD_GPO(GPIO_10, LOW),
	/* EC_SOC_WAKE_ODL */
	PAD_SCI(GPIO_11, PULL_NONE, EDGE_LOW),
	/* SOC_FP_RST_L */
	PAD_GPO(GPIO_12, LOW),
	/* GPIO_13 - GPIO_15: Not available */
	/* USB_OC0_L */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* SOC_SAR_INT_L */
	PAD_SCI(GPIO_17, PULL_NONE, EDGE_LOW),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_18, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_21, LOW),
	/* ESPI_ALERT_D1 */
	PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* SOC_FP_INT_L */
	PAD_SCI(GPIO_24, PULL_NONE, EDGE_LOW),
	/* GPIO_25: Not available */
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
	/* SD_AUX_RESET_L  */
	PAD_GPO(GPIO_27, HIGH),
	/* GPIO_28: Not available */
	/* EN_PP3300_TCHSCR */
	PAD_GPO(GPIO_29, HIGH),
	/* SOC_DISABLE_DISP_BL */
	PAD_GPO(GPIO_30, HIGH),
	/* Unused */
	PAD_NC(GPIO_31),
	/* LPC_RST_L */
	PAD_NF(GPIO_32, LPC_RST_L, PULL_NONE),
	/* GPIO_33 - GPIO_39: Not available */
	/* SOC_TCHPAD_INT_ODL */
	PAD_SCI(GPIO_40, PULL_NONE, EDGE_LOW),
	/* GPIO_41: Not available */
	/* WWAN_RST_L */
	PAD_GPO(GPIO_42, HIGH),
	/* GPIO_43 - GPIO_66: Not available */
	/* GPIO_67 */
	PAD_GPI(GPIO_67, PULL_NONE),
	/* SPI1_DATA2 */
	PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* SPI1_DATA3 */
	PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),
	/* ESPI_CS_L */
	PAD_NF(GPIO_74, SPI1_CS1_L, PULL_NONE),
	/* TCHSCR_REPORT_EN */
	PAD_GPO(GPIO_76, LOW),
	/* SPI1_CLK */
	PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* EN_PP3300_CAM */
	PAD_GPO(GPIO_78, HIGH),
	/* SPI1_DATA1 */
	PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* SPI1_DATA0 */
	PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* EC_SOC_INT_ODL */
	PAD_GPI(GPIO_84, PULL_NONE),
	/* RAM_ID_1 / DEV_BEEP_DATA */
	PAD_GPI(GPIO_85, PULL_NONE),
	/* RAM_ID_2 / DEV_BEEP_LRCLK */
	PAD_GPI(GPIO_89, PULL_NONE),
	/* HP_INT_ODL */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* RAM_ID_3 / DEV_BEEP_BCLK */
	PAD_GPI(GPIO_91, PULL_NONE),
	/* CLK_REQ0_L / SSD */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* I2C2_SCL */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* CLK_REQ1_L / SD */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_NONE),
	/* CLK_REQ2_L / WLAN */
	PAD_NF(GPIO_116, CLK_REQ2_L, PULL_NONE),
	/* SOC_FPMCU_BOOT0 */
	PAD_GPO(GPIO_130, LOW),
	/* TCHSCR_INT_ODL */
	PAD_GPI(GPIO_131, PULL_NONE),
	/* TCHSCR_RESET_L */
	PAD_GPO(GPIO_136, LOW),
	/* SOC_BIOS_WP_L */
	PAD_GPI(GPIO_138, PULL_NONE),
	/* EN_SPKR */
	PAD_GPO(GPIO_139, HIGH),
	/* RAM_ID_0 / DEV_BEEP_EN */
	PAD_GPI(GPIO_144, PULL_NONE),
	/* UART1_TXD / FP */
	PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
	/* UART0_RXD / DBG */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART1_RXD  / FP*/
	PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
	/* UART0_TXD / DBG */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* I2C0_SCL */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),
	/* I2C0_SDA */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),
	/* I2C1_SCL */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),
	/* I2C1_SDA */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
};

static const struct soc_amd_gpio tpm_gpio_table[] = {
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_18, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
};

/* GPIO configuration for sleep */
static const struct soc_amd_gpio sleep_gpio_table[] = {
	/* TODO: Fill sleep gpio configuration */
};

/* GPIO configuration in bootblock */
static const struct soc_amd_gpio bootblock_gpio_table[] = {
	/* Enable WLAN */
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_21, LOW),
};

/* Early GPIO configuration */
static const struct soc_amd_gpio early_gpio_table[] = {
	/* WLAN_AUX_RESET_L (ACTIVE LOW) */
	PAD_GPO(GPIO_7, LOW),
	/* Power on WLAN */
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_9, HIGH),
};

/* PCIE_RST needs to be brought high before FSP-M runs */
static const struct soc_amd_gpio pcie_gpio_table[] = {
	/* Deassert all AUX_RESET lines & PCIE_RST */
	/* WLAN_AUX_RESET_L (ACTIVE LOW) */
	PAD_GPO(GPIO_7, HIGH),
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
};

__weak void variant_pcie_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(pcie_gpio_table);
	*gpio = pcie_gpio_table;
}

__weak void variant_base_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(base_gpio_table);
	*gpio = base_gpio_table;
}

__weak void variant_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = 0;
	*gpio = NULL;
}

__weak void variant_bootblock_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(bootblock_gpio_table);
	*gpio = bootblock_gpio_table;
}

__weak void variant_early_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(early_gpio_table);
	*gpio = early_gpio_table;
}

__weak void variant_early_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = 0;
	*gpio = NULL;
}

__weak void variant_sleep_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(sleep_gpio_table);
	*gpio = sleep_gpio_table;
}

__weak void variant_tpm_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(tpm_gpio_table);
	*gpio = tpm_gpio_table;
}
