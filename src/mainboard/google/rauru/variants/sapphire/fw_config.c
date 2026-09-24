/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <baseboard/panel.h>
#include <fw_config.h>
#include <variants.h>

enum audio_amplifier_id get_audio_amp_id(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_AMPLIFIER, AUDIO_AMPLIFIER_TAS2563)))
		return AUD_AMP_ID_TAS2563;

	return AUD_AMP_ID_UNKNOWN;
}

void fw_config_mainboard_override(void)
{
	if (!fw_config_is_provisioned()) {
		/*
		 * Sapphire only supports TAS2563,
		 * so set it to enable audio on unprovisioned devices.
		 */
		uint64_t value = FW_CONFIG_VALUE(AUDIO_AMPLIFIER, AUDIO_AMPLIFIER_TAS2563);
		fw_config_override_field(FW_CONFIG_FIELD(AUDIO_AMPLIFIER), value);
	}

	fw_config_override_field(FW_CONFIG_FIELD(PANEL_ID), panel_id());
}
