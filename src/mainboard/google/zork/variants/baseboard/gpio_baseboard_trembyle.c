/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <soc/smi.h>
#include <stdlib.h>
#include <boardid.h>
#include <variant/gpio.h>

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* H1_FCH_INT_ODL */
	PAD_INT(GPIO_3, PULL_UP, EDGE_LOW, STATUS),
	/* I2C3_SCL - H1 */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_UP),
	/* I2C3_SDA - H1 */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_UP),

	/* FCH_ESPI_EC_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),

	/* ESPI_ALERT_L (may be unused) */
	PAD_NF(GPIO_108, ESPI_ALERT_L, PULL_UP),

	/* UART0_RXD - DEBUG */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	/* BIOS_FLASH_WP_ODL */
	PAD_GPI(GPIO_137, PULL_NONE),
	/* UART0_TXD - DEBUG */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
};

static const struct soc_amd_gpio gpio_set_stage_rom[] = {
	/* H1_FCH_INT_ODL */
	PAD_INT(GPIO_3, PULL_UP, EDGE_LOW, STATUS),
	/* PEN_POWER_EN - reset */
	PAD_GPO(GPIO_5, LOW),
	/* I2C3_SCL - H1 */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_UP),
	/* I2C3_SDA - H1 */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_UP),
	/* EC_FCH_WAKE_L */
	PAD_GPI(GPIO_24, PULL_UP),
	PAD_WAKE(GPIO_24, PULL_UP, EDGE_LOW, S3_S4_S5),
	/* PCIE_RST0_L - Fixed timings */
	/* TODO: Make sure this gets locked at end of post */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),
	/* PCIE_RST1_L - Variable timings (May remove) */
	PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
	/* FCH_ESPI_EC_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, HIGH),
	/* WIFI_AUX_RESET_L */
	PAD_GPO(GPIO_42, HIGH),
	/* EN_PWR_TOUCHPAD_PS2 - reset */
	PAD_GPO(GPIO_67, LOW),
	/* EMMC_RESET - reset (default stuffing unused)*/
	PAD_GPO(GPIO_68, HIGH),
	/* EN_PWR_CAMERA - reset */
	PAD_GPO(GPIO_76, LOW),
	/* RAM_ID_4 */
	PAD_GPI(GPIO_84, PULL_NONE),
	/* CLK_REQ0_L - WIFI */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_UP),
	/* ESPI_ALERT_L (may be unused) */
	PAD_NF(GPIO_108, ESPI_ALERT_L, PULL_UP),
	/* CLK_REQ1_L - SD Card */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_UP),
	/*  RAM_ID_3  */
	PAD_GPI(GPIO_116, PULL_NONE),
	/* RAM_ID_1  */
	PAD_GPI(GPIO_120, PULL_NONE),
	/* RAM_ID_0  */
	PAD_GPI(GPIO_121, PULL_NONE),
	/* RAM_ID_2 */
	PAD_GPI(GPIO_131, PULL_NONE),
	/* CLK_REQ4_L - SSD */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_UP),
	/* UART0_RXD - DEBUG */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	/* BIOS_FLASH_WP_ODL */
	PAD_GPI(GPIO_137, PULL_NONE),
	/* UART0_TXD - DEBUG */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
	/* USI_RESET - reset */
	PAD_GPO(GPIO_140, HIGH),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_142, HIGH),
};

static const struct soc_amd_gpio gpio_set_wifi[] = {
	/* EN_PWR_WIFI */
	PAD_GPO(GPIO_29, HIGH),
};

static const struct soc_amd_gpio gpio_set_stage_ram[] = {

	/* PWR_BTN_L */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_UP),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* PCIE_WAKE_L */
	PAD_NF(GPIO_2, WAKE_L, PULL_UP),
	/* PEN_DETECT_ODL */
	PAD_GPI(GPIO_4, PULL_UP),
	/* PEN_POWER_EN - Enabled*/
	PAD_GPO(GPIO_5, HIGH),
	/* FPMCU_INT_L */
	PAD_GPI(GPIO_6, PULL_UP),
	PAD_WAKE(GPIO_6, PULL_UP, EDGE_LOW, S3_S4_S5),
	/* I2S_SDIN */
	PAD_NF(GPIO_7, ACP_I2S_SDIN, PULL_NONE),
	/* I2S_LRCLK - Bit banged in depthcharge */
	PAD_NF(GPIO_8, ACP_I2S_LRCLK, PULL_NONE),
	/* TOUCHPAD_INT_ODL */
	/* TODO: Make sure driver sets as wake source */
	PAD_GPI(GPIO_9, PULL_UP),
	/* S0iX SLP - (unused - goes to EC & FPMCU */
	PAD_GPI(GPIO_10, PULL_UP),
	/* FPMCU_RST_L */
	PAD_GPO(GPIO_11, HIGH),
	/* USI_INT_ODL */
	PAD_GPI(GPIO_12, PULL_UP),
	/* DMIC_SEL */
	PAD_GPO(GPIO_13, LOW), // Select Camera 1 Dmic
	/* BT_DISABLE */
	PAD_GPO(GPIO_14, LOW),
	/* USB_OC0_L - USB C0 + USB A0 */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_UP),
	/* USB_OC1_L - USB C1 + USB A1 */
	PAD_NF(GPIO_17, USB_OC1_L, PULL_UP),
	/* WIFI_DISABLE */
	PAD_GPO(GPIO_18, LOW),
	/* EMMC_CMD */
	PAD_NF(GPIO_21, EMMC_CMD, PULL_UP),
	/* EC_FCH_SCI_ODL */
	PAD_SCI(GPIO_22, PULL_UP, EDGE_LOW),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/*  EC_AP_INT_ODL (Sensor Framesync) */
	PAD_GPI(GPIO_31, PULL_UP),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, HIGH),
	/* EN_PWR_TOUCHPAD_PS2 */
	/*
	 * EN_PWR_TOUCHPAD_PS2 - Make sure Ext ROM Sharing is disabled before
	 * using this GPIO.  Otherwise SPI flash access will be very slow.
	 */
	PAD_GPO(GPIO_67, HIGH),
	/* EMMC_RESET */
	PAD_GPO(GPIO_68, LOW),
	/* FPMCU_BOOT0 - TODO: Check this */
	PAD_GPO(GPIO_69, LOW),
	/* EMMC_CLK */
	PAD_NF(GPIO_70, EMMC_CLK, PULL_NONE),
	/* EMMC_DATA4 */
	PAD_NF(GPIO_74, EMMC_DATA4, PULL_NONE),
	/* EMMC_DATA6 */
	PAD_NF(GPIO_75, EMMC_DATA6, PULL_NONE),
	/* EN_PWR_CAMERA */
	PAD_GPO(GPIO_76, HIGH),
	/* APU_EDP_BL_DISABLE TODP: Set low in depthcharge */
	PAD_GPO(GPIO_85, HIGH),
	/* MST_GPIO_2 (Fw Update HDMI hub) */
	PAD_GPI(GPIO_86, PULL_NONE),
	/* EMMC_DATA7 */
	PAD_NF(GPIO_87, EMMC_DATA7, PULL_NONE),
	/* EMMC_DATA5 */
	PAD_NF(GPIO_88, EMMC_DATA5, PULL_NONE),
	/*  EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* MST_GPIO_3 (Fw Update HDMI hub) */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* EN_SPKR TODO: Verify driver enables this (add to ACPI) */
	PAD_GPO(GPIO_91, LOW),
	/* EMMC_DATA0 */
	PAD_NF(GPIO_104, EMMC_DATA0, PULL_NONE),
	/* EMMC_DATA1 */
	PAD_NF(GPIO_105, EMMC_DATA1, PULL_NONE),
	/* EMMC_DATA2 */
	PAD_NF(GPIO_106, EMMC_DATA2, PULL_NONE),
	/* EMMC_DATA3 */
	PAD_NF(GPIO_107, EMMC_DATA3, PULL_NONE),
	/* EMMC_DS */
	PAD_NF(GPIO_109, EMMC_DS, PULL_NONE),
	/* I2C2_SCL - USI/Touchpad */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_UP),
	/* I2C2_SDA - USI/Touchpad */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_UP),
	/* KBRST_L */
	PAD_NF(GPIO_129, KBRST_L, PULL_UP),
	/* EC_IN_RW_OD */
	PAD_GPI(GPIO_130, PULL_UP),
	/* DEV_BEEP_CODEC_IN (Dev beep Data out) */
	PAD_GPI(GPIO_135, PULL_NONE),
	/* DEV_BEEP_BCLK */
	PAD_GPI(GPIO_139, PULL_NONE),
	/* USI_RESET */
	PAD_GPO(GPIO_140, LOW),
	/* UART1_RXD - FPMCU */
	PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),
	/* UART1_TXD - FPMCU */
	PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),
	/* USI_REPORT_EN */
	/* TODO: Driver resets this later.  Do we want it high or low initially? */
	PAD_GPO(GPIO_144, HIGH),
};

const __weak
struct soc_amd_gpio *variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_reset);
	return gpio_set_stage_reset;
}

const __weak
struct soc_amd_gpio *variant_romstage_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_rom);
	return gpio_set_stage_rom;
}

const __weak
struct soc_amd_gpio *variant_wifi_romstage_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_wifi);
	return gpio_set_wifi;
}

const __weak
struct soc_amd_gpio *variant_base_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_ram);
	return gpio_set_stage_ram;
}

/*
 * This function is still needed for boards that sets gevents above 23
 * that will generate SCI or SMI. Normally this function
 * points to a table of gevents and what needs to be set. The code that
 * calls it was modified so that when this function returns NULL then the
 * caller does nothing.
 */
const __weak struct sci_source *get_gpe_table(size_t *num)
{
	return NULL;
}
