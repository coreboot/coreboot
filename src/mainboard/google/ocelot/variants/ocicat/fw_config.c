/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config pcie_wlan_enable_pads[] = {
	/* GPP_A11:     WLAN_RST_N */
	PAD_CFG_GPO(GPP_A11, 1, PLTRST),
	/* GPP_E07:     WIFI_WAKE_N */
	PAD_CFG_GPI_SCI_LOW(GPP_E07, NONE, DEEP, LEVEL),
};

static const struct pad_config pcie_wlan_disable_pads[] = {
	/* GPP_A11:     WLAN_RST_N */
	PAD_NC(GPP_A11, NONE),
	/* GPP_E07:     WIFI_WAKE_N */
	PAD_NC(GPP_E07, NONE),
};

static const struct pad_config cnvi_enable_pads[] = {
	/* GPP_B09:     BT_RF_KILL_N */
	PAD_CFG_GPO(GPP_B09, 1, DEEP),
	/* GPP_C10:     WIFI_RF_KILL_N */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CRF_CLKREQ_R */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
};

static const struct pad_config cnvi_disable_pads[] = {
	/* GPP_B09:     BT_RF_KILL_N */
	PAD_NC(GPP_B09, NONE),
	/* GPP_C10:     WIFI_RF_KILL_N */
	PAD_NC(GPP_C10, NONE),
	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	PAD_NC(GPP_F00, NONE),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	PAD_NC(GPP_F01, NONE),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	PAD_NC(GPP_F02, NONE),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	PAD_NC(GPP_F03, NONE),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	PAD_NC(GPP_F04, NONE),
	/* GPP_F05:     CRF_CLKREQ_R */
	PAD_NC(GPP_F05, NONE),
};

static const struct pad_config ish_disable_pads[] = {
	/* GPP_B05:     C_EC_ISH_ALRT */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B18:     ISH_I2C2_SDA_SNSR_HDR */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     ISH_I2C2_SCL_SNSR_HDR */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B22:     ISH_GP_5_SNSR_HDR */
	PAD_NC(GPP_B22, NONE),
	/* GPP_B23:     ISH_GP_6_SNSR_HDR */
	PAD_NC(GPP_B23, NONE),
	/* GPP_D06:     ISH_UART0_ECAIC_TXD */
	PAD_NC(GPP_D06, NONE),
	/* GPP_F23:     SMC_LID / ISH_GP9A*/
	PAD_NC(GPP_F23, NONE),
};

static const struct pad_config ish_enable_pads[] = {
	/* GPP_B05:     C_EC_ISH_ALRT */
	PAD_CFG_NF(GPP_B05, NONE, DEEP, NF4),
	/* GPP_B18:     ISH_I2C2_SDA_SNSR_HDR */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B18, NONE, DEEP, NF1),
	/* GPP_B19:     ISH_I2C2_SCL_SNSR_HDR */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B19, NONE, DEEP, NF1),
	/* GPP_B22:     ISH_GP_5_SNSR_HDR */
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF4),
	/* GPP_B23:     ISH_GP_6_SNSR_HDR */
	PAD_CFG_NF(GPP_B23, NONE, DEEP, NF4),
	/* GPP_D06:     ISH_UART0_ECAIC_TXD */
	PAD_CFG_NF(GPP_D06, NONE, DEEP, NF2),
	/* GPP_F23:     SMC_LID / ISH_GP9A*/
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF8),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_PCIE_7))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_CNVI_7))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(SENSOR_HUB, ISH_PRESENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	}
}
