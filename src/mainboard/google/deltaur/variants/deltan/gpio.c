/*
 * This file is part of the coreboot project.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {

};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A23 : GPP_A23 ==> RECOVERY# */
	PAD_CFG_GPI(GPP_A23, NONE, DEEP),
	/* C20 : GPP_C20 ==> PCHRX_SERVOTX_UART */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* C21 : CPP_G21 ==> PCHTX_SERVORX_UART */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* C22 : GPP_C22 ==> H1_FLASH_WP */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* C23 : GPP_C23 ==> H1_PCH_INT# */
	PAD_CFG_GPI_APIC(GPP_C23, NONE, DEEP, LEVEL, INVERT),
	/* E3  : GPP_E3 ==> MEM_INTERLEAVED */
	PAD_CFG_GPI(GPP_E3, NONE, PLTRST),
	/* F11 : GPP_F11 ==> MEM_CONFIG0_1P8 */
	PAD_CFG_GPI(GPP_F11, NONE, DEEP),
	/* F12 : GPP_F12 ==> MEM_CONFIG1_1P8 */
	PAD_CFG_GPI(GPP_F12, NONE, DEEP),
	/* F13 : GPP_F13 ==> MEM_CONFIG2_1P8 */
	PAD_CFG_GPI(GPP_F13, NONE, DEEP),
	/* F14 : GPP_F14 ==> MEM_CONFIG3_1P8 */
	PAD_CFG_GPI(GPP_F14, NONE, DEEP),
	/* F15 : GPP_F15 ==> MEM_CONFIG4_1P8 */
	PAD_CFG_GPI(GPP_F15, NONE, DEEP),
	/* F16 : GPP_F16 ==> WWAN_BB_RST#_1P8 */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),
	/* H4  : GPP_H4 ==> DDR_CHA_EN_1P8 */
	PAD_CFG_GPI(GPP_H4, NONE, DEEP),
	/* H5  : GPP_H5 ==> DDR_CHB_EN_1P8 */
	PAD_CFG_GPI(GPP_H5, NONE, DEEP),
	/* H6  : GPP_H6 ==> I2C_SDA_PCH_H1 */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : GPP_H7 ==> I2C_SCL_PCH_H1 */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* GPD3: GPD3 ==> SIO_PWRBTN# */
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
