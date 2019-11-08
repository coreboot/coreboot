/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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
	/* A0  : NC */
	PAD_NC(GPP_A0, NONE),
	/* A6  : NC */
	PAD_NC(GPP_A6, NONE),
	/* A8  : NC */
	PAD_NC(GPP_A8, NONE),
	/* A10 : NC */
	PAD_NC(GPP_A10, NONE),
	/* C12 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_C12, 0, DEEP),
	/* F1  : NC */
	PAD_NC(GPP_F1, NONE),
	/* F3  : MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F12 : EMMC_DATA0 ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* F13 : EMMC_DATA1 ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F14 : EMMC_DATA2 ==> NC */
	PAD_NC(GPP_F14, NONE),
	/* F15 : EMMC_DATA3 ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : EMMC_DATA4 ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* F17 : EMMC_DATA5 ==> NC */
	PAD_NC(GPP_F17, NONE),
	/* F18 : EMMC_DATA6 ==> NC */
	PAD_NC(GPP_F18, NONE),
	/* F19 : EMMC_DATA7 ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EMMC_RCLK ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EMMC_CLK ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F22 : EMMC_RESET# ==> NC */
	PAD_NC(GPP_F22, NONE),
	/* H19 : MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

const struct pad_config *override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/*
 * GPIOs configured before ramstage
 * Note: the Hatch platform's romstage will configure
 * the MEM_STRAP_* (a.k.a GPIO_MEM_CONFIG_*) pins
 * as inputs before it reads them, so they are not
 * needed in this table.
 */
static const struct pad_config early_gpio_table[] = {
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* C14 : BT_DISABLE_L */
	PAD_CFG_GPO(GPP_C14, 0, DEEP),
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* C21 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, PLTRST, LEVEL, INVERT),
	/* C23 : WLAN_PE_RST# */
	PAD_CFG_GPO(GPP_C23, 1, DEEP),
	/* E1  : M2_SSD_PEDET */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	/* E5  : SATA_DEVSLP1 */
	PAD_CFG_NF(GPP_E5, NONE, PLTRST, NF1),
	/* F2  : MEM_CH_SEL */
	PAD_CFG_GPI(GPP_F2, NONE, PLTRST),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
