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

static const struct pad_config hda_enable_pads[] = {
	/* GPP_D10:     HDA_BCLK (HDR) */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF1),
	/* GPP_D11:     HDA_SYNC (HDR) */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF1),
	/* GPP_D12:     HDA_SDO (HDR) */
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF1),
	/* GPP_D13:     HDA_SDI0 (HDR) */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	/* GPP_D16:     HDA_RST_N (HDR) */
	PAD_CFG_NF(GPP_D16, NONE, DEEP, NF1),
	/* GPP_D17:     HDA_SDI1 (HDR) */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	/* GPP_S04:     DMIC0_CLK (HDR) */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* GPP_S05:     DMIC0_DATA (HDR) */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
	/* GPP_S06:     DMIC1_CLK (HDR) */
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF5),
	/* GPP_S07:     DMIC1_DATA (HDR) */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF5),
};

static const struct pad_config sndw_cs42l43_enable_pads[] = {
	/* Soundwire GPIO Config */
	/* GPP_S00:     SNDW3_CLK_CODEC (HDR) */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01:     SNDW3_DATA0_CODEC (HDR) */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02:     SNDW3_DATA1_CODEC (HDR) */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* GPP_S04:     DMIC0_CLK (HDR) */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* GPP_S05:     DMIC0_DATA (HDR) */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
};

static const struct pad_config audio_disable_pads[] = {
	/* GPP_D10:     HDA_BCLK (HDR) */
	PAD_NC(GPP_D10, NONE),
	/* GPP_D11:     HDA_SYNC (HDR) */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12:     HDA_SDO (HDR) */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13:     HDA_SDI0 (HDR) */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D16:     HDA_RST_N (HDR) */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17:     HDA_SDI1 (HDR) */
	PAD_NC(GPP_D17, NONE),
	/* GPP_S00:     SNDW3_CLK_CODEC (HDR) */
	PAD_NC(GPP_S00, NONE),
	/* GPP_S01:     SNDW3_DATA0_CODEC (HDR) */
	PAD_NC(GPP_S01, NONE),
	/* GPP_S02:     SNDW3_DATA1_CODEC (HDR) */
	PAD_NC(GPP_S02, NONE),
	/* GPP_S03:     SNDW3_DATA2_CODEC (HDR) */
	PAD_NC(GPP_S03, NONE),
	/* GPP_S04:     DMIC0_CLK (HDR) */
	PAD_NC(GPP_S04, NONE),
	/* GPP_S05:     DMIC0_DATA (HDR) */
	PAD_NC(GPP_S05, NONE),
	/* GPP_S06:     DMIC1_CLK (HDR) */
	PAD_NC(GPP_S06, NONE),
	/* GPP_S07:     DMIC1_DATA (HDR) */
	PAD_NC(GPP_S07, NONE),
};

static const struct pad_config pre_mem_gen4_ssd_pwr_seq1_pads[] = {
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 0, PLTRST),
};

static const struct pad_config pre_mem_gen4_ssd_pwr_seq2_pads[] = {
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 1, PLTRST),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_A08:     M2_GEN4_SSD_RESET_N */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
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

static const struct pad_config touchpad_lpss_i2c_enable_pads[] = {
	/* GPP_H20:     SOC_I2C_0_SCL */
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1),
	/* GPP_H19:     SOC_I2C_0_SDA */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),
	/* GPP_A13:     TCH_PAD_INT_N */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_NC(GPP_VGPIO3_THC1, NONE),
};

static const struct pad_config touchpad_i2c_disable_pads[] = {
	/* GPP_H20:     SOC_I2C_0_SCL */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H19:     SOC_I2C_0_SDA */
	PAD_NC(GPP_H19, NONE),
	/* GPP_A13:     TCH_PAD_INT_N */
	PAD_NC(GPP_A13, NONE),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_NC(GPP_VGPIO3_THC1, NONE),
};

static const struct pad_config fp_disable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_NC(GPP_C15, NONE),
	/* GPP_E19:     FPS_SOC_INT_L */
	PAD_NC(GPP_E19, NONE),
	/* GPP_E20:     FPMCU_FW_UPDATE */
	PAD_NC(GPP_E20, NONE),
	/* GPP_F14:     GPP_F14_GPSI0A_MOSI */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     GPP_F15_GSPI0A_MISO */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     GPP_F16_GPSI0A_CLK */
	PAD_NC(GPP_F16, NONE),
	/* GPP_F18:     GPP_F18_GSPI0_CS0 */
	PAD_NC(GPP_F18, NONE),
};

static const struct pad_config fp_enable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_CFG_GPO_LOCK(GPP_C15, 1, LOCK_CONFIG),
	/* GPP_E19:     FPS_SOC_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E19, NONE, PWROK, LEVEL, INVERT),
	/* GPP_E20:     FPMCU_FW_UPDATE */
	PAD_CFG_GPO_LOCK(GPP_E20, 0, LOCK_CONFIG),
	/* GPP_F14:     GPP_F14_GPSI0A_MOSI */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF8),
	/* GPP_F15:     GPP_F15_GSPI0A_MISO */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF8),
	/* GPP_F16:     GPP_F16_GPSI0A_CLK */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF8),
	/* GPP_F18:     GPP_F18_GSPI0_CS0 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF8),
};

static const struct pad_config pre_mem_fp_enable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_CFG_GPO(GPP_C15, 0, DEEP),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_seq1_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_seq1_pads);
	}

	if (fw_config_probe(FW_CONFIG(FP, FP_PRESENT)))
		GPIO_CONFIGURE_PADS(pre_mem_fp_enable_pads);

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_seq2_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_seq2_pads);
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	//const struct soc_intel_pantherlake_config *config = config_of_soc();

	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_NONE))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, audio_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_CS42L43_SNDW))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_cs42l43_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256_HDA))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, hda_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) ||
		fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(WIFI, WIFI_NONE)))  {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(TOUCHPAD, TOUCHPAD_LPSS_I2C))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_lpss_i2c_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_i2c_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(FP, FP_PRESENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_disable_pads);
	}
}
