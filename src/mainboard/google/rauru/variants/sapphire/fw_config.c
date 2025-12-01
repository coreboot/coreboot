/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <fw_config.h>
#include <variants.h>

enum audio_amplifier_id get_audio_amp_id(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_AMPLIFIER, AUDIO_AMPLIFIER_TAS2563)))
		return AUD_AMP_ID_TAS2563;

	return AUD_AMP_ID_UNKNOWN;
}
