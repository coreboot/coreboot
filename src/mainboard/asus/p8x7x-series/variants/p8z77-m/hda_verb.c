/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>
#include <stdint.h>

#include <option.h>

const u32 cim_verb_data[] = {
	0x10ec0887, /* Codec Vendor / Device ID: Realtek */
	0x104384a8, /* Subsystem ID */
	15,         /* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x104384a8),
	AZALIA_PIN_CFG(0, 0x11, 0x90430130),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01014410),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19c50),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c60),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181345f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4016c629),
	AZALIA_PIN_CFG(0, 0x1e, 0x01446140),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),

	0x80862806, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */
	4,          /* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

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
