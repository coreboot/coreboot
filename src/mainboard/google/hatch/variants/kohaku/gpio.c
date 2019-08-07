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
	/* A10 : PEN_RESET_ODL */
	PAD_CFG_GPO(GPP_A10, 1, DEEP),
	/* A16 : EMR_GARAGE_DET (notification) */
	PAD_CFG_GPI_APIC(GPP_A16, NONE, PLTRST, LEVEL, NONE),
	/* A17 : PIRQA# ==> NC */
	PAD_NC(GPP_A17, NONE),
	/* A18 : ISH_GP0 ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : ISH_GP1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : ISH_GP2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A22 : ISH_GP4 ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* B8  : SRCCLKREQ3#: NC  */
	PAD_NC(GPP_B8, NONE),
	/* C1  : SMBDATA: NC  */
	PAD_NC(GPP_C1, NONE),
	/* C7  : PEN_IRQ_OD_L */
	PAD_CFG_GPI_APIC(GPP_C7, NONE, PLTRST, LEVEL, INVERT),
	/* C12 : EN_PP3300_TSP_DX */
	PAD_CFG_GPO(GPP_C12, 0, DEEP),
	/* C15 : EN_PP3300_DIG_DX */
	PAD_CFG_GPO(GPP_C15, 0, DEEP),
	/* C23 : UART2_CTS# ==> NC */
	PAD_NC(GPP_C23, NONE),
	/* D16 : TOUCHSCREEN_INT_L */
	PAD_CFG_GPI_APIC(GPP_D16, NONE, PLTRST, LEVEL, INVERT),
	/* E23 : GPP_E23 ==> NC */
	PAD_NC(GPP_E23, NONE),
	/* F1 : GPP_F1 ==> NC */
	PAD_NC(GPP_F1, NONE),
	/* G0 : GPP_G0 ==> NC  */
	PAD_NC(GPP_G0, NONE),
	/* G1 : GPP_G1 ==> NC  */
	PAD_NC(GPP_G1, NONE),
	/* G2 : GPP_G2 ==> NC  */
	PAD_NC(GPP_G2, NONE),
	/* G3 : GPP_G3 ==> NC  */
	PAD_NC(GPP_G3, NONE),
	/* G4 : GPP_G4 ==> NC  */
	PAD_NC(GPP_G4, NONE),
	/* G5 : GPP_G5 ==> NC  */
	PAD_NC(GPP_G5, NONE),
	/* G6 : GPP_G6 ==> NC  */
	PAD_NC(GPP_G6, NONE),
	/* H4  : PCH_I2C_PEN_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : PCH_I2C_PEN_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
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
