/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

static const struct pad_config gpio_table[] = {
	/* SD_1P8_SEL => NC */
	PAD_NC(GPP_A16, DN_20K),
	/* EN_PP3300_SD_DX */
	PAD_CFG_NF(GPP_A17, NONE, DEEP, NF1),
	/* EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A18, 1, DEEP),
	/* WWAN_RADIO_DISABLE_1V8_ODL */
	PAD_CFG_GPO(GPP_A19, 1, DEEP),
	/* TRACKPAD_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A21, NONE, PLTRST, LEVEL, INVERT),
	/* SRCCLKREQ1 */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),
	/* PCIE_14_WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* TOUCHSCREEN_DIS_L */
	PAD_CFG_GPO(GPP_C4, 0, DEEP),
	/* PCIE_14_WLAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_C1, NONE, DEEP, EDGE_SINGLE),
	/* GPP_C10_TP */
	PAD_NC(GPP_C10, DN_20K),
	/* GPP_C11_TP */
	PAD_NC(GPP_C11, DN_20K),
	/* BT_DISABLE_L */
	PAD_CFG_GPO(GPP_C14, 1, DEEP),
	/* WWAN_DPR_SAR_ODL
	 *
	 * TODO: Driver doesn't use this pin as of now. In case driver starts
	 * using this pin, expose this pin to driver.
	 */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),
	/* PCH_I2C_TRACKPAD_SDA */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* PCH_I2C_TRACKPAD_SCL */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* PCH_I2C_TOUCHSCREEN_SDA */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* PCH_I2C_TOUCHSCREEN_SCL */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, DEEP, LEVEL, INVERT),
	/* EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* WLAN_PE_RST# */
	PAD_CFG_GPO(GPP_C23, 1, DEEP),
	/* WWAN_CONFIG_0 */
	PAD_NC(GPP_D5, NONE),
	/* WWAN_CONFIG_1 */
	PAD_NC(GPP_D6, NONE),
	/* WWAN_CONFIG_2 */
	PAD_NC(GPP_D7, NONE),
	/* WWAN_CONFIG_3 */
	PAD_NC(GPP_D8, NONE),
	/* TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 0, DEEP),
	/* TOUCHSCREEN_INT_L */
	PAD_CFG_GPI_APIC(GPP_D16, NONE, DEEP, LEVEL, INVERT),
	/* SATAGP1 */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF2),
	/* M2_SSD_PE_WAKE_ODL */
	PAD_CFG_GPI(GPP_E4, NONE, DEEP),
	/* SATA_DEVSLP1 */
	PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1),
	/* USB_C_OC_OD USB_OC2*/
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),
	/* USB_A_OC_OD USB_OC3*/
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* USB_C0_DP_HPD */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* DDI2_HPD_ODL */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* DDPD_HPD2 =>  NC */
	PAD_NC(GPP_E15, DN_20K),
	/* DDPE_HPD2 => NC */
	PAD_NC(GPP_E16, DN_20K),
	/* EDP_HPD */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
	/* DDPB_CTRLCLK => NC */
	PAD_NC(GPP_E18, DN_20K),
	/* DDPC_CTRLCLK => NC */
	PAD_NC(GPP_E20, DN_20K),
	/* DDPD_CTRLCLK => NC */
	PAD_NC(GPP_E22, DN_20K),
	/* GPIO_WWAN_WLAN_COEX3 */
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1),
	/* WWAN_RESET_1V8_ODL */
	PAD_CFG_GPO(GPP_F1, 1, DEEP),
	/* UART_WWANTX_WLANRX_COEX1 */
	PAD_CFG_NF(GPP_F8, NONE, DEEP, NF1),
	/* UART_WWANRX_WLANTX_COEX2 */
	PAD_CFG_NF(GPP_F9, NONE, DEEP, NF1),
	/* PCH_MEM_STRAP0 */
	PAD_CFG_GPI(GPP_F20, NONE, PLTRST),
	/* PCH_MEM_STRAP1 */
	PAD_CFG_GPI(GPP_F21, NONE, PLTRST),
	/* PCH_MEM_STRAP2 */
	PAD_CFG_GPI(GPP_F11, NONE, PLTRST),
	/* PCH_MEM_STRAP3 */
	PAD_CFG_GPI(GPP_F22, NONE, PLTRST),
	/* SD_CMD */
	PAD_CFG_NF(GPP_G0, NONE, DEEP, NF1),
	/* SD_DATA0 */
	PAD_CFG_NF(GPP_G1, NONE, DEEP, NF1),
	/* SD_DATA1 */
	PAD_CFG_NF(GPP_G2, NONE, DEEP, NF1),
	/* SD_DATA2 */
	PAD_CFG_NF(GPP_G3, NONE, DEEP, NF1),
	/* SD_DATA3 */
	PAD_CFG_NF(GPP_G4, NONE, DEEP, NF1),
	/* SD_CD# */
	PAD_CFG_NF(GPP_G5, NONE, DEEP, NF1),
	/* SD_CLK */
	PAD_CFG_NF(GPP_G6, NONE, DEEP, NF1),
	/* SD_WP => NC */
	PAD_NC(GPP_G7, DN_20K),
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* GPIOs needed prior to ramstage. */
static const struct pad_config early_gpio_table[] = {
	/* H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, DEEP, LEVEL, INVERT),
	/* WLAN_PE_RST# */
	PAD_CFG_GPO(GPP_C23, 1, DEEP),
};

const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
};

const struct cros_gpio *__weak variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
