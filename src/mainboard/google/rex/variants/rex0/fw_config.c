/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

#define GPIO_PADBASED_OVERRIDE(b, a) gpio_padbased_override(b, a, ARRAY_SIZE(a))

static const struct pad_config dmic_disable_pads[] = {
	PAD_NC(GPP_S02, NONE),
	PAD_NC(GPP_S03, NONE),
	PAD_NC(GPP_S06, NONE),
	PAD_NC(GPP_S07, NONE),
};

static const struct pad_config sndw_disable_pads[] = {
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
};

static const struct pad_config i2s_disable_pads[] = {
	PAD_NC(GPP_D09, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, DN_20K),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, dmic_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_UNKNOWN))) {
		printk(BIOS_INFO, "Configure GPIOs for no audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, dmic_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, MAX98363_CS42L42_SNDW))) {
		printk(BIOS_INFO, "Configure GPIOs for SoundWire audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_ALC5682I_I2S))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
	}
}
