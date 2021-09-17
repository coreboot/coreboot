/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config dmic_enable_pads[] = {
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),	/* DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),	/* DMIC_DATA0_R */

};

static const struct pad_config dmic_disable_pads[] = {
	PAD_NC(GPP_R4, NONE),
	PAD_NC(GPP_R5, NONE),
};

static const struct pad_config i2s_enable_pads[] = {
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),	/* I2S_HP_SCLK_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),	/* I2S_HP_SFRM_R */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),	/* I2S_PCH_TX_HP_RX_STRAP */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),	/* I2S_PCH_RX_HP_TX */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),	/* I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),	/* I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),	/* I2S_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF4),	/* I2S_PCH_RX_SPKR_TX */
};

static const struct pad_config i2s_disable_pads[] = {
	PAD_NC(GPP_R0, NONE),
	PAD_NC(GPP_R1, NONE),
	PAD_NC(GPP_R2, NONE),
	PAD_NC(GPP_R3, NONE),
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_is_provisioned() || fw_config_probe(FW_CONFIG(AUDIO, AUDIO_UNKNOWN))) {
		printk(BIOS_INFO, "Disable audio related GPIO pins.\n");
		gpio_configure_pads(i2s_disable_pads, ARRAY_SIZE(i2s_disable_pads));
		gpio_configure_pads(dmic_disable_pads, ARRAY_SIZE(dmic_disable_pads));
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98390_ALC5682I_I2S_4SPK))) {
		printk(BIOS_INFO, "Configure audio over I2S with MAX98390 ALC5682I.\n");
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(i2s_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
