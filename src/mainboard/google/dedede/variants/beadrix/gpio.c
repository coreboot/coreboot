/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <bootstate.h>
#include <fw_config.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* A10 : WWAN_EN */
	PAD_CFG_GPO(GPP_A10, 1, PWROK),
	/* A11 : TOUCH_RPT_EN ==> NC */
	PAD_NC(GPP_A11, NONE),
	/* A18 : USB_OC0_N */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),

	/* B7  : PCIE_CLKREQ2_N ==> WWAN_SAR_DETECT_ODL */
	PAD_CFG_GPO(GPP_B7, 1, DEEP),

	/* C18 : AP_I2C_EMR_SDA ==> NC */
	PAD_NC(GPP_C18, NONE),
	/* C19 : AP_I2C_EMR_SCL ==> NC */
	PAD_NC(GPP_C19, NONE),
	/* C22 : UART2_RTS_N ==> NC */
	PAD_NC(GPP_C22, NONE),
	/* C23 : UART2_CTS_N ==> NC */
	PAD_NC(GPP_C23, NONE),

	/* D0  : WWAN_HOST_WAKE ==> WWAN_WDISABLE_L */
	PAD_CFG_GPO(GPP_D0, 1, DEEP),
	/* D12 : WCAM_RST_L ==> NC */
	PAD_NC(GPP_D12, NONE),
	/* D13 : EN_PP2800_CAMERA ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D14 : EN_PP1200_CAMERA ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* D15 : UCAM_RST_L ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D19 : WWAN_WLAN_COEX1 ==> TP */
	PAD_NC(GPP_D19, NONE),
	/* D20 : WWAN_WLAN_COEX2 ==> TP */
	PAD_NC(GPP_D20, NONE),
	/* D21 : WWAN_WLAN_COEX3 ==> TP */
	PAD_NC(GPP_D21, NONE),
	/* D22 : AP_I2C_SUB_SDA*/
	PAD_CFG_NF(GPP_D22, NONE, DEEP, NF1),
	/* D23 : AP_I2C_SUB_SCL */
	PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1),

	/* E1  : EMR_RESET_L ==> NC */
	PAD_NC(GPP_E1, NONE),
	/* E2  : CLK_24M_WCAM ==> NC */
	PAD_NC(GPP_E2, NONE),
	/* E5  : AP_SUB_IO_2 ==> TP */
	PAD_NC(GPP_E5, NONE),
	/* E10 : GPP_E10/SML_DATA0 ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E11 : AP_I2C_SUB_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E11, NONE, PLTRST, LEVEL, NONE),

	/* G0  : SD_CMD ==> NC */
	PAD_NC(GPP_G0, NONE),
	/* G1  : SD_DATA0 ==> NC */
	PAD_NC(GPP_G1, NONE),
	/* G2  : SD_DATA1 ==> NC */
	PAD_NC(GPP_G2, NONE),
	/* G3  : SD_DATA2 ==> NC */
	PAD_NC(GPP_G3, NONE),
	/* G4  : SD_DATA3 ==> NC */
	PAD_NC(GPP_G4, NONE),
	/* G5  : SD_CD_ODL ==> NC */
	PAD_NC(GPP_G5, NONE),
	/* G6  : SD_CLK ==> NC */
	PAD_NC(GPP_G6, NONE),
	/* G7  : SD_SDIO_WP ==> NC */
	PAD_NC(GPP_G7, NONE),

	/* H1  : EN_PP3300_SD_U ==> NC */
	PAD_NC(GPP_H1, NONE),
	/* H6  : AP_I2C_CAM_SDA ==> NC */
	PAD_NC(GPP_H6, NONE),
	/* H7  : AP_I2C_CAM_SCL ==> NC */
	PAD_NC(GPP_H7, NONE),
	/* H17 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_H17, 0, PLTRST),
};

static const struct pad_config lte_disable_pads[] = {
	PAD_NC(GPP_A10, NONE),
	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_H17, NONE),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

static void fw_config_handle(void *unused)
{
	 if (!fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1C_LTE)))
		gpio_configure_pads(lte_disable_pads, ARRAY_SIZE(lte_disable_pads));
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
