/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
/* I2S2_SCLK */		PAD_CFG_GPI(GPP_A7, NONE, PLTRST),
/* I2S2_RXD */		PAD_CFG_GPI(GPP_A10, NONE, PLTRST),
/* ONBOARD_X4_PCIE_SLOT1_PWREN_N */	PAD_CFG_GPO(GPP_A14, 0, DEEP),
/* USB_OC_ODL */		PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
/* WLAN_PCIE_WAKE_L */	PAD_CFG_GPI_SCI(GPP_B6, NONE, DEEP, EDGE_SINGLE,
			INVERT),
/* PCH_WP_OD */	PAD_CFG_GPI(GPP_B7, UP_20K, DEEP),
/* PCH_SPI_FPMCU_F7_CS_L */	PAD_CFG_NF(GPP_B23, NONE, DEEP, NF1),
/* PEN_INT_ODL */	PAD_CFG_GPI(GPP_C2, NONE, PLTRST),
/* GPP_C5_STRAP */	PAD_CFG_GPO(GPP_C5, 1, DEEP),
/* TCH_INT_ODL */			PAD_CFG_GPI(GPP_C10, NONE, PLTRST),
/* TCH_RST_ODL */		PAD_CFG_GPO(GPP_C11, 0, PLTRST),
/* M2_SHUTDOWN+L */		PAD_CFG_GPO(GPP_C12, 1, PLTRST),
/* M2_RESET_L */		PAD_CFG_GPO(GPP_C13, 1, PLTRST),
/* M2_INT_L */			PAD_CFG_GPI(GPP_C14, NONE, PLTRST),
/* HP_INT_L */			PAD_CFG_GPI(GPP_C15, NONE, PLTRST),
/* PCH_DEV_INT_ODL */			PAD_CFG_GPI(GPP_C22, NONE, PLTRST),
/* PCH_DEV_RESET_L */		PAD_CFG_GPO(GPP_C23, 0, PLTRST),
/* TRACKPAD_WAKE */	PAD_CFG_GPI_SCI(GPP_D0, NONE, DEEP, EDGE_SINGLE,
			INVERT),
/* TRACKPAD_INT_ODL */	PAD_CFG_GPI_APIC(GPP_D1, NONE, DEEP, EDGE_SINGLE,
						 INVERT),
/* PCI_NVME_CLKREQ_ODL */		PAD_CFG_NF(GPP_D7, UP_20K, PWROK, NF1),
/* H1_PCH_INT_ODL */	PAD_CFG_GPI_APIC(GPP_D16, UP_20K, PLTRST, LEVEL,
		INVERT),
/* GPP_E6_STRAP */		PAD_CFG_GPO(GPP_E6, 1, DEEP),
/* USB_C0_SBU_1_DC */		PAD_CFG_GPO(GPP_E22, 1, PLTRST),
/* USB_C0_SBU_2_DC */	PAD_CFG_GPO(GPP_E23, 0, DEEP),
/* CNV_RF_RESET_N */		PAD_CFG_NF(GPP_F4, DN_20K, PWROK, NF1),
/* CNV_CLKREQ0 */		PAD_CFG_NF(GPP_F5, DN_20K, PWROK, NF2),
/* SPKR_IRQ_L */	PAD_CFG_GPI_APIC(GPP_F6, NONE, DEEP, LEVEL, NONE),
/* SPKR_RST_L */	PAD_CFG_GPO(GPP_F19, 1, DEEP),
/* SD_CD# */		PAD_CFG_GPI_GPIO_DRIVER(GPP_G5, UP_20K, DEEP),
/* SD_WP */		PAD_CFG_NF(GPP_G7, DN_20K, DEEP, NF1),
/* I2C3_SDA */		PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
/* I2C3_SCL */		PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
/* PCH_MEM_STRAP0 */		PAD_CFG_GPI(GPP_H12, DN_20K, PLTRST),
/* PCH_MEM_STRAP1 */		PAD_CFG_GPI(GPP_H13, DN_20K, PLTRST),
/* PCH_MEM_STRAP2 */		PAD_CFG_GPI(GPP_H14, DN_20K, PLTRST),
/* PCH_MEM_STRAP3 */		PAD_CFG_GPI(GPP_H15, DN_20K, PLTRST),
/* I2S0_SCLK */		PAD_CFG_GPO(GPP_R0, 1, DEEP),
/* I2S0_SFRM */		PAD_CFG_GPO(GPP_R1, 1, DEEP),
/* I2S0_TXD */		PAD_CFG_GPO(GPP_R2, 1, DEEP),
/* I2S0_RXD */		PAD_CFG_GPI(GPP_R3, NONE, DEEP),
};
/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
/* PCH_WP_OD */	PAD_CFG_GPI(GPP_B7, UP_20K, DEEP),
/* GSPI0_CS# */	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
/* GSPI0_CLK */	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
/* GSPI0_MISO */	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
/* GSPI0_MOSI */	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
/* H1_PCH_INT_ODL */	PAD_CFG_GPI_APIC(GPP_D16, UP_20K, PLTRST, LEVEL,
		INVERT),
};

const struct pad_config *__attribute__((weak)) variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__attribute__((weak))
	variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(39, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *__attribute__((weak)) variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
