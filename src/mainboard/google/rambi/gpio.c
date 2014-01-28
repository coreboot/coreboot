/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <baytrail/gpio.h>

/* TODO(SHAWNN): Modify gpios labeled 'INT' for interrupt handling */
/* NCORE GPIOs */
static const struct soc_gpio_map gpncore_gpio_map[] = {
	GPIO_FUNC2,	/* S0_NC00 - INT_HDMI_HPD - INT */
	GPIO_FUNC2,	/* S0_NC01 - HDMI_DDCDATA_SW */
	GPIO_FUNC2,	/* S0_NC02 - HDMI_DDCCLK_SW */
	GPIO_NC,	/* S0_NC03 - NC */
	GPIO_NC,	/* S0_NC04 - NC */
	GPIO_NC,	/* S0_NC05 - NC */
	GPIO_FUNC2,	/* S0_NC06 - EDP_HPD_L */
	GPIO_INPUT,	/* S0_NC07 - DDI1_DDCDATA - STRAP */
	GPIO_NC,	/* S0_NC08 - NC */
	GPIO_FUNC2,	/* S0_NC09 - SOC_DISP_ON_C */
	GPIO_FUNC2,	/* S0_NC10 - SOC_EDP_BLON_C */
	GPIO_FUNC2,	/* S0_NC11 - SOC_DPST_PWM_C */
	GPIO_NC,	/* S0_NC12 - NC */
	GPIO_INPUT,	/* S0_NC13 - GPIO_NC13 - STRAP */
	GPIO_NC,	/* S0_NC14 - NC */
	GPIO_DEFAULT,	/* S0_NC15 - XDP_GPIO_S0_NC15 */
	GPIO_DEFAULT,	/* S0_NC16 - XDP_GPIO_S0_NC16 */
	GPIO_DEFAULT,	/* S0_NC17 - XDP_GPIO_S0_NC17 */
	GPIO_DEFAULT,	/* S0_NC18 - XDP_GPIO_S0_NC18 */
	GPIO_DEFAULT,	/* S0_NC19 - XDP_GPIO_S0_NC19 */
	GPIO_DEFAULT,	/* S0_NC20 - XDP_GPIO_S0_NC20 */
	GPIO_DEFAULT,	/* S0_NC21 - XDP_GPIO_S0_NC21 */
	GPIO_DEFAULT,	/* S0_NC22 - XDP_GPIO_S0_NC22 */
	GPIO_DEFAULT,	/* S0_NC23 - XDP_GPIO_S0_NC23 */
	GPIO_NC,	/* S0_NC24 - NC */
	GPIO_NC,	/* S0_NC25 - NC */
	GPIO_NC,	/* S0_NC26 - NC */
	GPIO_END
};

/* SCORE GPIOs */
static const struct soc_gpio_map gpscore_gpio_map[] = {
	GPIO_INPUT,	/* S0_SC000 - SOC_KBC_SCI - INT */
	GPIO_FUNC2,	/* S0_SC001 - SATA_DEVSLP_C */
	GPIO_NC,	/* S0-SC002 - SATA_LED_R_N (NC/PU) */
	GPIO_FUNC1,	/* S0-SC003 - PCIE_CLKREQ_IMAGE# */
	GPIO_FUNC1,	/* S0-SC004 - PCIE_CLKREQ_WLAN# */
	GPIO_NC,	/* S0-SC005 - PCIE_CLKREQ_LAN# (NC) */
	GPIO_NC,	/* S0-SC006 - PCIE_CLKREQ3# (NC) */
	GPIO_NC,	/* S0-SC007 - SD3_WP (NC) */
	GPIO_NC,	/* S0-SC008 - ACZ_RST# (NC) */
	GPIO_NC,	/* S0-SC009 - ACZ_SYNC (NC) */
	GPIO_NC,	/* S0-SC010 - ACZ_BCLK (NC) */
	GPIO_NC,	/* S0-SC011 - ACZ_STDOUT (NC) */
	GPIO_NC,	/* S0-SC012 - PCH_AZ_CODEC_SDIN0 (NC) */
	GPIO_NC,	/* S0-SC013 - NC */
	GPIO_INPUT,	/* S0-SC014 - DET_TRIGGER - INT */
	GPIO_INPUT,	/* S0-SC015 - AJACK_MICPRES_L - INT */
	GPIO_FUNC3,	/* S0-SC016 - MMC1_CLK */
	GPIO_FUNC3,	/* S0-SC017 - MMC1_D[0] */
	GPIO_FUNC3,	/* S0-SC018 - MMC1_D[1] */
	GPIO_FUNC3,	/* S0-SC019 - MMC1_D[2] */
	GPIO_FUNC3,	/* S0-SC020 - MMC1_D[3] */
	GPIO_FUNC3,	/* S0-SC021 - MMC1_D[4] */
	GPIO_FUNC3,	/* S0-SC022 - MMC1_D[5] */
	GPIO_FUNC3,	/* S0-SC023 - MMC1_D[6] */
	GPIO_FUNC3,	/* S0-SC024 - MMC1_D[7] */
	GPIO_FUNC3,	/* S0-SC025 - MMC1_CMD */
	GPIO_FUNC3,	/* S0-SC026 - MMC1_RST */
	GPIO_NC,	/* S0-SC027 - NC */
	GPIO_NC,	/* S0-SC028 - NC */
	GPIO_NC,	/* S0-SC029 - NC */
	GPIO_NC,	/* S0-SC030 - NC */
	GPIO_NC,	/* S0-SC031 - NC */
	GPIO_NC,	/* S0-SC032 - NC */
	GPIO_NC,	/* S0-SC033 - SD3_CLK (NC) */
	GPIO_NC,	/* S0-SC034 - SD3_D0 (NC) */
	GPIO_NC,	/* S0-SC035 - SD3_D1 (NC) */
	GPIO_NC,	/* S0-SC036 - SD3_D2 (NC) */
	GPIO_NC,	/* S0-SC037 - SD3_D3 (NC) */
	GPIO_NC,	/* S0-SC038 - SD3_CD# (NC) */
	GPIO_NC,	/* S0-SC039 - SD3_CMD (NC) */
	GPIO_NC,	/* S0-SC040 - SDMMC3_1P8_EN (NC) */
	GPIO_NC,	/* S0-SC041 - SDIO3_PWR_EN# (NC)*/
	GPIO_FUNC1,	/* S0-SC042 - LPC_LAD0 */
	GPIO_FUNC1,	/* S0-SC043 - LPC-LAD1 */
	GPIO_FUNC1,	/* S0-SC044 - LPC_LAD2 */
	GPIO_FUNC1,	/* S0-SC045 - LPC_LAD3 */
	GPIO_FUNC1,	/* S0-SC046 - LPC_LFRAME# */
	GPIO_FUNC1,	/* S0-SC047 - PCLK_TPM */
	GPIO_FUNC1,	/* S0-SC048 - CLK_PCI_EC */
	GPIO_FUNC1,	/* S0-SC049 - LPC_CLKRUN_L */
	GPIO_FUNC1,	/* S0-SC050 - IRQ_SERIRQ */
	GPIO_NC,	/* S0-SC051 - SMB_SOC_DATA (XDP) */
	GPIO_NC,	/* S0-SC052 - SMB_SOC_CLK (XDP) */
	GPIO_NC,	/* S0-SC053 - SMB_SOC_ALERTB (NC) */
	GPIO_DEFAULT,	/* S0-SC054 - NC */
	GPIO_INPUT,	/* S0-SC055 - TRACKPAD_INT_DX - INT */
	GPIO_INPUT,	/* S0-SC056 - GPIO_S0_SC_56 - STRAP */
	GPIO_FUNC1,	/* S0-SC057 - PCH_UART_TXD */
	GPIO_INPUT,	/* S0-SC058 - SIM_DET_C */
	GPIO_INPUT,	/* S0-SC059 - EC_IN_RW_C */
	GPIO_NC,	/* S0-SC060 - NC */
	GPIO_FUNC1,	/* S0-SC061 - SOC_UART_RX */
	GPIO_FUNC1,	/* S0-SC062 - I2S_BCLK */
	GPIO_FUNC1,	/* S0-SC063 - I2S_LRCLK */
	GPIO_FUNC1,	/* S0-SC064 - I2S_DIN */
	GPIO_FUNC1,	/* S0-SC065 - I2S_DOUT */
	GPIO_FUNC1,	/* S0-SC066 - SIO_SPI_CS# */
	GPIO_FUNC1,	/* S0-SC067 - SIO_SPI_MISO */
	GPIO_FUNC1,	/* S0-SC068 - SIO_SPI_MOSI */
	GPIO_FUNC1,	/* S0-SC069 - SIO_SPI_CLK */
	GPIO_INPUT,	/* S0-SC070 - ALS_INT_L - INT */
	GPIO_NC,	/* S0-SC071 - NC */
	GPIO_INPUT,	/* S0-SC072 - TOUCH_INT_L_DX - INT */
	GPIO_NC,	/* S0-SC073 - NC */
	GPIO_NC,	/* S0-SC074 - SIO_UART2_RXD (NC) */
	GPIO_NC,	/* S0-SC075 - SIO_UART2_TXD (NC) */
	GPIO_INPUT,	/* S0-SC076 - BIOS_STRAP - STRAP */
	GPIO_INPUT,	/* S0-SC077 - SOC_OVERRIDE - STRAP */
	GPIO_FUNC1,	/* S0-SC078 - I2C_0_SDA */
	GPIO_FUNC1,	/* S0-SC079 - I2C_0_SCL */
	GPIO_FUNC1,	/* S0-SC080 - I2C_1_SDA */
	GPIO_FUNC1,	/* S0-SC081 - I2C_1_SCL */
	GPIO_NC,	/* S0-SC082 - NC */
	GPIO_NC,	/* S0-SC083 - NC */
	GPIO_NC,	/* S0-SC084 - NC */
	GPIO_NC,	/* S0-SC085 - NC */
	GPIO_FUNC1,	/* S0-SC086 - I2C_4_SDA */
	GPIO_FUNC1,	/* S0-SC087 - I2C_4_SCL */
	GPIO_FUNC1,	/* S0-SC088 - I2C_5_SDA */
	GPIO_FUNC1,	/* S0-SC089 - I2C_5_SCL */
	GPIO_NC,	/* S0-SC090 - NC */
	GPIO_NC,	/* S0-SC091 - NC */
	GPIO_NC,	/* S0-SC092 - I2C_NGFF_SDA (NC/PU) */
	GPIO_NC,	/* S0-SC093 - I2C_NGFF_SCL (NC/PU) */
	GPIO_NC,	/* S0-SC094 - NC */
	GPIO_NC,	/* S0-SC095 - SIO_PWM1 (NC) */
	GPIO_FUNC1,	/* S0-SC096 - I2S_MCLK */
	GPIO_NC,	/* S0-SC097 - NC */
	GPIO_NC,	/* S0-SC098 - NC */
	GPIO_NC,	/* S0-SC099 - NC */
	GPIO_NC,	/* S0-SC100 - NC */
	GPIO_NC,	/* S0-SC101 - NC*/
	GPIO_END
};

/* SSUS GPIOs */
static const struct soc_gpio_map gpssus_gpio_map[] = {
	GPIO_NC,	/* S500 - PCH_WAKE# (NC) */
	GPIO_FUNC6,	/* S501 - TRACKPAD_INT# - INT */
	GPIO_FUNC6,	/* S502 - TOUCH_INT# - INT */
	GPIO_FUNC6,	/* S503 - LTE_WAKE_L# - INT */
	GPIO_NC,	/* S504 - SOC_JTAG2_TDO (NC/PU) */
	GPIO_NC,	/* S505 - SUS_CLK_WLAN (NC) */
	GPIO_INPUT_PU,	/* S506 - PCH_SPI_WP */
	GPIO_INPUT,	/* S507 - SOC_KBC_SMI - INT */
	GPIO_NC,	/* S508 - NC */
	GPIO_NC,	/* S509 - MUX_AUD_INT1# (NC) */
	GPIO_OUT_HIGH,	/* S510 - WIFI_DISABLE_L */
	GPIO_FUNC0,	/* S511 - SUSPWRDNACK */
	GPIO_FUNC0,	/* S512 - WIFI_SUSCLK */
	GPIO_FUNC0,	/* S513 - SLP_SX */
	GPIO_NC,	/* S514 - NC */
	GPIO_FUNC0,	/* S515 - WLAN_WAKE_L - INT */
	GPIO_FUNC0,	/* S516 - PCH_PWRBTN_L */
	GPIO_NC,	/* S517 - NC */
	GPIO_FUNC0,	/* S518 - SUS_STAT# */
	GPIO_FUNC0,	/* S519 - USB_OC0# */
	GPIO_FUNC0,	/* S520 - USB_OC1# */
	GPIO_NC,	/* S521 - NC */
	GPIO_NC,	/* S522 - XDP_GPIO_DFX0 */
	GPIO_NC,	/* S523 - XDP_GPIO_DFX1 */
	GPIO_NC,	/* S524 - XDP_GPIO_DFX2 */
	GPIO_NC,	/* S525 - XDP_GPIO_DFX3 */
	GPIO_NC,	/* S526 - XDP_GPIO_DFX4 */
	GPIO_NC,	/* S527 - XDP_GPIO_DFX5 */
	GPIO_NC,	/* S528 - XDP_GPIO_DFX6 */
	GPIO_NC,	/* S529 - XDP_GPIO_DFX7 */
	GPIO_NC,	/* S530 - XDP_GPIO_DFX8 */
	GPIO_NC,	/* S531 - NC */
	GPIO_NC,	/* S532 - NC */
	GPIO_NC,	/* S533 - NC */
	GPIO_NC,	/* S534 - NC */
	GPIO_OUT_HIGH,	/* S535 - LTE_DISABLE_L */
	GPIO_NC,	/* S536 - NC */
	GPIO_INPUT,	/* S537 - RAM_ID0 */
	GPIO_INPUT,	/* S538 - RAM_ID1 */
	GPIO_INPUT,	/* S539 - RAM_ID2 */
	GPIO_NC,	/* S540 - NC */
	GPIO_NC,	/* S541 - NC */
	GPIO_NC,	/* S542 - NC */
	GPIO_NC,	/* S543 - NC */
	GPIO_END
};

static struct soc_gpio_config gpio_config = {
	.ncore = gpncore_gpio_map,
	.score = gpscore_gpio_map,
	.ssus  = gpssus_gpio_map
};

struct soc_gpio_config* mainboard_get_gpios(void)
{
	return &gpio_config;
}
