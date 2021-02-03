/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* H4  : I2C0 SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H6  : I2C1 SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* B16 : I2C5 SDA */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF2),

	/* H5  : I2C0 SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H7  : I2C1 SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* B17 : I2C5 SCL */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF2),

	/* C5  : WWAN_PERST_N */
	PAD_CFG_GPO(GPP_C5, 1, PLTRST),
	/* E5  : WWAN_PERST# */
	PAD_CFG_GPO(GPP_E5, 1, PLTRST),
	/* D15 : WWAN_DISABLE_N */
	PAD_CFG_GPO(GPP_D15, 1, PLTRST),
	/* D9  : WWAN_FCP_POWER_OFF_N */
	PAD_CFG_GPO(GPP_D9, 1, PLTRST),

	/* D10 : PCH_SSD_PWR_EN */
	PAD_CFG_GPO(GPP_D10, 1, PLTRST),
	/* H0  : PCH_SSD_RST# */
	PAD_CFG_GPO(GPP_H0, 1, PLTRST),
	/* D16 : CPU_SSD_PWR_EN */
	PAD_CFG_GPO(GPP_D16, 1, PLTRST),
	/* H13 : CPU_SSD_RST# */
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),

	/* DDP1/2/A/B  CTRLCLK and CTRLDATA pins */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF4),
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF4),
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF4),
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF4),
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_A21, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_A22, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),

	/* HPD_1 (E14) and HPD_2 (A18) pins */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1),

	/* GPIO pin for PCIE SRCCLKREQB */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),

	/* CAM1_RST */
	PAD_CFG_GPO(GPP_R5, 1, PLTRST),
	/* CAM2_RST */
	PAD_CFG_GPO(GPP_E15, 1, PLTRST),
	/* CAM1_PWR_EN */
	PAD_CFG_GPO(GPP_B23, 1, PLTRST),
	/* CAM2_PWR_EN */
	PAD_CFG_GPO(GPP_E16, 1, PLTRST),
	/* IMGCLKOUT0 */
	PAD_CFG_NF(GPP_D4, NONE, DEEP, NF1),
	/* IMGCLKOUT1 */
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1),
};

void variant_configure_gpio_pads(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
