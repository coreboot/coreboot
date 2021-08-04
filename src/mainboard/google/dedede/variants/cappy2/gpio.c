/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <bootstate.h>
#include <fw_config.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* D15 : EN_PP3300_CAMERA */
	PAD_CFG_GPO(GPP_D15, 1, PLTRST),
	/* H16 : AP_SUB_IO_L ==> HP_RST_ODL */
	PAD_CFG_GPO(GPP_H16, 1, PWROK),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

static const struct pad_config codec_rt5682_pads[] = {
	PAD_NC(GPP_H16, NONE),
};

static const struct pad_config codec_cs42l42_pads[] = {
	PAD_NC(GPP_D18, NONE),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC_SOURCE, AUDIO_CODEC_ALC5682)))
		gpio_configure_pads(codec_rt5682_pads, ARRAY_SIZE(codec_rt5682_pads));
	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC_SOURCE, AUDIO_CODEC_CS42l42)))
		gpio_configure_pads(codec_cs42l42_pads, ARRAY_SIZE(codec_cs42l42_pads));
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
