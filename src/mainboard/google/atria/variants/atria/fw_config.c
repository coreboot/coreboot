/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config pre_mem_gen4_ssd_pads[] = {
	/* GPP_B10:     SOC_M2_GEN4_SSD3_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
};

static const struct pad_config pre_mem_gen5_ssd_pads[] = {
	/* GPP_E04:     SOC_M2_GEN5_SSD2_PWREN */
	PAD_CFG_GPO(GPP_E04, 1, DEEP),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_B10:     SOC_M2_GEN4_SSD3_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
	/* GPP_A11: 	SOC_M2_GEN4_SSD3_RESET_N */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),
};

static const struct pad_config gen5_ssd_pads[] = {
	/* GPP_E04:     SOC_M2_GEN5_SSD2_PWREN */
	PAD_CFG_GPO(GPP_E04, 1, DEEP),
	/* GPP_E03: 	SOC_M2_GEN5_SSD2_RESET_N */
	PAD_CFG_GPO(GPP_E03, 1, DEEP),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_B25: 	UFS_RST_N */
	PAD_CFG_GPO(GPP_B25, 1, DEEP),
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
};

static const struct pad_config ish_disable_pads[] = {
	/* GPP_B02: 	ISH_I3C0_SDA_SNSR_HDR_R */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03: 	ISH_I3C0_SCL_SNSR_HDR_R */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04: 	ISH_GP_0_SNSR_HDR */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05: 	ISH_GP_1_SNSR_HDR */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B06: 	ISH_GP_2_SNSR_HDR */
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07: 	ISH_GP_3_SNSR_HDR */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08: 	ISH_GP_4_SNSR_HDR */
	PAD_NC(GPP_B08, NONE),
	/* GPP_B18: 	ISH_I2C2_SDA_SNSR_HDR */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19: 	ISH_I2C2_SCL_SNSR_HDR */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B22: 	ISH_GP_5_SNSR_HDR */
	PAD_NC(GPP_B22, NONE),
	/* GPP_B23: 	ISH_GP_6_SNSR_HDR */
	PAD_NC(GPP_B23, NONE),

	/* GPP_D05: 	ISH_SPI_CS_N_SNSR_HDR */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06: 	ISH_SPI_CLK_SNSR_HDR */
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07: 	ISH_SPI_MISO_SNSR_HDR */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08: 	ISH_SPI_MOSI_SNSR_HDR */
	PAD_NC(GPP_D08, NONE),

	/* GPP_F08: 	ISH_GP_7A_SNSR_HDR_R */
	PAD_NC(GPP_F08, NONE),
	/* GPP_F09: 	ISH_INT_GP11_CVS */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F23:     ISH_GP_9A; NOTE: ISH will handle SMC_LID; not AP */
	PAD_NC(GPP_F23, NONE),

	/* GPP_H14: 	ISH_I2C1_ISH_I3C1_SDA_SNSR_HDR */
	PAD_NC(GPP_H14, NONE),
	/* GPP_H15: 	ISH_I2C1_ISH_I3C1_SCL_SNSR_HDR */
	PAD_NC(GPP_H15, NONE),
};

static const struct pad_config sndw_alc721_enable_pads[] = {
	/* SNDW3_CLK   */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* SNDW3_DATA0 */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* SNDW3_DATA1 */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* SNDW3_DATA2 */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF1),
	/* DMIC_CLK_A0 */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* DMIC_DATA_0 */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
	/* SNDW1_CLK   */
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF5),
	/* SNDW1_DATA  */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF5),
	/* DMIC_CLK (native PCH DMIC1) - unused, keep disabled */
	PAD_NC(GPP_D16, NONE),
	/* DMIC_DATA (native PCH DMIC1) - unused, keep disabled */
	PAD_NC(GPP_D17, NONE),
};

static const struct pad_config audio_disable_pads[] = {
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S02, NONE),
	PAD_NC(GPP_S03, NONE),
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
	PAD_NC(GPP_S06, NONE),
	PAD_NC(GPP_S07, NONE),
	PAD_NC(GPP_D09, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, NONE),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned\n");
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pads);
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN5))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pads);
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN4))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN5))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(SENSOR_HUB, ISH_ABSENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_ALC721))) {
		printk(BIOS_INFO, "Configure GPIOs for ALC721/ALC722 SoundWire audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_alc721_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_ABSENT))) {
		printk(BIOS_INFO, "Audio codec absent; disable audio GPIOs.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, audio_disable_pads);
	}
}
