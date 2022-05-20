/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <gpio.h>
#include <soc/gpio.h>

/* GPIO configuration in ramstage */
/* Please make sure that *ALL* GPIOs are configured in this table */
static const struct soc_amd_gpio base_gpio_table[] = {
	/* PWR_BTN_L */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* WAKE_L */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_3, LOW),
	/* SOC_PEN_DETECT_ODL */
	PAD_WAKE(GPIO_4, PULL_NONE, EDGE_HIGH, S0i3),
	/* Unused */
	PAD_NC(GPIO_5),
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_6, HIGH),
	/* EN_PP3300_TCHPAD */
	PAD_GPO(GPIO_7, HIGH),
	/* EN_PWR_WWAN_X */
	PAD_GPO(GPIO_8, HIGH),
	/* SOC_TCHPAD_INT_ODL */
	PAD_SCI(GPIO_9, PULL_NONE, EDGE_LOW),
	/* S0A3 */
	PAD_NF(GPIO_10, S0A3, PULL_NONE),
	/* SOC_FP_RST_L */
	PAD_GPO(GPIO_11, LOW),
	/* SLP_S3_GATED */
	PAD_GPO(GPIO_12, LOW),
	/* GPIO_13 - GPIO_15: Not available */
	/* USB_OC0_L */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* SOC_SAR_INT_L */
	PAD_SCI(GPIO_17, PULL_NONE, EDGE_LOW),
	/* WWAN_AUX_RESET_L */
	PAD_GPO(GPIO_18, HIGH),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* SOC_FP_INT_L */
	PAD_SCI(GPIO_21, PULL_NONE, EDGE_LOW),
	/* EC_SOC_WAKE_ODL */
	PAD_SCI(GPIO_22, PULL_NONE, EDGE_LOW),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* WWAN_RST_L */
	PAD_GPO(GPIO_24, HIGH),
	/* GPIO_25: Not available */
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST_L, HIGH),
	/* PCIE_RST1_L */
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* GPIO_28: Not available */
	/* WLAN_AUX_RESET (Active HIGH)*/
	PAD_GPO(GPIO_29, LOW),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_31),
	/* Unused */
	PAD_NC(GPIO_32),
	/* GPIO_33 - GPIO_39: Not available */
	/* SSD_AUX_RESET_L */
	PAD_GPO(GPIO_40, HIGH),
	/* GPIO_41: Not available */
	/* WWAN_DPR_SAR_ODL */
	PAD_GPO(GPIO_42, LOW),
	/* GPIO_43 - GPIO_66: Not available */
	/* SOC_BIOS_WP_L */
	PAD_GPI(GPIO_67, PULL_NONE),
	/* EN_PP3300_TCHSCR */
	PAD_GPO(GPIO_68, HIGH),
	/* SD_AUX_RESET_L  */
	PAD_GPO(GPIO_69, HIGH),
	/* EN_SPKR */
	PAD_GPO(GPIO_70, LOW),
	/* GPIO_71 - GPIO_73: Not available */
	/* Unused TP49 */
	PAD_NC(GPIO_74),
	/* RAM_ID_2 / DEV_BEEP_LRCLK */
	PAD_GPI(GPIO_75, PULL_NONE),
	/* EN_PP3300_CAM */
	PAD_GPO(GPIO_76, HIGH),
	/* GPIO_77 - GPIO_83: Not available */
	/* EC_SOC_INT_ODL */
	PAD_GPI(GPIO_84, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_85, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* ESPI_SOC_CLK */
	PAD_NF(GPIO_86, SPI_CLK, PULL_NONE),
	/* RAM_ID_1 / DEV_BEEP_DATA */
	PAD_GPI(GPIO_87, PULL_NONE),
	/* RAM_ID_3 / DEV_BEEP_BCLK */
	PAD_GPI(GPIO_88, PULL_NONE),
	/* TCHSCR_INT_ODL */
	PAD_GPI(GPIO_89, PULL_NONE),
	/* HP_INT_ODL */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* SD_EX_PRSNT_L(Guybrush BoardID 1 only) / EC_IN_RW_OD */
	PAD_GPI(GPIO_91, PULL_NONE),
	/* CLK_REQ0_L */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* GPIO_93 - GPIO_103: Not available */
	/* ESPI1_DATA0 */
	PAD_NF(GPIO_104, SPI2_DO_ESPI2_D0, PULL_NONE),
	/* ESPI1_DATA1 */
	PAD_NF(GPIO_105, SPI2_DI_ESPI2_D1, PULL_NONE),
	/* ESPI1_DATA2 */
	PAD_NF(GPIO_106, SPI2_WP_L_ESPI2_D2, PULL_NONE),
	/* ESPI1_DATA3 */
	PAD_NF(GPIO_107, SPI2_HOLD_L_ESPI2_D3, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_108, ESPI_ALERT_D1, PULL_NONE),
	/* RAM_ID_0 / DEV_BEEP_EN */
	PAD_GPI(GPIO_109, PULL_NONE),
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
	/* TCHSCR_REPORT_EN */
	PAD_GPO(GPIO_120, LOW),
	/* TCHSCR_RESET_L */
	PAD_GPO(GPIO_121, LOW),
	/* GPIO_122 - GPIO_128: Not available */
	/* SOC_DISABLE_DISP_BL */
	PAD_GPO(GPIO_129, HIGH),
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_130, LOW),
	/* CLK_REQ3_L */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_NONE),
	/* BT_DISABLE */
	PAD_GPO(GPIO_132, LOW),
	/* UART1_TXD */
	PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART1_RXD */
	PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* SOC_FPMCU_BOOT0 */
	PAD_GPO(GPIO_144, LOW),
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
	/* Assert all AUX reset lines */
	/* Unused */
	PAD_NC(GPIO_5),
	/* WWAN_AUX_RESET_L */
	PAD_GPO(GPIO_18, LOW),
	/* WLAN_AUX_RESET (ACTIVE HIGH) */
	PAD_GPO(GPIO_29, HIGH),
	/* SSD_AUX_RESET_L */
	PAD_GPO(GPIO_40, LOW),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_69, LOW),
	/* Guybrush BID>1, Other variants : Unused TP27; BID==1: SD_AUX_RESET_L */
	PAD_NC(GPIO_70),

	/* Deassert PCIe Reset lines */
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST_L, HIGH),
	/* PCIE_RST1_L */
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),

/* Power on WLAN & WWAN */
	/* EN_PP3300_WLAN */
	PAD_GPO(GPIO_6, HIGH),
	/* EN_PWR_WWAN_X */
	PAD_GPO(GPIO_8, HIGH),

/* Put WWAN into reset */
	/* WWAN_RST_L */
	PAD_GPO(GPIO_24, LOW),

/* Enable UART 0 */
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),

/* Support EC trusted */
	/* SD_EX_PRSNT_L(Guybrush BoardID 1 only) / EC_IN_RW_OD */
	PAD_GPI(GPIO_91, PULL_NONE),
};

static const struct soc_amd_gpio espi_gpio_table[] = {
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* ESPI_SOC_CLK */
	PAD_NF(GPIO_86, SPI_CLK, PULL_NONE),
	/* ESPI1_DATA0 */
	PAD_NF(GPIO_104, SPI2_DO_ESPI2_D0, PULL_NONE),
	/* ESPI1_DATA1 */
	PAD_NF(GPIO_105, SPI2_DI_ESPI2_D1, PULL_NONE),
	/* ESPI1_DATA2 */
	PAD_NF(GPIO_106, SPI2_WP_L_ESPI2_D2, PULL_NONE),
	/* ESPI1_DATA3 */
	PAD_NF(GPIO_107, SPI2_HOLD_L_ESPI2_D3, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_108, ESPI_ALERT_D1, PULL_NONE),
};

static const struct soc_amd_gpio tpm_gpio_table[] = {
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_85, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
};

/* Power-on timing requirements:
 * Fibocom 350-GL:
 *   FCP0# goes high (GPIO 6)	to	Reset# high (GPIO 24):	20ms min
 *   FCP0# goes high (GPIO 6)	to	PERST# high (GPIO 26):	100ms min
 *   PERST# high (GPIO 26)	to	PCIE Training (FSP-M):	23ms min
 *
 * Realtek RTL8852AE:
 *   Power (3.3 V) valid	to	PERST# high (GPIO_26):	50ms min
 *
 * Qualcomm WCN6856:
 *   Power (3.3 V) valid	to	PERST# high (GPIO_26):	50ms min
 *
 * RTS5250S / RTS5227S / RTS5261S
 *   Power (3.3 V) valid	to	PERST# high (GPIO_69/70):	1ms min
 *
 * PCIe spec:
 *   Power (3.3 V) valid	to	PERST# high (GPIO_26):	50ms min (SUGGESTED)
 *
 * NVME adapters planned for Guybrush:
 *   No power on timings specified - Assumed to require PCIe Spec suggested
 *   guidelines.  Testing seems to bear out this assumption.
 */

static const struct soc_amd_gpio bootblock_gpio_table[] = {
	/* Enable WWAN, Deassert WWAN reset, keep WWAN PCIe Aux reset asserted */
	/* WWAN_RST_L */
	PAD_GPO(GPIO_24, HIGH),

	/* Enable WLAN */
	/* WLAN_DISABLE */
	PAD_GPO(GPIO_130, LOW),
};

/* PCIE_RST needs to be brought high before FSP-M runs */
static const struct soc_amd_gpio pcie_gpio_table[] = {
	/* Deassert all AUX_RESET lines & PCIE_RST */
	/* Unused */
	PAD_NC(GPIO_5),
	/* WWAN_AUX_RESET_L */
	PAD_GPO(GPIO_18, HIGH),
	/* WLAN_AUX_RESET (ACTIVE HIGH) */
	PAD_GPO(GPIO_29, LOW),
	/* SSD_AUX_RESET_L */
	PAD_GPO(GPIO_40, HIGH),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_69, HIGH),
	/* Guybrush BID>1, Other variants : Unused TP27; BID==1: SD_AUX_RESET_L */
	PAD_NC(GPIO_70),
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST_L, HIGH),
};

const struct soc_amd_gpio *__weak variant_pcie_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(pcie_gpio_table);
	return pcie_gpio_table;
}

const struct soc_amd_gpio *__weak variant_bootblock_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(bootblock_gpio_table);
	return bootblock_gpio_table;
}

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

const struct soc_amd_gpio * __weak variant_early_override_gpio_table(size_t *size)
{
	/* Note that when overriding this, board ID & CBI is not available */
	*size = 0;
	return NULL;
}

const struct soc_amd_gpio * __weak variant_bootblock_override_gpio_table(size_t *size)
{
	*size = 0;
	return NULL;
}

const struct soc_amd_gpio * __weak variant_pcie_override_gpio_table(size_t *size)
{
	*size = 0;
	return NULL;
}

const struct soc_amd_gpio *__weak variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

const __weak struct soc_amd_gpio *variant_espi_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(espi_gpio_table);
	return espi_gpio_table;
}

const __weak struct soc_amd_gpio *variant_tpm_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(tpm_gpio_table);
	return tpm_gpio_table;
}
