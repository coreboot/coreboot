/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <gpio.h>

/* GPIO configuration in ramstage*/
static const struct soc_amd_gpio base_gpio_table[] = {
	/* PWR_BTN_L */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* WAKE_L */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),
	/* Unused */
	PAD_NC(GPIO_3),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_4, LOW),
	/* EN_PP3300_TCHPAD */
	PAD_GPO(GPIO_5, HIGH),
	/* EN_PP3300_WWAN_X */
	PAD_GPO(GPIO_6, LOW),
	/* SOC_PEN_DETECT_ODL */
	PAD_WAKE(GPIO_7, PULL_NONE, EDGE_LOW, S0i3),
	/* SOC_TCHPAD_INT_ODL */
	PAD_SCI(GPIO_8, PULL_NONE, LEVEL_LOW),
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_9, HIGH),
	/* WWAN_RST */
	PAD_GPO(GPIO_11, HIGH),
	/* Unused */
	PAD_NC(GPIO_12),
	/* GPIO_13 - GPIO_15: Not available */
	/* USB_OC0_L */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* EC_SOC_WAKE_R_ODL */
	PAD_SCI(GPIO_17, PULL_UP, EDGE_LOW),
	/* FP_SOC_INT_L */
	PAD_SCI(GPIO_18, PULL_NONE, LEVEL_LOW),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_21),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_24),
	/* GPIO_25-26: Not available */
	/* SOC_PCIE_RST1_R_L  */
	PAD_NC(GPIO_27),
	/* GPIO_28: Not available */
	/* SD_AUX_RST */
	PAD_GPO(GPIO_29, LOW),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* SSD_AUX_RST */
	PAD_GPO(GPIO_31, LOW),
	/* LPC_RST_L */
	PAD_NF(GPIO_32, LPC_RST_L, PULL_NONE),
	/* GPIO_33 - GPIO_37: Not available */
	/* WLAN_AUX_RST_L */
	PAD_GPO(GPIO_38, HIGH),
	/* WWAN_AUX_RST_L */
	PAD_GPO(GPIO_39, HIGH),
	/* SOC_FP_RST_L */
	PAD_GPO(GPIO_40, LOW),
	/* GPIO_41 - GPIO_66: Not available */
	/* GPIO_67 (Unused) */
	PAD_NC(GPIO_67),
	/* ESPI1_DATA2 */
	PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* ESPI1_DATA3 */
	PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),
	/* SOC_CLK_FPMCU_R if SPI FP populated */
	PAD_NC(GPIO_70),
	/* EN_TCHSCR_REPORT */
	PAD_GPO(GPIO_74, LOW),
	/* SOC_CLK_FPMCU_R_L if SPI FP populated */
	PAD_NC(GPIO_75),
	/* Unused */
	PAD_NC(GPIO_76),
	/* ESPI_SOC_CLK_EC_R */
	PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* RAM_ID_0 */
	PAD_GPI(GPIO_78, PULL_NONE),
	/* RAM_ID_1 */
	PAD_GPI(GPIO_79, PULL_NONE),
	/* ESPI_SOC_D1_EC_R */
	PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* ESPI_SOC_D0_EC_R */
	PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_84, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* Unused */
	PAD_NC(GPIO_85),
	/* HP_INT_ODL */
	PAD_GPI(GPIO_89, PULL_NONE),
	/* EC_SOC_INT_ODL */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* TCHSCR_INT_ODL */
	PAD_GPI(GPIO_91, PULL_NONE),
	/* CLK_REQ0_L / WLAN */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* SPI_SOC_DO_FPMCU_DI_R if SPI FP populated */
	PAD_NC(GPIO_104),
	/* SPI_SOC_DI_FPMCU_DO_R if SPI FP populated */
	PAD_NC(GPIO_105),
	/* RAM_ID_2 */
	PAD_GPI(GPIO_106, PULL_NONE),
	/* RAM_ID_3 */
	PAD_GPI(GPIO_107, PULL_NONE),
	/* I2C2_SCL */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* CLK_REQ1_L / SD */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_NONE),
	/* CLK_REQ2_L / WWAN */
	PAD_NF(GPIO_116, CLK_REQ2_L, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_130),
	/* CLK_REQ3_L / SSD */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_NONE),
	/* SOC_FP_BOOT0 */
	PAD_GPO(GPIO_132, LOW),
	/* EN_PP3300_CAM */
	PAD_GPO(GPIO_135, HIGH),
	/* Unused */
	PAD_NC(GPIO_136),
	/* Unused */
	PAD_NC(GPIO_137),
	/* Unused */
	PAD_NC(GPIO_138),
	/* SOC_BIOS_WP_OD */
	PAD_GPI(GPIO_139, PULL_NONE),
	/* UART1_TXD if UART FP populated */
	PAD_NC(GPIO_140),
	/* UART0_RXD / DBG */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART1_RXD if UART FP populated */
	PAD_NC(GPIO_142),
	/* UART0_TXD / DBG */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* EN_PP3300_TCHSCR */
	PAD_GPO(GPIO_144, HIGH),
	/* I2C0_SCL */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),
	/* I2C0_SDA */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),
	/* I2C1_SCL */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),
	/* I2C1_SDA */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
	/* EN_SPKR */
	PAD_GPO(GPIO_153, LOW),
	/* BT_DISABLE */
	PAD_GPO(GPIO_154, LOW),
	/* HDMI_RST */
	PAD_GPO(GPIO_155, HIGH),
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_156, LOW),
	/* TCHSCR_RST_L */
	PAD_GPO(GPIO_157, HIGH),
};

static const struct soc_amd_gpio tpm_gpio_table[] = {
	/* I2C2_SCL */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_84, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
};

/* GPIO configuration in bootblock */
static const struct soc_amd_gpio bootblock_gpio_table[] = {
	/* Enable WLAN */
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_156, LOW),
};

/* Early GPIO configuration */
static const struct soc_amd_gpio early_gpio_table[] = {
	/* WLAN_AUX_RST_L (ACTIVE LOW) */
	PAD_GPO(GPIO_38, LOW),
	/* Power on WLAN */
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_9, HIGH),
};

/* PCIE_RST needs to be brought high before FSP-M runs */
static const struct soc_amd_gpio romstage_gpio_table[] = {
	/* Deassert all AUX_RESET lines & PCIE_RST */
	/* SD_AUX_RST */
	PAD_GPO(GPIO_29, LOW),
	/* SSD_AUX_RESET */
	PAD_GPO(GPIO_31, LOW),
	/* WLAN_AUX_RST_L (ACTIVE LOW) */
	PAD_GPO(GPIO_38, HIGH),
	/* WWAN_AUX_RST_L (ACTIVE LOW) */
	PAD_GPO(GPIO_39, HIGH),
	/* CLK_REQ0_L / WLAN */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* CLK_REQ1_L / SD */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_NONE),
	/* CLK_REQ2_L / WWAN */
	PAD_NF(GPIO_116, CLK_REQ2_L, PULL_NONE),
	/* CLK_REQ3_L / SSD */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_NONE),
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
};

static const struct soc_amd_gpio espi_gpio_table[] = {
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* ESPI_CLK */
	PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* ESPI1_DATA0 */
	PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* ESPI1_DATA1 */
	PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* ESPI1_DATA2 */
	PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* ESPI1_DATA3 */
	PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
};

void baseboard_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(base_gpio_table);
	*gpio = base_gpio_table;
}

__weak void baseboard_romstage_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(romstage_gpio_table);
	*gpio = romstage_gpio_table;
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

void variant_espi_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(espi_gpio_table);
	*gpio = espi_gpio_table;
}

__weak void variant_tpm_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(tpm_gpio_table);
	*gpio = tpm_gpio_table;
}

__weak void variant_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = 0;
	*gpio = NULL;
}
