/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Lenovo ThinkPad X61 - HD Audio (Azalia) Codec Verb Table
 *
 * Extracted from Phoenix BIOS bioscode_7.rom:
 *   - Verb table header at file offset 0x2B70
 *   - 60 verb dwords at file offset 0x2B7D
 *   - Codec lookup dispatch at 0x5329
 *   - Verb send loop at 0x52D7
 *
 * Primary codec: Analog Devices AD1984
 *   Vendor/Device ID: 0x11D41984
 *   Subsystem ID:     0x17AA20D6 (Lenovo ThinkPad X61)
 */

#include <device/azalia_device.h>

static const u32 ad1984_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x17aa20d6),

	/* NID 0x11: Headphone output jack (right side) */
	AZALIA_PIN_CFG(0, 0x11, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
		AZALIA_HP_OUT,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_GREEN,
		AZALIA_JACK_PRESENCE_DETECT,
		1, 15
	)),

	/* NID 0x12: Internal speaker */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_SPEAKER,
		AZALIA_OTHER_ANALOG,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		1, 0
	)),

	AZALIA_PIN_CFG(0, 0x13, AZALIA_PIN_CFG_NC(0)),

	/* NID 0x14: External mic input jack (right side) */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
		AZALIA_MIC_IN,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_RED,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 1
	)),

	/* NID 0x15: Internal microphone */
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_MIC_IN,
		AZALIA_OTHER_ANALOG,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		2, 14
	)),

	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(1)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(2)),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(3)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(4)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(5)),

	/* NID 0x1C: Dock mic input */
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_SEPARATE_CHASSIS | AZALIA_REAR,
		AZALIA_MIC_IN,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_RED,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),

	/*
	 * Extra init verbs from BIOS at bioscode_7.rom:0x2C2D
	 *
	 * Configure amplifier gains, digital converter modes,
	 * beep generator, power states, and EAPD.
	 */

	/* DAC output amp gains */
	0x00c3b027,	/* NID 0x0C: Set output amp, L+R, gain 39 (~-6dB) */
	0x00d3b027,	/* NID 0x0D: Set output amp, L+R, gain 39 (~-6dB) */
	/* Digital converter - set to 0 (PCM mode) */
	0x00737100,	/* NID 0x07: Set digital converter = 0x00 */
	0x00a37100,	/* NID 0x0A: Set digital converter = 0x00 */

	/* Beep generator config */
	0x02037318,	/* NID 0x20: Set beep amp, index 3, gain 24 */
	0x0213b01f,	/* NID 0x21: Set output amp, L+R, gain 31 (max) */
	/* Mute HP Out and Speaker output amps initially */
	0x0113b000,	/* NID 0x11: Set output amp, L+R, gain 0 (mute) */
	0x0123b000,	/* NID 0x12: Set output amp, L+R, gain 0 (mute) */

	/* Power state D0 (fully on) for DACs and ADCs */
	0x00370500,	/* NID 0x03: Set power state D0 */
	0x00470500,	/* NID 0x04: Set power state D0 */
	0x00570500,	/* NID 0x05: Set power state D0 */
	0x00670500,	/* NID 0x06: Set power state D0 */

	0x00870500,	/* NID 0x08: Set power state D0 */
	0x00970500,	/* NID 0x09: Set power state D0 */
	0x01970500,	/* NID 0x19: Set power state D0 */
	/* EAPD enable on internal speaker */
	0x01270c02,	/* NID 0x12: Set EAPD/BTL enable = 0x02 (EAPD on) */
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Analog Devices AD1984",
		.vendor_id    = 0x11d41984,
		.subsystem_id = 0x17aa20d6,
		.address      = 0,
		.verbs        = ad1984_verbs,
		.verb_count   = ARRAY_SIZE(ad1984_verbs),
	},
};

AZALIA_ARRAY_SIZES;
