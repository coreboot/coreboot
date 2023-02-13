#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

static const struct pad_config i2s_enable_pads[] = {
	/* Audio: I2S */
	PAD_CFG_NF(GPP_D09, NONE, DEEP, NF2), /* I2S_MCLK1_OUT */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF2), /* I2S0_SCLK_HDR */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF2), /* I2S0_SFRM_HDR */
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF2), /* I2S0_TXD_HDR */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF2), /* I2S0_RXD_HDR */

	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF6), /* I2S1_SCLK_HDR */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF6), /* I2S1_SFRM_HDR */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF6), /* I2S1_TXD_HDR */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF6), /* I2S1_RXD_HDR */
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF3), /* DMIC_CLK_A1 */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF3), /* DMIC_DATA1 */
};

static const struct pad_config sndw_enable_pads[] = {
	/* Soundwire GPIO Config */
	/* DMIC config pads */
	PAD_CFG_NF(GPP_D09, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF2),
	/* Soundwire - External codec - JE Header */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF1),
	/* DMIC - JD Header */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF3),
};

static const struct pad_config sndw_alc711_enable_pads[] = {
	/* DMIC config pads */
	PAD_CFG_NF(GPP_D09, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF2),
	/* Soundwire GPIO Config */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* DMIC - JD Header */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF3),
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
};

static void fw_config_handle(void *unused)
{
	printk(BIOS_INFO, "FW config 0x%" PRIx64 "\n", fw_config_get());
	if (fw_config_probe(FW_CONFIG(AUDIO, NONE))) {
		printk(BIOS_INFO, "Configure GPIOs for no audio.\n");
		gpio_configure_pads(audio_disable_pads, ARRAY_SIZE(audio_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, MTL_ALC1019_ALC5682I_I2S)) ||
		fw_config_probe(FW_CONFIG(AUDIO, MTL_ALC5682I_MAX9857A_I2S))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio.\n");
		gpio_configure_pads(i2s_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, MTL_MAX98373_ALC5682_SNDW))) {
		printk(BIOS_INFO, "Configure GPIOs for SoundWire audio (ext codec).\n");
		gpio_configure_pads(sndw_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, MTL_ALC711_SNDW))) {
		printk(BIOS_INFO, "Configure GPIOs for SoundWire audio (onboard codec).\n");
		gpio_configure_pads(sndw_alc711_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}

}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
