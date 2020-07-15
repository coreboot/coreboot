/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/smi.h>
#include <stdlib.h>
#include <boardid.h>
#include <variant/gpio.h>

static const struct soc_amd_gpio gpio_set_stage_ram[] = {

	/* PWR_BTN_L */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_UP),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* PCIE_WAKE_L */
	PAD_NF(GPIO_2, WAKE_L, PULL_UP),
	/* PEN_DETECT_ODL */
	PAD_WAKE(GPIO_4, PULL_NONE, EDGE_HIGH, S3),
	/* PEN_POWER_EN - Enabled*/
	PAD_GPO(GPIO_5, HIGH),
	/* EN_PWR_TOUCHPAD */
	PAD_GPO(GPIO_6, HIGH),
	/* I2S_SDIN */
	PAD_NF(GPIO_7, ACP_I2S_SDIN, PULL_NONE),
	/* I2S_LRCLK - Bit banged in depthcharge */
	PAD_NF(GPIO_8, ACP_I2S_LRCLK, PULL_NONE),
	/* TOUCHPAD_INT_ODL */
	PAD_SCI(GPIO_9, PULL_NONE, EDGE_LOW),
	/* S0iX SLP - (unused - goes to EC & FPMCU */
	PAD_GPI(GPIO_10, PULL_UP),
	/* EC_IN_RW_OD */
	PAD_GPI(GPIO_11, PULL_UP),
	/* USI_INT_ODL */
	PAD_GPI(GPIO_12, PULL_UP),
	/* DMIC_SEL */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_UP),
	/* USB_OC1_L - USB C1 */
	PAD_NF(GPIO_17, USB_OC1_L, PULL_UP),
	/* WIFI_DISABLE */
	PAD_GPO(GPIO_18, LOW),
	/* EMMC_CMD */
	PAD_NF(GPIO_21, EMMC_CMD, PULL_UP),
	/* EC_FCH_SCI_ODL */
	PAD_SCI(GPIO_22, PULL_UP, EDGE_LOW),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* EC_FCH_WAKE_L */
	PAD_SCI(GPIO_24, PULL_NONE, EDGE_LOW),
	/*  EC_AP_INT_ODL (Sensor Framesync) */
	PAD_GPI(GPIO_31, PULL_UP),
	/*  */
	PAD_GPI(GPIO_32, PULL_DOWN),
	/* DMIC_SEL */
	/*
	 * Make sure Ext ROM Sharing is disabled before using this GPIO.  Otherwise SPI flash
	 * access will be very slow.
	 */
	PAD_GPO(GPIO_67, LOW),  // Select Camera 1 Dmic
	/* EMMC_RESET */
	PAD_GPO(GPIO_68, LOW),
	/* RAM ID 3*/
	PAD_GPI(GPIO_69, PULL_NONE),
	/* EMMC_CLK */
	PAD_NF(GPIO_70, EMMC_CLK, PULL_NONE),
	/* EMMC_DATA4 */
	PAD_NF(GPIO_74, EMMC_DATA4, PULL_NONE),
	/* EMMC_DATA6 */
	PAD_NF(GPIO_75, EMMC_DATA6, PULL_NONE),
	/* EN_PWR_CAMERA */
	PAD_GPO(GPIO_76, HIGH),
	/* UNUSED */
	PAD_GPO(GPIO_84, HIGH),
	/* APU_EDP_BL_DISABLE TODP: Set low in depthcharge */
	PAD_GPO(GPIO_85, HIGH),
	/* RAM ID 2 */
	PAD_GPI(GPIO_86, PULL_NONE),
	/* EMMC_DATA7 */
	PAD_NF(GPIO_87, EMMC_DATA7, PULL_NONE),
	/* EMMC_DATA5 */
	PAD_NF(GPIO_88, EMMC_DATA5, PULL_NONE),
	/*  EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* RAM ID 1 */
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
	/* RAM ID 0 */
	PAD_GPI(GPIO_132, PULL_NONE),
	/* DEV_BEEP_CODEC_IN (Dev beep Data out) */
	PAD_GPI(GPIO_135, PULL_NONE),
	/* BIOS_FLASH_WP_ODL */
	PAD_GPI(GPIO_137, PULL_NONE),
	/* DEV_BEEP_BCLK */
	PAD_GPI(GPIO_139, PULL_NONE),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
	/* USB_HUB_RST_L */
	PAD_GPO(GPIO_141, HIGH),
	/* BT_DISABLE */
	PAD_GPO(GPIO_143, LOW),
	/*
	 * USI_REPORT_EN - TODO: Driver resets this later.
	 * Do we want it high or low initially?
	 */
	PAD_GPO(GPIO_144, HIGH),
};

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
const __weak struct sci_source *variant_gpe_table(size_t *num)
{
	return NULL;
}

static void wifi_power_reset_configure_active_low_power(void)
{
	/*
	 * Configure WiFi GPIOs such that:
	 * - WIFI_AUX_RESET is configured first to assert PERST# to WiFi device.
	 * - Enable power to WiFi using EN_PWR_WIFI_L.
	 * - Wait for 50ms after power to WiFi is enabled.
	 * - Deassert WIFI_AUX_RESET.
	 */
	static const struct soc_amd_gpio v3_wifi_table[] = {
		/* WIFI_AUX_RESET */
		PAD_GPO(GPIO_29, HIGH),
		/* EN_PWR_WIFI_L */
		PAD_GPO(GPIO_42, LOW),
	};
	program_gpios(v3_wifi_table, ARRAY_SIZE(v3_wifi_table));

	mdelay(50);
	gpio_set(GPIO_29, 0);
}

static void wifi_power_reset_configure_active_high_power(void)
{
	/*
	 * When GPIO_42 is configured as active high for enabling WiFi power, WIFI_AUX_RESET
	 * gets pulled high because of external PU to PP3300_WIFI. Thus, EN_PWR_WIFI needs to be
	 * set low before driving it high to trigger a WiFi power cycle to meet PCIe
	 * requirements. Thus, configure GPIOs such that:
	 * - WIFI_AUX_RESET is configured first to assert PERST# to WiFi device
	 * - Disable power to WiFi.
	 * - Wait 10ms for WiFi power to go low.
	 * - Enable power to WiFi using EN_PWR_WIFI.
	 * - Deassert WIFI_AUX_RESET.
	 */
	static const struct soc_amd_gpio v3_wifi_table[] = {
		/* WIFI_AUX_RESET */
		PAD_GPO(GPIO_29, HIGH),
		/* EN_PWR_WIFI */
		PAD_GPO(GPIO_42, LOW),
	};
	program_gpios(v3_wifi_table, ARRAY_SIZE(v3_wifi_table));

	mdelay(10);
	gpio_set(GPIO_42, 1);
	mdelay(50);
	gpio_set(GPIO_29, 0);
}

static void wifi_power_reset_configure_v3(void)
{
	if (variant_has_active_low_wifi_power())
		wifi_power_reset_configure_active_low_power();
	else
		wifi_power_reset_configure_active_high_power();
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

__weak void variant_pcie_gpio_configure(void)
{
	static const struct soc_amd_gpio pcie_gpio_table[] = {
		/* PCIE_RST1_L - Variable timings (May remove) */
		PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
		/* NVME_AUX_RESET_L */
		PAD_GPO(GPIO_40, HIGH),
		/* CLK_REQ0_L - WIFI */
		PAD_NF(GPIO_92, CLK_REQ0_L, PULL_UP),
		/* CLK_REQ1_L - SD Card */
		PAD_NF(GPIO_115, CLK_REQ1_L, PULL_UP),
		/* CLK_REQ2_L - NVMe */
		PAD_NF(GPIO_116, CLK_REQ2_L, PULL_UP),
		/* SD_AUX_RESET_L */
		PAD_GPO(GPIO_142, HIGH),
	};

	program_gpios(pcie_gpio_table, ARRAY_SIZE(pcie_gpio_table));

	/* Deassert PCIE_RST1_L */
	gpio_set(GPIO_27, 1);

	if (variant_uses_v3_schematics())
		wifi_power_reset_configure_v3();
	else
		wifi_power_reset_configure_pre_v3();
}

static const struct soc_amd_gpio gpio_sleep_table[] = {
	/* PCIE_RST1_L */
	PAD_GPO(GPIO_27, LOW),
	/*
	 * On pre-v3 schematics, GPIO_29 is EN_PWR_WIFI. So, setting to high should be no-op.
	 * On v3+ schematics, GPIO_29 is WIFI_AUX_RESET. Setting to high ensures that PERST# is
	 * asserted to WiFi device until coreboot reconfigures GPIO_29 on resume path.
	 */
	PAD_GPO(GPIO_29, HIGH),
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, LOW),
	/* EN_PWR_CAMERA */
	PAD_GPO(GPIO_76, LOW),
};

const __weak struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size, int slp_typ)
{
	*size = ARRAY_SIZE(gpio_sleep_table);
	return gpio_sleep_table;
}
