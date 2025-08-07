/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>
#include <stdint.h>

#include <option.h>

static const u32 realtek_alc887_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x104384a8),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT2, 0x90430130),
	AZALIA_PIN_CFG(0, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_FRONT, 0x01014410),
	AZALIA_PIN_CFG(0, ALC887_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_MIC1, 0x01a19c50),
	AZALIA_PIN_CFG(0, ALC887_MIC2, 0x02a19c60),
	AZALIA_PIN_CFG(0, ALC887_LINE1, 0x0181345f),
	AZALIA_PIN_CFG(0, ALC887_LINE2, 0x02214c20),
	AZALIA_PIN_CFG(0, ALC887_CD, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_PC_BEEP, 0x4016c629),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT1, 0x01446140),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_IN, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC887",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x104384a8,
		.address      = 0,
		.verbs        = realtek_alc887_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc887_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862806,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	unsigned int ac97 = get_uint_option("audio_panel_type", 0) & 0x1;

	/*
	 * The verbs above are for a HD Audio front panel.
	 * With vendor firmware, if audio front panel type is set as AC97, line out 2
	 * (0x1b) and mic 2 (0x19) pins of ALC887 are configured differently.
	 *
	 * The differences are all in the "Misc" fields of configuration defaults (in byte 2)
	 * as shown below. ALC887 datasheet did not offer details on what those bits
	 * (listed as reserved in HDA spec) are, so we'll have to take their word for it.
	 *
	 * Pin  | 0x19 | 0x1b
	 * -----+------+-----
	 * HDA  | 1100 | 1100
	 * AC97 | 1001 | 0001
	 */

	const u32 verbs[] = {
		AZALIA_VERB_12B(0, 0x19, 0x71d, 0x99),
		AZALIA_VERB_12B(0, 0x1b, 0x71d, 0x41)
	};

	if ((viddid == 0x10ec0887) && ac97) {
		azalia_program_verb_table(base, verbs, ARRAY_SIZE(verbs));
	}
}
