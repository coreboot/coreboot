/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/smi.h>
#include <stdlib.h>
#include <boardid.h>
#include <variant/gpio.h>

static const struct soc_amd_gpio gpio_set_stage_rom_pre_v3[] = {
	/* PEN_POWER_EN - reset */
	PAD_GPO(GPIO_5, LOW),
	/* EC_FCH_WAKE_L */
	PAD_GPI(GPIO_24, PULL_UP),
	PAD_WAKE(GPIO_24, PULL_UP, EDGE_LOW, S3_S4_S5),
	/* PCIE_RST1_L - Variable timings (May remove) */
	PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, HIGH),
	/* EN_PWR_TOUCHPAD_PS2 - reset */
	PAD_GPO(GPIO_67, LOW),
	/* EMMC_RESET - reset (default stuffing unused)*/
	PAD_GPO(GPIO_68, HIGH),
	/* EN_PWR_CAMERA - reset */
	PAD_GPO(GPIO_76, LOW),
	/* CLK_REQ0_L - WIFI */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_UP),
	/* CLK_REQ1_L - SD Card */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_UP),
	/* CLK_REQ4_L - SSD */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_UP),
	/* BIOS_FLASH_WP_ODL */
	PAD_GPI(GPIO_137, PULL_NONE),
	/* USI_RESET - reset */
	PAD_GPO(GPIO_140, HIGH),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_142, HIGH),
};

static const struct soc_amd_gpio gpio_set_stage_rom_v3[] = {
	/* PEN_POWER_EN - reset */
	PAD_GPO(GPIO_5, LOW),
	/* EN_PWR_TOUCHPAD_PS2 - reset */
	PAD_GPO(GPIO_13, LOW),
	/* EC_FCH_WAKE_L */
	PAD_GPI(GPIO_24, PULL_UP),
	PAD_WAKE(GPIO_24, PULL_UP, EDGE_LOW, S3_S4_S5),
	/* PCIE_RST1_L - Variable timings (May remove) */
	PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, HIGH),
	/* EMMC_RESET - reset (default stuffing unused)*/
	PAD_GPO(GPIO_68, HIGH),
	/* EN_PWR_CAMERA - reset */
	PAD_GPO(GPIO_76, LOW),
	/* CLK_REQ0_L - WIFI */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_UP),
	/* CLK_REQ1_L - SD Card */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_UP),
	/* CLK_REQ4_L - SSD */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_UP),
	/* BIOS_FLASH_WP_ODL */
	PAD_GPI(GPIO_137, PULL_NONE),
	/* USI_RESET - reset */
	PAD_GPO(GPIO_140, HIGH),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_142, HIGH),
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
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_13, HIGH),
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
	/* DMIC SEL */
	/*
	 * Make sure Ext ROM Sharing is disabled before using this GPIO.  Otherwise SPI flash
	 * access will be very slow.
	 */
	PAD_GPO(GPIO_67, LOW), // Select Camera 1 Dmic
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
	/* EMMC_DATA7 */
	PAD_NF(GPIO_87, EMMC_DATA7, PULL_NONE),
	/* EMMC_DATA5 */
	PAD_NF(GPIO_88, EMMC_DATA5, PULL_NONE),
	/*  EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* Testpoint */
	PAD_GPI(GPIO_90, PULL_UP),
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
struct soc_amd_gpio *variant_romstage_gpio_table(size_t *size)
{
	uint32_t board_version;

	if (!google_chromeec_cbi_get_board_version(&board_version) &&
	    (board_version >= CONFIG_VARIANT_MIN_BOARD_ID_V3_SCHEMATICS)) {
		*size = ARRAY_SIZE(gpio_set_stage_rom_v3);
		return gpio_set_stage_rom_v3;
	}

	*size = ARRAY_SIZE(gpio_set_stage_rom_pre_v3);
	return gpio_set_stage_rom_pre_v3;
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

static void wifi_power_reset_configure_v3(void)
{
	/*
	 * Configure WiFi GPIOs such that:
	 * - WIFI_AUX_RESET_L is configured first to assert PERST# to WiFi device.
	 * - Enable power to WiFi using EN_PWR_WIFI.
	 * - Wait for 50ms after power to WiFi is enabled.
	 * - Deassert WIFI_AUX_RESET_L.
	 */
	static const struct soc_amd_gpio v3_wifi_table[] = {
		/* WIFI_AUX_RESET_L */
		PAD_GPO(GPIO_86, LOW),
		/* EN_PWR_WIFI */
		PAD_GPO(GPIO_42, HIGH),
	};
	program_gpios(v3_wifi_table, ARRAY_SIZE(v3_wifi_table));

	mdelay(50);
	gpio_set(GPIO_86, 1);
}

static void wifi_power_reset_configure_pre_v3(void)
{
	/*
	 * Configure WiFi GPIOs such that:
	 * - WIFI_AUX_RESET_L is configured first to assert PERST# to WiFi device.
	 * - Disable power to WiFi since GPIO_29 goes high on PWRGOOD but has a glitch on RESET#
	 *   deassertion causing WiFi to enter a bad state.
	 * - Wait 10ms for WiFi power to go low.
	 * - Enable power to WiFi using EN_PWR_WIFI.
	 * - Wait for 50ms after power to WiFi is enabled.
	 * - Deassert WIFI_AUX_RESET_L.
	 */
	static const struct soc_amd_gpio pre_v3_wifi_table[] = {
		/* WIFI_AUX_RESET_L */
		PAD_GPO(GPIO_42, LOW),
		/* EN_PWR_WIFI */
		PAD_GPO(GPIO_29, LOW),
	};
	program_gpios(pre_v3_wifi_table, ARRAY_SIZE(pre_v3_wifi_table));

	mdelay(10);
	gpio_set(GPIO_29, 1);
	mdelay(50);
	gpio_set(GPIO_42, 1);
}

__weak void variant_pcie_power_reset_configure(void)
{
	uint32_t board_version;

	if (!google_chromeec_cbi_get_board_version(&board_version) &&
	    (board_version >= CONFIG_VARIANT_MIN_BOARD_ID_V3_SCHEMATICS))
		wifi_power_reset_configure_v3();
	else
		wifi_power_reset_configure_pre_v3();
}
