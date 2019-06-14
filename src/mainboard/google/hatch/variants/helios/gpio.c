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
	/* A0  : RCIN# ==> NC */
	PAD_NC(GPP_A0, NONE),
	/* A6  : SERIRQ ==> NC */
	PAD_NC(GPP_A6, NONE),
	/* A18 : ISH_GP0 ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : ISH_GP1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : ISH_GP2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* B19 : GSPI1_CS0# ==> NC */
	PAD_NC(GPP_B19, NONE),
	/* C1  : SMBDATA ==> NC */
	PAD_NC(GPP_C1, NONE),
	/* C6  : GPP_C6 ==> NC */
	PAD_NC(GPP_C6, NONE),
	/* C7  : GPP_C7 ==> NC */
	PAD_NC(GPP_C7, NONE),
	/* C15 : UART1_CTS# ==> NC */
	PAD_NC(GPP_C15, NONE),
	/* C23 : UART2_CTS# ==> NC */
	PAD_NC(GPP_C23, NONE),
	/* D5  : ISH_I2C0_SDA ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D6  : ISH_I2C0_SCL ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D7  : ISH_I2C1_SDA ==> NC */
	PAD_NC(GPP_D7, NONE),
	/* D8  : ISH_I2C1_SCL ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* D10 : ISH_SPI_CLK ==> EN_PP3300_PP1800_FP */
	PAD_CFG_GPO(GPP_D10, 0, DEEP),
	/* D21 : SPI1_IO2 ==> NC */
	PAD_NC(GPP_D21, NONE),
	/* F0  : GPP_F0 ==> NC */
	PAD_NC(GPP_F0, NONE),
	/* F1  : GPP_F1 ==> NC */
	PAD_NC(GPP_F1, NONE),
	/* F3  : GPP_F3 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : GPP_F10 ==> MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F20 : EMMC_RCLK ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EMMC_CLK ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F22 : EMMC_RESET# ==> NC */
	PAD_NC(GPP_F22, NONE),
	/* G0  : GPP_G0 ==> NC  */
	PAD_NC(GPP_G0, NONE),
	/* G1  : GPP_G1 ==> NC  */
	PAD_NC(GPP_G1, NONE),
	/* G2  : GPP_G2 ==> NC  */
	PAD_NC(GPP_G2, NONE),
	/* G3  : GPP_G3 ==> NC  */
	PAD_NC(GPP_G3, NONE),
	/* G4  : GPP_G4 ==> NC  */
	PAD_NC(GPP_G4, NONE),
	/* G5  : GPP_G5 ==> NC  */
	PAD_NC(GPP_G5, NONE),
	/* G6  : GPP_G6 ==> NC  */
	PAD_NC(GPP_G6, NONE),
	/* H4  : I2C2_SDA ==> NC */
	PAD_NC(GPP_H4, NONE),
	/* H5  : I2C2_SCL ==> NC */
	PAD_NC(GPP_H5, NONE),
	/* H13 : M2_SKT2_CFG1 ==> SPKR_RST_L */
	PAD_CFG_GPO(GPP_H13, 0, PLTRST),
	/* H14 : M2_SKT2_CFG2 ==> TOUCHSCREEN_STOP_L */
	PAD_CFG_GPO(GPP_H14, 0, PLTRST),
	/* H19 : TIMESYNC[0] ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

const struct pad_config *override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* GPIOs configured before ramstage */
static const struct pad_config early_gpio_table[] = {
	PAD_NC(GPP_C23, NONE),
};

const struct pad_config *override_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/*
 * GPIO settings before entering all sleep states
 */
static const struct pad_config sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 1, DEEP), /* FPMCU_RST_ODL */
};

const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num)
{
	*num = ARRAY_SIZE(sleep_gpio_table);
	return sleep_gpio_table;
}
