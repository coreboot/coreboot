/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config sndw_alc721_enable_pads[] = {
	/* GPP_S00: SNDW3_CLK */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01: SNDW3_DATA0 */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02: SNDW3_DATA1 */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* GPP_S03: SNDW3_DATA2 */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF1),
	/* GPP_S04: PCH_DMIC_CLK */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* GPP_S05: PCH_DMIC_DAT */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
};

static const struct pad_config audio_disable_pads[] = {
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S02, NONE),
	PAD_NC(GPP_S03, NONE),
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
};

static const struct pad_config pre_mem_ssd_pwr_seq_pads[] = {
	/* GPP_H18: EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_H18, 1, PLTRST),
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

static const struct pad_config ish_disable_pads[] = {
	/* GPP_B05:     C_EC_ISH_ALRT */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B18:     ISH_I2C2_SDA_SNSR_HDR */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     ISH_I2C2_SCL_SNSR_HDR */
	PAD_NC(GPP_B19, NONE),
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
	/* GPP_D06:     ISH_UART0_ECAIC_TXD */
	PAD_CFG_NF(GPP_D06, NONE, DEEP, NF2),
	/* GPP_F23:     SMC_LID / ISH_GP9A*/
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF8),
};

void fw_config_configure_pre_mem_gpio(void)
{
	GPIO_CONFIGURE_PADS(pre_mem_ssd_pwr_seq_pads);
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_ALC721))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_alc721_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, audio_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_CNVI)))
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_enable_pads);

	if (fw_config_probe(FW_CONFIG(SENSOR_HUB, ISH_PRESENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	}
}
