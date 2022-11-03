/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A8  : WWAN_RF_DISABLE_ODL ==> NC */
	PAD_NC(GPP_A8, NONE),
	/* A12 : EN_PP3300_WWAN ==> NC */
	PAD_NC(GPP_A12, NONE),
	/* A19 : USB_C2_AUX_DC_P ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : USB_C2_AUX_DC_N ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : USB_C1_AUX_DC_P ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : USB_C1_AUX_DC_N ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* B5  : PCH_I2C_MISC_SDA ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : PCH_I2C_MISC_SCL ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* D5  : WWAN_DPR_SAR_ODL ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D8  : SD_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* D9  : USB_C2_LSX_TX ==> NC */
	PAD_NC(GPP_D9, NONE),
	/* D14 : SPKR_INT_L ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* E0 : WWAN_PERST_L ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E10 : WWAN_CONFIG0 ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E16 : WWAN_RST_L ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : WWAN_CONFIG2 ==> SSD_STRAP */
	PAD_CFG_GPI(GPP_E17, NONE, DEEP),
	/* E18 : USB_C0_LSX_TX ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E20 : USB_C1_LSX_TX ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* F6  : WWAN_WLAN_COEX3 ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F21 : WWAN_FCPO_L ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* H8  : WWAN_WLAN_COEX1 ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : WWAN_WLAN_COEX2 ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H21 : UCAM_MCLK ==>  NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : WCAM_MCLK ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : WWAN_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_H23, NONE),
	/* S0 : SDW_HP_CLK_R ==> NC */
	PAD_NC(GPP_S0, NONE),
	/* S1 : SDW_HP_DATA_R ==> NC */
	PAD_NC(GPP_S1, NONE),
	/* S4 : SDW_SPKR_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SDW_SPKR_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : DMIC_CLK1_R ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : DMIC_DATA1_R ==> NC */
	PAD_NC(GPP_S7, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
};

static const struct pad_config romstage_gpio_table[] = {
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
